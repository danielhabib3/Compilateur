int main() {
    int i = 0;
    int c = 0;
    int d = 0;
    while (i < 3) {
        int j = 0;
        while (j < 3) {
            if (j == 1) {
                j = j+1;
                continue; 
            }

            c++;
            j = j+1;
        }
        i = i+1;
        d++;
    }
    return c+d;
}