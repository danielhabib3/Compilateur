
int main() {

    int c = 4; 
    int a = c * 5; 
    int b = c; 
    int i = 1;

    while (i < a + 1) {
        int j = 0;
        while (j < a - i) {
            putchar(' ');
            j++;
        }
        int k = 0;
        while (k < (2 * i - 1)) {
            putchar('*');
            k++;
        }
        putchar(10);
        i++;
    }

    int p = 0;
    while (p < b) {
        int j = 0;
        while (j < a - c / 2) {
            putchar(' ');
            j++;
        }
        int k = 0;
        while (k < c) {
            putchar('|');
            k++;
        }
        putchar(10);
        p++;
    }

    return 0;
}