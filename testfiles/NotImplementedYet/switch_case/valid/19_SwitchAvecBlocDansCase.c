int main() {
    int x = 3;
    switch (x) {
        case 3: {
            x = x * 2;
            break;
        }
        default:
            x = 0;
    }
    return x;
}