
int factoriel(int n) {
    if(n < 2) {
        if(n < 0) {
            return 0; 
        }
        return 1;
    }
    int a = factoriel(n - 1);
    int z = n * a;
    return z;
}

int main() {

    return factoriel(2);
}
