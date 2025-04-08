int main() {
    int i = 0;
    int b = 0;

    while (i < 10) {
        
        if(i==5){
            if(b==0){
                break;
            }
        }

        i = i+1;
    }
    
    return i;
}
