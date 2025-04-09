int main() {
    int arr[3] = {10, 20, 30};
    if (arr[2] == 30) {
        arr[1] = arr[0];
    }
    return arr[1];
}