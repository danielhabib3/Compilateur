int main() {
    int a = 0;
    int b = 1;
    int c = 0;

    if ((a || b) && (c = 2)) {
        return c;
    }

    return 0;
}