int main() {
    int a[3] = {1, 2, 3};
    int b[3];
    int i = 0;
    while (i < 3) {
        b[i] = a[2 - i];
        i = i + 1;
    }
    return b[0];
}