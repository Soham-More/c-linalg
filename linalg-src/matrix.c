#include "../linalg.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>

// LINALG_UNPACK_MAT(matrix, r, c)[x][y] = value at xth col and yth row
#define LA_UNPACK(matrix) ((double (*)[matrix.cols]) matrix.mat)

#define LA_UNPACK_PTR(matrix) ((double (*)[matrix->cols]) matrix->mat)

#define LA_UNPACK_ROW(matrix, row) mat2DRow(matrix, row)
#define LA_UNPACK_COL(matrix, col) mat2DCol(matrix, col)

// initialzie the matrix on the heap with some initial value
Mat2d mat2DInitA(double value, size_t rows, size_t cols)
{
    if(rows == 0 || cols == 0)
    {
        LINALG_REPORT_ERROR("invalid zero row or col matrix requested!");
        return (Mat2d){ NULL, 0, 0 };
    }
    Mat2d mat = { (double*)calloc(rows*cols, sizeof(double)), rows, cols };
    LINALG_ASSERT_ERROR(!mat.mat, mat, "unkown error occured when allocation memory!");
    for(size_t i = 0; i < mat.cols*mat.rows; i++) mat.mat[i] = value;

    return mat;
}
// initialize the matrix on the heap to zeros
Mat2d mat2DInitZerosA(size_t rows, size_t cols)
{
    return mat2DInitA(0.0, rows, cols);
}
// initialize the matrix on the heap to ones
Mat2d mat2DInitOnesA(size_t rows, size_t cols)
{
    return mat2DInitA(1.0, rows, cols);
}

// make a copy of a matrix on heap
Mat2d mat2DCopyA(Mat2d matrix)
{
    if(matrix.rows == 0 || matrix.cols == 0)
    {
        LINALG_REPORT_ERROR("invalid zero row or col matrix requested!");
        return (Mat2d){ NULL, 0, 0 };
    }
    else if(matrix.mat == NULL)
    {
        LINALG_REPORT_ERROR("invalid matrix pointer(null)!");
        return (Mat2d){ NULL, 0, 0 };
    }
    Mat2d mat = { (double*)calloc(matrix.rows*matrix.cols, sizeof(double)), matrix.rows, matrix.cols };
    LINALG_ASSERT_ERROR(!mat.mat, mat, "unkown error occured when allocation memory!");
    for(size_t i = 0; i < mat.cols*mat.rows; i++) mat.mat[i] = matrix.mat[i];
    return mat;
}
// make a copy of a matrix on heap
int mat2DCopy(Mat2d src, Mat2d* dst)
{
    LINALG_ASSERT_ERROR(src.rows == 0 || src.cols == 0, LINALG_ERROR, "invalid zero row or col matrix requested!");
    LINALG_ASSERT_ERROR(!src.mat, LINALG_ERROR, "invalid zero row or col matrix requested!");
    LINALG_ASSERT_ERROR(src.cols == dst->cols && src.rows == dst->rows, LINALG_ERROR, "mat2DCopy arguments do not have same size!");

    for(size_t i = 0; i < src.cols*src.rows; i++) dst->mat[i] = src.mat[i];
    return LINALG_OK;
}

// construct a matrix from a pointer(does not allocate)
Mat2d mat2DConstruct(double* ptr, size_t rows, size_t cols)
{
    Mat2d mat = { ptr, rows, cols };
    return mat;
}

// pretty print a matrix
void mat2DPrint(Mat2d a)
{
    printf("[\n");
    for(size_t i = 0; i < a.rows; i++)
    {
        printf("%2c", ' ');
        vecPrint(LA_UNPACK_ROW(a, i));
        if(i != a.rows - 1) printf(",\n");
        else printf("\n");
    }
    printf("]");
}

// get a row as vector
// Warning: this is a copy by reference
// DO NOT USE after matrix is freed
Vec mat2DRow(Mat2d matrix, size_t row)
{
    // rows are stored continuously in buffer
    Vec vrow = {&(LA_UNPACK(matrix)[row][0]), matrix.cols, 1};
    return vrow;
}
// get a column as vector
// Warning: this is a copy by reference
// DO NOT USE after matrix is freed
Vec mat2DCol(Mat2d matrix, size_t col)
{
    // cols are not stored continuously in buffer, each value is stored at a offset of matrix.cols
    Vec vcol = {&(LA_UNPACK(matrix)[0][col]), matrix.rows, matrix.cols};
    return vcol;
}

// gets the nth value in a matrix(by value)
// checks for out-of-bounds
double mat2DGet(Mat2d a, size_t row, size_t col)
{
    LINALG_ASSERT_ERROR(row >= a.rows, NAN, "out of bounds matrix row access!");
    LINALG_ASSERT_ERROR(col >= a.cols, NAN, "out of bounds matrix col access!");

    return LA_UNPACK(a)[row][col];
}
// gets the nth value in a matrix(by ref)
// checks for out-of-bounds( returns nullptr is performed )
double* mat2DRef(Mat2d a, size_t row, size_t col)
{
    LINALG_ASSERT_ERROR(row >= a.rows, NULL, "out of bounds matrix row access!");
    LINALG_ASSERT_ERROR(col >= a.cols, NULL, "out of bounds matrix col access!");

    return &(LA_UNPACK(a)[row][col]);
}

// add 2 matrixs and get result into another matrix, prints error if input is invalid
int mat2DAdd(Mat2d a, Mat2d b, Mat2d* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->mat, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(a.rows != b.rows || b.cols != a.cols, LINALG_ERROR, "attempt to add mat(%zux%zu) and mat(%zux%zu)", a.cols, a.rows, b.cols, b.rows);
    LINALG_ASSERT_ERROR(a.rows != result->rows || b.cols != result->cols, LINALG_ERROR, "result matrix is mat(%zux%zu) but inputs are mat(%zux%zu)", result->cols, result->rows, b.cols, b.rows);

    for(size_t i = 0; i < a.rows*a.cols; i++) result->mat[i] = a.mat[i] + b.mat[i];

    return LINALG_OK;
}
// subtract 2 matrixes(a-b) and get result into another matrix, prints error if input is invalid
int mat2DSub(Mat2d a, Mat2d b, Mat2d* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->mat, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(a.rows != b.rows || b.cols != a.cols, LINALG_ERROR, "attempt to add mat(%zux%zu) and mat(%zux%zu)", a.cols, a.rows, b.cols, b.rows);
    LINALG_ASSERT_ERROR(a.rows != result->rows || b.cols != result->cols, LINALG_ERROR, "result matrix is mat(%zux%zu) but inputs are mat(%zux%zu)", result->cols, result->rows, b.cols, b.rows);

    for(size_t i = 0; i < a.rows*a.cols; i++) result->mat[i] = a.mat[i] - b.mat[i];

    return LINALG_OK;
}
// multiply scalar value to matrixs and get result into another matrix, prints error if input is invalid
int mat2DScale(double a, Mat2d b, Mat2d* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->mat, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(b.rows != result->rows || b.cols != result->cols, LINALG_ERROR, "result matrix is mat(%zux%zu) but inputs are mat(%zux%zu)", result->cols, result->rows, b.cols, b.rows);

    for(size_t i = 0; i < b.rows*b.cols; i++) result->mat[i] = a * b.mat[i];

    return LINALG_OK;
}

// compute result = Ax. prints error if the input is invalid
int mat2DTransform(Mat2d A, Vec x, Vec* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->x, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(A.cols != x.len, LINALG_ERROR, "invalid vector: mat(%zux%zu) applied over vec(%zu)", A.rows, A.cols, x.len);
    LINALG_ASSERT_ERROR(A.rows != result->len, LINALG_ERROR, "invalid vector: mat(%zux%zu) applied over vec(%zu) is put in vec(%zu)", A.rows, A.cols, x.len, result->len);

    for(size_t i = 0; i < A.rows; i++)
    {
        double val = 0;
        for(size_t j = 0; j < A.cols; j++) val += LA_UNPACK(A)[i][j] * VEC_INDEX(x, j);
        VEC_INDEX(*result, i) = val;
    }

    return LINALG_OK;
}
// compute result = Ax. prints error if the input is invalid(allocates memory)
Vec mat2DTransformA(Mat2d A, Vec x)
{
    Vec badVec = {NULL, 0, 0};
    LINALG_ASSERT_ERROR(A.cols != x.len, badVec, "invalid vector: mat(%zux%zu) applied over vec(%zu)", A.rows, A.cols, x.len);

    Vec result = vecInitZerosA(A.rows);

    for(size_t i = 0; i < A.rows; i++)
    {
        double val = 0;
        for(size_t j = 0; j < A.cols; j++) val += LA_UNPACK(A)[i][j] * VEC_INDEX(x, j);
        VEC_INDEX(result, i) = val;
    }

    return result;
}

// compute result = A*B. prints error if the input is invalid
int mat2DMul(Mat2d A, Mat2d B, Mat2d* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->mat, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(A.cols != B.rows, LINALG_ERROR, "invalid operation: multiplication between mat(%zux%zu) and mat(%zux%zu)", A.rows, A.cols, B.rows, B.cols);
    LINALG_ASSERT_ERROR(A.rows != result->rows || B.cols != result->cols, LINALG_ERROR, 
                        "invalid operation: multiplication between mat(%zux%zu) and mat(%zux%zu) stored in mat(%zux%zu)", A.rows, A.cols, B.rows, B.cols, result->rows, result->cols);

    for(size_t i = 0; i < A.rows; i++)
    {
        for(size_t j = 0; j < B.cols; j++)
        {
            double res = 0;
            for(size_t k = 0; k < B.rows; k++) res += LA_UNPACK(A)[i][k] * LA_UNPACK(B)[k][j];
            LA_UNPACK_PTR(result)[i][j] = res;
        }
    }

    return LINALG_OK;
}
// compute result = A*B(allocates memory). prints error if the input is invalid
Mat2d mat2DMulA(Mat2d A, Mat2d B)
{
    Mat2d bad_mat = {NULL, 0, 0};
    LINALG_ASSERT_ERROR(A.cols != B.rows, bad_mat, "invalid operation: multiplication between mat(%zux%zu) and mat(%zux%zu)", A.rows, A.cols, B.rows, B.cols);

    Mat2d result = mat2DInitZerosA(A.rows, B.cols);

    for(size_t i = 0; i < A.rows; i++)
    {
        for(size_t j = 0; j < B.cols; j++)
        {
            double res = 0;
            for(size_t k = 0; k < B.rows; k++) res += LA_UNPACK(A)[i][k] * LA_UNPACK(B)[k][j];
            LA_UNPACK(result)[i][j] = res;
        }
    }

    return result;
}

// solve Ax = b using gauss elmination
// scratch space should be nx(n+1) big and order should be n elements big
int mat2DSqSolve(Mat2d A, Vec x, Mat2d* scratch, size_t* order, Vec* y)
{
    LINALG_ASSERT_ERROR(A.rows != A.cols, LINALG_ERROR, "invalid operation: mat2DSq operation on non square matrix mat(%zux%zu)", A.rows, A.cols);
    LINALG_ASSERT_ERROR(scratch->rows + 1 != scratch->cols, LINALG_ERROR, "invalid operation: mat2DSq operation wrong scratch space mat(%zux%zu)", scratch->rows, scratch->cols);

    size_t N = A.rows;

    // make the augmented matrix
    for(size_t i = 0; i < N; i++)
    {
        for(size_t j = 0; j < N; j++)
        {
            LA_UNPACK_PTR(scratch)[i][j] = LA_UNPACK(A)[i][j];
        }
        LA_UNPACK_PTR(scratch)[i][N] = VEC_INDEX(x, i);
        order[i] = i;
    }

    for(size_t h = 0, k = 0; (h < N) && (k < N + 1); k++)
    {
        /* Find the k-th pivot: */
        size_t i_max = (size_t)-1;
        double maxval = 0.0;
        for(size_t i = h; i < N; i++)
        {
            if(maxval < fabs(LA_UNPACK_PTR(scratch)[order[i]][k]))
            {
                maxval = fabs(LA_UNPACK_PTR(scratch)[order[i]][k]);
                i_max = i;
            }
        }

        // no value greater than 1 found in this row
        if(i_max == (size_t)-1) continue;

        //swap rows(h, i_max)
        size_t tmp = order[i_max];
        order[i_max] = order[h];
        order[h] = tmp;

        /* Do for all rows below pivot: */
        for(size_t i = h + 1; i < N; i++)
        {
            double f = LA_UNPACK_PTR(scratch)[order[i]][k] / LA_UNPACK_PTR(scratch)[order[h]][k];
            /* Fill with zeros the lower part of pivot column: */
            LA_UNPACK_PTR(scratch)[order[i]][k] = 0;
            /* Do for all remaining elements in current row: */
            for(size_t j = k + 1; j < N + 1; j++)
            {
                LA_UNPACK_PTR(scratch)[order[i]][j] = LA_UNPACK_PTR(scratch)[order[i]][j] - LA_UNPACK_PTR(scratch)[order[h]][j] * f;
            }
        }
        /* Increase pivot row and column */
        h++;
    }

    // i != (size_t)-1 is done here to remove a compiler warning
    // DO NOT REMOVE "if(i == 0) return LINALG_OK;" 
    // as C standards do not guarantee i warp around
    for(size_t i = N - 1; i != (size_t)-1; i--)
    {
        double sum = LA_UNPACK_PTR(scratch)[order[i]][N];
        for(size_t j = i + 1; j < N; j++)
        {
            sum -= LA_UNPACK_PTR(scratch)[order[i]][j] * VEC_INDEX(*y, j);
        }
        VEC_INDEX(*y, i) = sum / LA_UNPACK_PTR(scratch)[order[i]][i];
        
        if(i == 0) return LINALG_OK;
    }
}

// compute result = A^T. prints error if the input is invalid
int mat2DTranspose(Mat2d A, Mat2d* result)
{
    LINALG_ASSERT_ERROR(!result, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(!result->mat, LINALG_ERROR, "result matrix is null!");
    LINALG_ASSERT_ERROR(A.cols != result->rows || A.rows != result->cols, LINALG_ERROR, "invalid operation: multiplication between mat(%zux%zu) and mat(%zux%zu)", A.rows, A.cols, result->rows, result->cols);

    for(size_t i = 0; i < A.rows; i++)
    {
        for(size_t j = 0; j < A.cols; j++)
        {
            LA_UNPACK_PTR(result)[j][i] = LA_UNPACK(A)[i][j];
        }
    }

    return LINALG_OK;
}

// maximum value in the matrix, prints error if input is invalid
double mat2DMax(Mat2d a)
{
    LINALG_ASSERT_ERROR(!a.mat, NAN, "input matrix is null!");
    LINALG_ASSERT_WARN(a.rows*a.cols == 0, -INFINITY, "input matrix is null!");

    double max_value = -INFINITY;

    for(size_t i = 0; i < a.rows*a.cols; i++) max_value = max_value > a.mat[i] ? max_value : a.mat[i];

    return max_value;
}
// minimum value in the matrix, prints error if input is invalid
double mat2DMin(Mat2d a)
{
    LINALG_ASSERT_ERROR(!a.mat, NAN, "input matrix is null!");
    LINALG_ASSERT_WARN(a.rows*a.cols == 0, INFINITY, "input matrix is null!");

    double min_value = INFINITY;

    for(size_t i = 0; i < a.rows*a.cols; i++) min_value = min_value < a.mat[i] ? min_value : a.mat[i];

    return min_value;
}

// free the matrix on the heap
void freeMat2D(Mat2d* mat)
{
    if(!mat->mat) return;

    free(mat->mat);
    mat->rows = 0;
    mat->cols = 0;
}

MatTriDiag triDiagInitA(double value, size_t n)
{
    MatTriDiag mat;
    mat.diagonal = vecInitA(value, n);
    mat.subdiagonal = vecInitA(value, n);
    mat.superdiagonal = vecInitA(value, n);

    mat.scratch = vecInitA(value, n);

    return mat;
}
MatTriDiag triDiagInitZeroA(size_t n)
{
    return triDiagInitA(0, n);
}

// add 2 tridiagonal matrixes and get result into another tridiagonal matrix, prints error if input is invalid
int triDiagAdd(MatTriDiag a, MatTriDiag b, MatTriDiag* result)
{
    vecAdd(a.diagonal, b.diagonal, &result->diagonal);
    vecAdd(a.subdiagonal, b.subdiagonal, &result->subdiagonal);
    vecAdd(a.superdiagonal, b.superdiagonal, &result->superdiagonal);

    return LINALG_OK;
}
// subtract 2 tridiagonal matrixes (a - b) and get result into another tridiagonal matrix, prints error if input is invalid
int triDiagSub(MatTriDiag a, MatTriDiag b, MatTriDiag* result)
{
    vecSub(a.diagonal, b.diagonal, &result->diagonal);
    vecSub(a.subdiagonal, b.subdiagonal, &result->subdiagonal);
    vecSub(a.superdiagonal, b.superdiagonal, &result->superdiagonal);

    return LINALG_OK;
}
// multiply scalar value to tridiagonal matrix and get result into another tridiagonal matrix, prints error if input is invalid
int triDiagScale(double a, MatTriDiag b, MatTriDiag* result)
{
    vecScale(a, b.diagonal, &result->diagonal);
    vecScale(a, b.subdiagonal, &result->subdiagonal);
    vecScale(a, b.superdiagonal, &result->superdiagonal);

    return LINALG_OK;
}

// add vec to diagonal entries
int triDiagAddDiagonalSelf(MatTriDiag* a, Vec diag)
{
    vecAdd(a->diagonal, diag, &a->diagonal);
    return LINALG_OK;
}
// add vec to diagonal entries
int triDiagSubDiagonalSelf(MatTriDiag* a, Vec diag)
{
    vecSub(a->diagonal, diag, &a->diagonal);
    return LINALG_OK;
}

// solve Ax = b using tridiagonal matrix algorithm
void triDiagSolveDestructive(MatTriDiag* A, Vec* x)
{
    VEC_INDEX(A->scratch, 0) = VEC_INDEX(A->superdiagonal, 0) / VEC_INDEX(A->diagonal, 0);
    VEC_INDEX(*x, 0) = VEC_INDEX(*x, 0) / VEC_INDEX(A->diagonal, 0);

    for (size_t ix = 1; ix < A->diagonal.len; ix++)
    {
        if (ix < A->diagonal.len-1)
        {
            VEC_INDEX(A->scratch, ix) = VEC_INDEX(A->superdiagonal, ix) / (VEC_INDEX(A->diagonal, ix) - VEC_INDEX(A->subdiagonal, ix) * VEC_INDEX(A->scratch, ix - 1));
        }
        VEC_INDEX(*x, ix) = (VEC_INDEX(*x, ix) - VEC_INDEX(A->subdiagonal, ix) * VEC_INDEX(*x, ix - 1)) / (VEC_INDEX(A->diagonal, ix) - VEC_INDEX(A->subdiagonal, ix) * VEC_INDEX(A->scratch, ix-1));
    }

    for (size_t ix = A->diagonal.len - 2; ix > 0; ix--)
    {
        VEC_INDEX(*x, ix) -= VEC_INDEX(A->scratch, ix) * VEC_INDEX(*x, ix + 1);
    }
    VEC_INDEX(*x, 0) -= VEC_INDEX(A->scratch, 0) * VEC_INDEX(*x, 1);
}

void freeMatTriDiag(MatTriDiag* mat)
{
    freeVec(&mat->diagonal);
    freeVec(&mat->subdiagonal);
    freeVec(&mat->superdiagonal);
    freeVec(&mat->scratch);
}

Vec2 vec2Add(Vec2 a, Vec2 b)
{
    Vec2 r;
    r.x[0] = a.x[0] + b.x[0];
    r.x[1] = a.x[1] + b.x[1];
    return r;
}
Vec2 vec2Sub(Vec2 a, Vec2 b)
{
    Vec2 r;
    r.x[0] = a.x[0] - b.x[0];
    r.x[1] = a.x[1] - b.x[1];
    return r;
}

Block2 blkInit(double value)
{
    Block2 mat;
    mat.mat[0][0] = value;
    mat.mat[0][1] = value;
    mat.mat[1][0] = value;
    mat.mat[1][1] = value;
    return mat;
}
Block2 blkInitZeros()
{
    return blkInit(0.0);
}

Block2 blkAdd(Block2 A, Block2 B)
{
    Block2 mat;
    mat.mat[0][0] = A.mat[0][0] + B.mat[0][0];
    mat.mat[0][1] = A.mat[0][1] + B.mat[0][1];
    mat.mat[1][0] = A.mat[1][0] + B.mat[1][0];
    mat.mat[1][1] = A.mat[1][1] + B.mat[1][1];
    return mat;
}
Block2 blkSub(Block2 A, Block2 B)
{
    Block2 mat;
    mat.mat[0][0] = A.mat[0][0] - B.mat[0][0];
    mat.mat[0][1] = A.mat[0][1] - B.mat[0][1];
    mat.mat[1][0] = A.mat[1][0] - B.mat[1][0];
    mat.mat[1][1] = A.mat[1][1] - B.mat[1][1];
    return mat;
}
Block2 blkMul(Block2 A, Block2 B)
{
    Block2 mat;
    mat.mat[0][0] = A.mat[0][0] * B.mat[0][0] + A.mat[0][1] * B.mat[1][0];
    mat.mat[0][1] = A.mat[0][0] * B.mat[0][1] + A.mat[0][1] * B.mat[1][1];
    mat.mat[1][0] = A.mat[1][0] * B.mat[0][0] + A.mat[1][1] * B.mat[1][0];
    mat.mat[1][1] = A.mat[1][0] * B.mat[0][1] + A.mat[1][1] * B.mat[1][1];
    return mat;
}
Block2 blkInverse(Block2 A)
{
    double det = blkDeterminant(A);
    Block2 mat;
    mat.mat[0][0] =  A.mat[1][1] / det;
    mat.mat[0][1] = -A.mat[0][1] / det;
    mat.mat[1][0] = -A.mat[1][0] / det;
    mat.mat[1][1] =  A.mat[0][0] / det;
    return mat;
}

Vec2 blkTransform(Block2 A, Vec2 x)
{
    Vec2 y;
    y.x[0] = A.mat[0][0] * x.x[0] + A.mat[0][1] * x.x[1];
    y.x[1] = A.mat[1][0] * x.x[0] + A.mat[1][1] * x.x[1];
    return y;
}

double blkDeterminant(Block2 A)
{
    return A.mat[0][0] * A.mat[1][1] - A.mat[0][1] * A.mat[1][0];
}

MatBlock2TD blkTriDiagInitA(double value, size_t n)
{
    MatBlock2TD matBlk2;
    matBlk2.len = n;
    matBlk2.diagonal = malloc(matBlk2.len * sizeof(Block2));
    matBlk2.subdiagonal = malloc(matBlk2.len * sizeof(Block2));
    matBlk2.superdiagonal = malloc(matBlk2.len * sizeof(Block2));
    matBlk2.scratch = malloc(matBlk2.len * sizeof(Block2));

    for(size_t i = 0; i < n; i++)
    {
        matBlk2.diagonal[i] = blkInit(value);
        matBlk2.subdiagonal[i] = blkInit(value);
        matBlk2.superdiagonal[i] = blkInit(value);
        matBlk2.scratch[i] = blkInit(value);
    }
}
MatBlock2TD blkTriDiagInitZeroA(size_t n)
{
    return blkTriDiagInitA(0.0, n);
}

// solve Ax = b using block tridiagonal matrix algorithm
void blkTriDiagSolveSelf(MatBlock2TD* A, Vec2* x)
{
    Block2 inv = blkInverse(A->diagonal[0]);

    A->scratch[0] = blkMul(inv, A->superdiagonal[0]);
    x[0] = blkTransform(inv, x[0]);

    for (size_t ix = 1; ix < A->len; ix++)
    {
        Block2 Factor = blkSub(A->diagonal[ix], blkMul(A->subdiagonal[ix], A->scratch[ix-1]));
        Block2 invFactor = blkInverse(Factor);

        if (ix < A->len-1)
        {
            A->scratch[ix] = blkMul(invFactor, A->superdiagonal[ix]);
        }
        Vec2 delX = vec2Sub(x[ix], blkTransform(A->subdiagonal[ix], x[ix - 1]));
        x[ix] = blkTransform(invFactor, delX);
    }

    for (size_t ix = A->len - 2; ix > 0; ix--)
    {
        x[ix] = vec2Sub(x[ix], blkTransform(A->scratch[ix], x[ix + 1]));
    }
    x[0] = vec2Sub(x[0], blkTransform(A->scratch[0], x[1]));
}

void freeMatBlock2TD(MatBlock2TD* mat)
{
    free(mat->superdiagonal);
    free(mat->diagonal);
    free(mat->subdiagonal);
    free(mat->scratch);

    mat->diagonal = NULL;
    mat->subdiagonal = NULL;
    mat->superdiagonal = NULL;
    mat->scratch = NULL;
}

