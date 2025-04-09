int main() {
    int tab[3] = {9, 8, 7};
    if (tab[0] > tab[2]) {
        tab[2] = tab[1];
    }
    return tab[2];
}