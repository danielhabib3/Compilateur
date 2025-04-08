int main() {
    int a[4] = {1, 2, 3, 4};
    int sum = 0;
    int i = 0;
    while (i < 4) {
        sum = sum + a[i];
        i = i + 1;
    }
    a[0] = sum;
    return a[0];
}