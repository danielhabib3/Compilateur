int main() {
    int arr[3] = {100, 200, 300};
    if (arr[2] > 250) {
        arr[1] = arr[0] - arr[2];
    }
    return arr[0];
}