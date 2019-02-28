#include <stdio.h>
#include "gobs_location.h"

void dgesvi_(int* n, int* nrhs,
	double* a, int* lda, int** ipiv,
	double* b, int* ldb, int *info);
int cal_location(Point p1, Point p2, Point p3, float t1, float t2, float t3, float watervel,Point *p_obs);

int main()
{
    Point p1,p2,p3,p4, p_obs;
    p1.x=10;p1.y=10;p1.z=0.0;
    p2.x=30;p2.y=10;p2.z=0.0;
    p3.x=10;p3.y=30;p3.z=0.0;
    p4.x=30;p4.y=30;p4.z=0.0;

    float t1, t2, t3 ,t4;

    t1=0.01;t2=0.0166666667;t3=0.0166666667;t4=0.01;

    float watervel= 1500.;

    int info=cal_location(p1, p2, p3, t1, t2, t3, watervel,&p_obs);

    if(0==info)
        printf("obs:x=%f,y=%f,z=%f\n",p_obs.x, p_obs.y, p_obs.z);
    else
        printf("info=%d,cal error!\n",info);
}

int cal_location(Point p1, Point p2, Point p3, float t1, float t2, float t3, float watervel,Point *p_obs)
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
}

/*
int main()
{
    int N = 4;
    double A[16] = {  1,  2,  3,  1,
                      4,  2,  0,  2,
                     -2,  0, -1,  2,
                      3,  4,  2, -3};
    double B[8] = {  6,  2,  1,  8,
                     1,  2,  3,  4};
    int ipiv[4];
    int n = N;
    int nrhs = 2;
    int lda = N;
    int ldb = N;
    int info;

    dgesv_(&n,&nrhs,A,&lda,ipiv,B,&ldb,&info);
    printf("info:%d\n",info);
    if(info==0)
    {
        int i = 0;
        int j = 0;
        for(j=0;j<nrhs;j++)
        {
            printf("x%d\n",j);
            for(i=0;i<N;i++)
                printf("%.6g \t",B[i+j*N]);
            printf("\n");
        }
    }
}
*/
