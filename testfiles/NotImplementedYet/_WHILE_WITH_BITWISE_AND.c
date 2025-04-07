int main() {
    int i = 0;
    while ((i & 1) == 0 && i < 4) {
        i = i + 1;
    }
    return i;
}
