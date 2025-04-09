int main() {
    int arr[2] = {7, 8};
    if (arr[0] < arr[1]) {
        arr[1] = arr[1] - arr[0];
    }
    return arr[1];
}