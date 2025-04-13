

int main() {
    int arr[5] = {12, 11, 13, 5, 6};
    int n = 5;

    int i = 1;
    while (i < n) {
        int key = arr[i];
        int j = i - 1;

        while (j > -1) {
            if(!(arr[j] > key)) {
                break;
            }
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
        i++;
    }  
    return arr[0];
}