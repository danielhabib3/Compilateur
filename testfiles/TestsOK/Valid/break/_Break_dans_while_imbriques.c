int main() {
    int i = 0;
    int c = 0;
    while (i < 5) {
        int j = 0;
        c++;
        while (j < 5) {
            c++;
            if (j == 2) {
                break; 
            }
            j = j+1;
        }
        i = i+1;
    }
    return c;
}
