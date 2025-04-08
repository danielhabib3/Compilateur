int main() {
    int arr[4] = {1, 2, 3, 4};
    int i = 0;
    while (i < 4) {
        arr[i] = arr[i] + 1;
        i = i + 1;
    }
    return arr[3];
}