int main() {
    int n = 5;
    int result;

    if (n < 0) {
        result = -1;
    } else {
        result = 1;
        while (n > 0) {
            result = result * n;
            n = n - 1;
        }
    }

    return result;
}