int main() {
    int i = 0;
    int c = 0;
    
    while (i < 5) {
        i = i + 1;
        c++;
        continue;
    }
    return c;
}