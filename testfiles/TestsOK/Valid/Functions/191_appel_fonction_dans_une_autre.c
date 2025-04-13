int f() {
    return 42;
}

int g() {
    return f();
}
int main() {
    return g();
}