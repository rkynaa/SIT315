// notation: i - row, j - column
kernel void matrix_mul(global float *A, global float *B, global float *C, int n) {
    int i = get_global_id(1);
    int j = get_global_id(0);
    int k;
    float c = 0;
    if (i > n) return;
    if (j > n) return;
    for (k = 0; k < n; k++) {
        c = c + A [i * n + k] * B [k * n + j];
    }
    C [i * n + j] = c;
}