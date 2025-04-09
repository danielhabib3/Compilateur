int main() {
    int mat[2][2] = {{1, 2}, {3, 4}};
    if (mat[0][1] < mat[1][0]) {
        mat[1][1] = mat[0][0] + mat[1][0];
    }
    return mat[1][1];
}