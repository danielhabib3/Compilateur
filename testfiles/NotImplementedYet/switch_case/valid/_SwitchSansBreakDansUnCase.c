int main() {
    int x = 1;
    switch (x) {
        case 1:
            x = x + 1;
        case 2:
            x = x + 2;
            break;
        default:
            x = 0;
    }
    return x;
}