int main() {
    int x = 7;
    switch (x) {
        case 5:
            x = 9;
            break;
        default:
            x = 10;
    }
    return x;
}