int function1();

int function1() {
    int a = 1;
    if(a == 1) {
        a = 2;
    }
    else {
        a = 3;
    }
    return a;
}
int main() {
    int b = function1();
    if(b == 2) {
        b = 4;
    }
    else {
        b = 5;
    }
    return b;
}