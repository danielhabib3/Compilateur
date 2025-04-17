int main() {
    int a = 1;
    int b = 0;
    int c = 0;

    if (a || (b = 2)) {
        c = 1;
    } else {
        c = 2;
    }

    return c;
}