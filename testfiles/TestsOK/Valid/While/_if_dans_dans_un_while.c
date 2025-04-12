int main() {
    int i = 1;
    while (i < 4) {
        while(i < 4) {
            i = i + 1;
            if (i == 2) {
                i = i + 1;
            }
        }
        i = i + 1;
    }
    return i;
}