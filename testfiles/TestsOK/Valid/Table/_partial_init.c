int main() {
    int arr[5] = {2, 3};
    if (arr[3] == 0) {
        arr[3] = arr[0] + arr[1];
    }
    return arr[3];
}