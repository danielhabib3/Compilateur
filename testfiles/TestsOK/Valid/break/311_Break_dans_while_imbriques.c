int main() {
    int i = 0;
    while (i < 5) {
        int j = 0;
        while (j < 5) {
            if (j == 2) {
                break; 
            }
            j = j+1;
        }
        i = i+1;
    }
    return 0;
}
