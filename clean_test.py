import os
import sys


def extract_test_numbers(file_paths):
    pattern = re.compile(r'/(?<!_)\b(\d+)_[^/]+\.c$')
    test_numbers = sorted(set(map(int, pattern.findall("\n".join(file_paths)))))
    return test_numbers

orig_cwd=os.getcwd()
os.chdir(orig_cwd)
inputfilenames=[]
path='testfiles'
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


for inputfilename in inputfilenames:
    old_path = inputfilename
    # if(len(extract_test_numbers([old_path])) == 0):

    # Extraire le dossier et le nom du fichier
    dir_name = os.path.dirname(old_path)
    filename = os.path.basename(old_path)

    # Enlever tout jusqu'à avoir un underscore, mais garder le underscore
    new_filename = '_' + filename.split('_', 1)[-1]
    # Nouveau chemin complet
    new_path = os.path.join(dir_name, new_filename)

    # Renommer le fichier
    os.rename(old_path, new_path)