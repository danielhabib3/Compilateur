int main() {
    int a = 2;
    int res = 0;
    switch (a) {
        case 1:
            res = res + 10;
        case 2:
            res = res + 20;
            break;
        default:
            res = -1;
    }
    return res;
}