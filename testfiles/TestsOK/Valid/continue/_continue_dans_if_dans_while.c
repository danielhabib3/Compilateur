int main() {
    int i = 0;
    while (i < 5) {
        if (i == 2) {
            i = i + 1;
            continue;
        }
        i = i + 1;
    }
    return i;
}