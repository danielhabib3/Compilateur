int main() {
    int arr[3];
    arr[0] = 4;
    arr[1] = 5;
    arr[2] = 6;
    if (arr[1] > arr[0]) {
        arr[0] = arr[2];
    }
    return arr[0];
}