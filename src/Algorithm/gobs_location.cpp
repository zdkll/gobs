#include "gobs_location.h"
#include "lapacke.h"

int cal_location(Point p1, Point p2, Point p3
                                        , float t1, float t2, float t3, float watervel,Point *p_obs)
{
    int N = 2;

    float d1, d2, d3, r1, r2, r3;

    d1= p1.x*p1.x + p1.y*p1.y + p1.z*p1.z;
    d2= p2.x*p2.x + p2.y*p2.y + p2.z*p2.z;
    d3= p3.x*p3.x + p3.y*p3.y + p3.z*p3.z;

    r1=watervel*t1;
    r2=watervel*t2;
    r3=watervel*t3;
    r1*=r1;
    r2*=r2;
    r3*=r3;

    double A[4] = {  p2.x-p1.x,  p3.x-p1.x,
                     p2.y-p1.y,  p3.y-p1.y};
    double B[2] = {  0.5*(d2-d1+r1-r2), 0.5*(d3-d1+r1-r3) };
//    printf("A=%f, %f, %f, %f\n",A[0],A[1],A[2],A[3]);
//    printf("B=%f, %f\n",B[0],B[1]);

    int ipiv[2];
    int n = N;
    int nrhs = 1;
    int lda = N;
    int ldb = N;
    int info;

    dgesv_(&n,&nrhs,A,&lda,ipiv,B,&ldb,&info);
    (*p_obs).x=B[0];
    (*p_obs).y=B[1];
    (*p_obs).z=p1.z-sqrt(r1-(B[0]-p1.x)*(B[0]-p1.x)-(B[1]-p1.y)*(B[1]-p1.y));

    return info;

    return info;
}


/*****************以下为测试程序代码*****************************/
#if 0

/*
   LAPACKE_dgesv Example.
   ======================

   The program computes the solution to the system of linear
   equations with a square matrix A and multiple
   right-hand sides B, where A is the coefficient matrix:

     6.80  -6.05  -0.45   8.32  -9.67
    -2.11  -3.30   2.58   2.71  -5.14
     5.66   5.36  -2.70   4.35  -7.26
     5.97  -4.44   0.27  -7.17   6.08
     8.23   1.08   9.04   2.14  -6.87

   and B is the right-hand side matrix:

     4.02  -1.56   9.81
     6.19   4.00  -4.09
    -8.22  -8.67  -4.57
    -7.57   1.75  -8.61
    -3.03   2.86   8.99

   Description.
   ============

   The routine solves for X the system of linear equations A*X = B,
   where A is an n-by-n matrix, the columns of matrix B are individual
   right-hand sides, and the columns of X are the corresponding
   solutions.

   The LU decomposition with partial pivoting and row interchanges is
   used to factor A as A = P*L*U, where P is a permutation matrix, L
   is unit lower triangular, and U is upper triangular. The factored
   form of A is then used to solve the system of equations A*X = B.

   Example Program Results.
   ========================

 LAPACKE_dgesv (row-major, high-level) Example Program Results

 Solution
  -0.80  -0.39   0.96
  -0.70  -0.55   0.22
   0.59   0.84   1.90
   1.32  -0.10   5.36
   0.57   0.11   4.04

 Details of LU factorization
   8.23   1.08   9.04   2.14  -6.87
   0.83  -6.94  -7.92   6.55  -3.99
   0.69  -0.67 -14.18   7.24  -5.19
   0.73   0.75   0.02 -13.82  14.19
  -0.26   0.44  -0.59  -0.34  -3.43

 Pivot indices
      5      5      3      4      5
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>

#include "lapacke.h"

/* Auxiliary routines prototypes */
extern void print_matrix( char* desc, lapack_int m, lapack_int n, double* a, lapack_int lda );
extern void print_int_vector( char* desc, lapack_int n, lapack_int* a );

/* Parameters */
#define N 5
#define NRHS 3
#define LDA N
#define LDB NRHS

int main(int argc, char *argv[])
{
    /* Locals */
    lapack_int n = N, nrhs = NRHS, lda = LDA, ldb = LDB, info;

    /* Local arrays */
    lapack_int ipiv[N];
    double a[LDA*N] = {
        6.80, -6.05, -0.45,  8.32, -9.67,
        -2.11, -3.30,  2.58,  2.71, -5.14,
        5.66, 5.36, -2.70,  4.35, -7.26,
        5.97, -4.44,  0.27, -7.17, 6.08,
        8.23, 1.08,  9.04,  2.14, -6.87
    };
    double b[LDB*N] = {
        4.02, -1.56, 9.81,
        6.19,  4.00, -4.09,
        -8.22, -8.67, -4.57,
        -7.57,  1.75, -8.61,
        -3.03,  2.86, 8.99
    };
    /* Print Entry Matrix */
    print_matrix( "Entry Matrix A", n, n, a, lda );
    /* Print Right Rand Side */
    print_matrix( "Right Rand Side", n, nrhs, b, ldb );
    printf( "\n" );
    /* Executable statements */
    printf( "LAPACKE_dgesv (row-major, high-level) Example Program Results\n" );
    /* Solve the equations A*X = B */

    info = LAPACKE_dgesv( LAPACK_ROW_MAJOR, n, nrhs, a, lda, ipiv,
                          b, ldb );

    for( int j = 0; j < N; j++ ) cout<<ipiv[j]<<" "<<endl;

    /* Check for the exact singularity */
    if( info > 0 ) {
        printf( "The diagonal element of the triangular factor of A,\n" );
        printf( "U(%i,%i) is zero, so that A is singular;\n", info, info );
        printf( "the solution could not be computed.\n" );
        exit( 1 );
    }

    /* Print solution */
    print_matrix( "Solution", n, nrhs, b, ldb );
    /* Print details of LU factorization */
    print_matrix( "Details of LU factorization", n, n, a, lda );
    /* Print pivot indices */
    print_int_vector( "Pivot indices", n, ipiv );
    exit( 0 );
}

void print_matrix( char* desc, lapack_int m, lapack_int n, double* a, lapack_int lda ) {
    lapack_int i, j;
    printf( "\n %s\n", desc );
    for( i = 0; i < m; i++ ) {
        for( j = 0; j < n; j++ ) printf( " %6.2f", a[i*lda+j] );
        printf( "\n" );
    }
}

/* Auxiliary routine: printing a vector of integers */
void print_int_vector( char* desc, lapack_int n, lapack_int* a ) {
    lapack_int j;
    printf( "\n %s\n", desc );
    for( j = 0; j < n; j++ ) printf( " %6i", a[j] );
    printf( "\n" );
}


#endif


