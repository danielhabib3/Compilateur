int main() {
    int x = 2;
    switch (x) {
        case 1:
            x = 10;
            break;
        case 2:
            x = 20;
            break;
        default:
            x = 30;
            break;
    }
    return x;
}