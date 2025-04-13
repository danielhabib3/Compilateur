int main() {
    int arr[7] = {64, 34, 25, 12, 22, 11, 90};
    int n = 7;

    int i, j, temp;
    i = 0;
    while (i < n-1) {
        j = 0;
        while (j < n-i-1) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
            j++;
        }
        i++;
    }

    i = 0;
    while (i < n-1) {
        if (arr[i] > arr[i+1]) {
            return 0; 
        }
        i++;
    }

    return 99; 
}