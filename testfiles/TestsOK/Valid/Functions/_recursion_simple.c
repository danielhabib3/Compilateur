int somme(int n) {
    if (n == 0) {
        return 0;
    }
    return n + somme(n - 1); 
}

int main() {
    int number = 1;
    return somme(number);
}