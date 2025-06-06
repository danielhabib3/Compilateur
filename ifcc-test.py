#!/usr/bin/env python3

# In "multiple-files mode" (by default), this script runs both GCC and
# IFCC on each test-case provided and compares the results.
#
# In "single-file mode", we mimic the CLI behaviour of GCC i.e. we
# interpret the '-o', '-S', and '-c' options.
#
# Run "python3 ifcc-test.py --help" for more info.

import argparse
import glob
import os
import shutil
import sys
import subprocess
import re

def run_command(string, logfile=None, toscreen=False):
    """ execute `string` as a shell command. Maybe write stdout+stderr to `logfile` and/or to the toscreen.
        return the exit status""" 

    if args.debug:
        print("ifcc-test.py: "+string)
    
    process=subprocess.Popen(string,shell=True,
                             stderr=subprocess.STDOUT,stdout=subprocess.PIPE,
                             text=True,bufsize=0)
    if logfile:
        logfile=open(logfile,'w')
    
    while True:
        output = process.stdout.readline()
        if len(output) == 0: # only happens when 'process' has terminated
            break
        if logfile: logfile.write(output)
        if toscreen: sys.stdout.write(output)
    process.wait() # collect child exit status
    assert process.returncode != None # sanity check (I was using poll() instead of wait() previously, and did see some unsanity)
    if logfile:
        logfile.write(f'\nexit status: {process.returncode}\n')
    return process.returncode

def dumpfile(name,quiet=False):
    data=open(name,"rb").read().decode('utf-8',errors='ignore')
    if not quiet:
        print(data,end='')
    return data
    
######################################################################################
## ARGPARSE step: make sense of our command-line arguments

# This is where we decide between multiple-files
# mode and single-file mode

import textwrap
import shutil
width = shutil.get_terminal_size().columns-2
twf=lambda text: textwrap.fill(text,width,initial_indent=' '*4,subsequent_indent=' '*6)
            
argparser   = argparse.ArgumentParser(
formatter_class=argparse.RawDescriptionHelpFormatter,
description = "Testing script for the ifcc compiler. operates in one of two modes:\n\n"
    +twf("- Multiple-files mode (by default): Compile several programs with both GCC and IFCC, run them, and compare the results.",)+"\n\n"
    +twf("- Single-file mode (with options -o,-c and/or -S): Compile and/or assemble and/or link a single program."),
epilog="examples:\n\n"
    +twf("python3 ifcc-test.py testfiles")+'\n'
    +twf("python3 ifcc-test.py path/to/some/dir/*.c path/to/some/other/dir")+'\n'
    +'\n'
    +twf("python3 ifcc-test.py -o ./myprog path/to/some/source.c")+'\n'
    +twf("python3 ifcc-test.py -S -o truc.s truc.c")+'\n'
    ,
)

argparser.add_argument('input',metavar='PATH',nargs='+',help='For each path given:'
                       +' if it\'s a file, use this file;'
                       +' if it\'s a directory, use all *.c files under this subtree')

argparser.add_argument('-v','--verbose',action="count",default=0,
                       help='increase verbosity level. You can use this option multiple times.')
argparser.add_argument('-d','--debug',action="count",default=0,
                       help='increase quantity of debugging messages (only useful to debug the test script itself)')
argparser.add_argument('-S',action = "store_true", help='single-file mode: compile from C to assembly, but do not assemble')
argparser.add_argument('-c',action = "store_true", help='single-file mode: compile/assemble to machine code, but do not link')
argparser.add_argument('-o','--output',metavar = 'OUTPUTNAME', help='single-file mode: write output to that file')

args=argparser.parse_args()

if args.debug >=2:
    print('debug: command-line arguments '+str(args))

orig_cwd=os.getcwd()
if "ifcc-test-output" in orig_cwd:
    print('error: cannot run ifcc-test.py from within its own output directory')
    exit(1)

pld_base_dir=os.path.normpath(os.path.dirname(__file__))
if args.debug:
    print("ifcc-test.py: "+os.path.dirname(__file__))

# cleanup stale output directory
if os.path.isdir(f'{pld_base_dir}/ifcc-test-output'):
    run_command(f'rm -rf {pld_base_dir}/ifcc-test-output')

# Ensure that the `ifcc` executable itself is up-to-date
makestatus=run_command(f'cd {pld_base_dir}/compiler; make --question ifcc')
if makestatus: # updates are needed
    makestatus=run_command(f'cd {pld_base_dir}/compiler; make ifcc',toscreen=True) # this time we run `make` for real
    if makestatus: # if `make` failed, we fail too
        if os.path.exists("ifcc"): # and we remove any out-of-date compiler (to reduce chance of confusion)
            os.unlink("ifcc")
        exit(makestatus)

##########################################
## single-file mode aka "let's act just like GCC (almost)"

if args.S or args.c or args.output:
    if args.S and args.c:
        print("error: options -S and -c are not compatible")
        exit(1)
    if len(args.input)>1:
        print("error: this mode only supports a single input file")
        exit(1)
    inputfilename=args.input[0]
        
    if inputfilename[-2:] != ".c":
        print("error: incorrect filename suffix (should be '.c'): "+inputfilename)
        exit(1)

    try:
        open(inputfilename,"r").close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        exit(1)

    if (args.S or args.c) and not args.output:
        print("error: option '-o filename' is required in this mode")
        exit(1)
        
    if args.S: # produce assembly
        if args.output[-2:] != ".s":
            print("error: output file name must end with '.s'")
            exit(1)
        ifccstatus=run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename} > {args.output}')
        if ifccstatus: # let's show error messages on screen
            exit(run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename}',toscreen=True))
        else:
            exit(0)

    elif args.c: # produce machine code
        if args.output[-2:] != ".o":
            print("error: output file name must end with '.o'")
            exit(1)
        asmname=args.output[:-2]+".s"
        ifccstatus=run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename} > {asmname}')
        if ifccstatus: # let's show error messages on screen
            exit(run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename}',toscreen=True))
        exit(run_command(f'gcc -c -o {args.output} {asmname}',toscreen=True))
        
    else: # produce an executable
        if args.output[-2:] in [".o",".c",".s"]:
            print("error: incorrect name for an executable: "+args.output)
            exit(1)
        asmname=args.output+".s"
        ifccstatus=run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename} > {asmname}')
        if ifccstatus:
            exit(run_command(f'{pld_base_dir}/compiler/ifcc {inputfilename}', toscreen=True))
        exit(run_command(f'gcc -o {args.output} {asmname}'))

    # we should never end up here
    print("unexpected error. please report this bug.")
    exit(1)

# if we were not in single-file mode, then it means we are in
# multiple-files mode.

######################################################################################
## PREPARE step: find and copy all test-cases under ifcc-test-output

## Process each cli argument as a filename or subtree
os.chdir(orig_cwd)
inputfilenames=[]
for path in args.input:
    path=os.path.normpath(path) # collapse redundant slashes etc.
    if os.path.isfile(path):
        if path[-2:] == '.c':
            inputfilenames.append(path)
        else:
            print("error: incorrect filename suffix (should be '.c'): "+path)
            exit(1)
    elif os.path.isdir(path):
        for dirpath,dirnames,filenames in os.walk(path):
            inputfilenames+=[dirpath+'/'+name for name in filenames if name[-2:]=='.c']
    else:
        print("error: cannot read input path `"+path+"'")
        sys.exit(1)

inputfilenames=sorted(inputfilenames)
## debug: after treewalk
if args.debug:
    print("debug: list of files after tree walk:"," ".join(inputfilenames))

## sanity check
if len(inputfilenames) == 0:
    print("error: found no test-case in: "+" ".join(args.input))
    sys.exit(1)

def extract_test_numbers(file_paths):
    pattern = re.compile(r'/(?<!_)\b(\d+)_[^/]+\.c$')
    test_numbers = sorted(set(map(int, pattern.findall("\n".join(file_paths)))))
    return test_numbers

max = 0
# for inputfilename in inputfilenames:
#     if(len(extract_test_numbers([inputfilename])) == 1):
#         num_fichier = extract_test_numbers([inputfilename])[0]
#         if(num_fichier >= max):
#             max = num_fichier
rename = False
for inputfilename in inputfilenames:
    if(len(extract_test_numbers([inputfilename])) == 0):
        rename = True
        break

new_inputfilenames = []
## Check that we actually can read these files. Our goal is to
## fail as early as possible when the CLI arguments are wrong.
for inputfilename in inputfilenames:
    try:
        f=open(inputfilename,"r")

        if rename:
            old_path = inputfilename
            # if(len(extract_test_numbers([old_path])) == 0):

            # Extraire le dossier et le nom du fichier
            dir_name = os.path.dirname(old_path)
            filename = os.path.basename(old_path)

            # Enlever tout jusqu'à avoir un underscore, mais garder le underscore
            filename = '_' + filename.split('_', 1)[-1]

            # Nouveau nom avec un numéro (par exemple 3)
            new_filename = str(max + 1) + filename

            # Nouveau chemin complet
            new_path = os.path.join(dir_name, new_filename)

            # Renommer le fichier
            os.rename(old_path, new_path)

            max = max + 1

            # if(len(extract_test_numbers([old_path])) == 0):
            new_inputfilenames.append(new_path)
            # else:
            #     new_inputfilenames.append(old_path)
        
        f.close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        exit(1)

if rename:
    inputfilenames = new_inputfilenames
## We're going to copy every test-case in its own subdir of ifcc-test-output
os.mkdir(pld_base_dir+'/ifcc-test-output')

jobs=[]

for inputfilename in inputfilenames:
    if args.debug>=2:
        print("debug: PREPARING "+inputfilename)

    if 'ifcc-test-output' in os.path.realpath(inputfilename):
        print('error: input filename is within output directory: '+inputfilename)
        exit(1)

    ## each test-case gets copied and processed in its own subdirectory:
    ## ../somedir/subdir/file.c becomes ifcc-test-output/--somedir-subdir-file/input.c
    subdirname=inputfilename[:-2] # remove the '.c' suffix
    if pld_base_dir in subdirname: # hide "absolute" part of path when not meaningful
        subdirname=subdirname[len(pld_base_dir):]
    subdirname=subdirname.replace('..','-') # keep some punctuation to discern "bla.c" from "../bla.c"
    subdirname=subdirname.replace('./','')  # remove meaningless part of name
    subdirname=subdirname.replace('/','-')  # flatten path to single subdir
    if args.debug>=2:
        print("debug: subdir="+subdirname)
        
    os.mkdir(pld_base_dir+'/ifcc-test-output/'+subdirname)
    shutil.copyfile(inputfilename,pld_base_dir+'/ifcc-test-output/'+subdirname+'/input.c')
    jobs.append(subdirname)

## eliminate duplicate paths from the 'jobs' list
unique_jobs=[]
for j in jobs:
    for d in unique_jobs:
        if os.path.samefile(pld_base_dir+'/ifcc-test-output/'+j,pld_base_dir+'/ifcc-test-output/'+d):
            break # and skip the 'else' branch
    else:
        unique_jobs.append(j)
jobs=sorted(unique_jobs)

# debug: after deduplication
if args.debug:
    print("debug: list of test-cases after PREPARE step:"," ".join(jobs))

######################################################################################
## TEST step: actually compile/link/run each test-case with both compilers.
##
##            if both toolchains agree, this test-case is passed.
##            otherwise, this is a fail.

all_ok=True
last_folder = last_subfolder = ""
current_folder = current_subfolder = ""

special_folders = ["NotImplementedYet", "TestsOK", "WillNeverBeImplemented"]
special_sub_folders = ["TestsOK-Valid","TestsOK-Invalid"]

displayed_categories = set() # Pour savoir si on a déjà affiché un message comme quoi on commence un certain type de test

test_results = {
    "NotImplementedYet": {"success": 0, "failure": 0},
    "TestsOK": {"success": 0, "failure": 0},
    "WillNeverBeImplemented": {"success": 0, "failure": 0}
}

print(f"\nSTART RUNNING TESTS ")

for jobname in jobs:
    os.chdir(f'{pld_base_dir}/ifcc-test-output')
    
    for folder in special_folders:
        if folder in jobname and folder not in displayed_categories:
            last_folder = current_folder
            current_folder = folder
            # Affichage recap last_folder
            if last_folder != "" :
                print(f"\n==== Summary of Tests for {last_folder}  ===")
                print(f"{test_results[last_folder]['success']} success, {test_results[last_folder]['failure']} failures")

            print("\n----------------------------------------------")
            print(f"\nStarting {current_folder}\n")
            displayed_categories.add(folder)
            break

    for subfolder in special_sub_folders:
        if subfolder in jobname and subfolder not in displayed_categories:
            last_subfolder = current_subfolder
            current_subfolder = subfolder

            if last_subfolder != "" : print()

            print(f"Starting {current_subfolder}\n")
            displayed_categories.add(subfolder)
            break

    # Enlever dans le chemin du test qui est affiché la partie correspondant au Folder (car redondant)
    parts = jobname.split("-")  
    # Fixe une largeur pour les noms des tests
    test_case_name = f"TEST-CASE: {parts[-1]}"
    print(f"{test_case_name:<70}", end=" ")  # Ici, <50 indique que le texte sera aligné à gauche avec une largeur de 50 caractères

    
    os.chdir(jobname)
    
    ## Reference compiler = GCC
    gccstatus=run_command("gcc -S -o asm-gcc.s input.c", "gcc-compile.txt")
    if gccstatus == 0:
        # test-case is a valid program. we should run it
        gccstatus=run_command("gcc -o exe-gcc asm-gcc.s", "gcc-link.txt")
    if gccstatus == 0: # then both compile and link stage went well
        exegccstatus=run_command("./exe-gcc", "gcc-execute.txt")
        if args.verbose >=2:
            dumpfile("gcc-execute.txt")
            
    ## IFCC compiler
    ifccstatus=run_command(f'{pld_base_dir}/compiler/ifcc input.c > asm-ifcc.s', 'ifcc-compile.txt')
    
    folder = current_folder

    if gccstatus != 0 and ifccstatus != 0:
        test_results[folder]["success"] += 1
        ## ifcc correctly rejects invalid program -> test-case ok
        print(f"{'TEST OK':<12}")
        continue
    elif gccstatus != 0 and ifccstatus == 0:
        test_results[folder]["failure"] += 1
        ## ifcc wrongly accepts invalid program -> error
        print(f"{'TEST FAIL':<12} (your compiler accepts an invalid program)")
        all_ok=False
        continue
    elif gccstatus == 0 and ifccstatus != 0:
        test_results[folder]["failure"] += 1
        ## ifcc wrongly rejects valid program -> error
        print(f"{'TEST FAIL':<12} (your compiler rejects a valid program)")
        all_ok=False
        if args.verbose:
            dumpfile("asm-ifcc.s")       # stdout of ifcc
            dumpfile("ifcc-compile.txt") # stderr of ifcc
        continue
    else:
        ## ifcc accepts to compile valid program -> let's link it
        ldstatus=run_command("gcc -o exe-ifcc asm-ifcc.s", "ifcc-link.txt")
        if ldstatus:
            test_results[folder]["failure"] += 1
            print(f"{'TEST FAIL':<12} (your compiler produces incorrect assembly)")
            all_ok=False
            if args.verbose:
                dumpfile("asm-ifcc.s")
                dumpfile("ifcc-link.txt")
            continue

    ## both compilers  did produce an  executable, so now we  run both
    ## these executables and compare the results.
        
    run_command("./exe-ifcc", "ifcc-execute.txt")
    if open("gcc-execute.txt").read() != open("ifcc-execute.txt").read() :
        test_results[folder]["failure"] += 1
        print(f"{'TEST FAIL':<12} (different results at execution)")
        all_ok=False

        if args.verbose:
            print("GCC:")
            dumpfile("gcc-execute.txt")
            print("you:")
            dumpfile("ifcc-execute.txt")
        continue

    ## last but not least
    test_results[folder]["success"] += 1
    print(f"{'TEST OK':<12}")

print("\n----------------------------------------------\n")

print("\n==== Summary of Tests ===\n")
for folder, results in test_results.items():
    print(f"{folder}: {results['success']} success, {results['failure']} failures")

if not (all_ok or args.verbose):
    print("\nSome test-cases failed. Run ifcc-test.py with option '--verbose' for more detailed feedback.")
