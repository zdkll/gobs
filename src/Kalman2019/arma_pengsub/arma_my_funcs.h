
/*--------------------------------------------------------------------------------*/
template<typename T>
int SetRow_peng(Col<T> & v, Mat<T> & M, int I_row)
{
    int n1 = M.n_rows;
    int n2 = M.n_cols;
    int nv = v.n_elem;
    if (nv != n2)
    {
        printf("GetRow error ! nv,n2=%d %d \n", nv,n2);
        flush_std;
        exit(-1);
    }
    for (int i=0;i<n2;i++)
    {
        M(I_row,i) = v[i];
    }
    return 0;
}

/*--------------------------------------------------------------------------------*/
template<typename T>
int GetRow_peng(Col<T> & v, Mat<T> & M, int I_row)
{
    int n1 = M.n_rows;
    int n2 = M.n_cols;
    int nv = v.n_elem;
    if (nv != n2)
    {
        printf("GetRow error ! nv,n2=%d %d \n", nv,n2);
        flush_std;
        exit(-1);
    }
    for (int i=0;i<n2;i++)
    {
        v[i] = M(I_row,i);
    }
    return 0;
}
/*--------------------------------------------------------------------------------*/
template<typename T>
void DoColWeighting(Mat<T> & A, const fvec & w,int ind0,int inde)
{
    int n1 = A.n_rows;
    int n2 = A.n_cols;
    int nw = w.n_elem;
    int n_use = inde - ind0 + 1;
    if (nw != n_use)
    {
        printf("DoColWeighting: size mis-match   \n");
        printf("A[%dx%d] , w[%d]  ind0,inde,n_use=%d,%d,%d \n",\
                n1,n2,nw,ind0,inde,n_use);
    }
    for (int i = 0;i<n_use;i++)
    {
        A.col(i+ind0) *= w(i);
    }
}
/*--------------------------------------------------------------------------------*/
template<typename T>
void DoRowWeighting(Mat<T> & A, const fvec & w,int ind0,int inde)
{
    int n1 = A.n_rows;
    int n2 = A.n_cols;
    int nw = w.n_elem;
    int n_use = inde - ind0 + 1;
    if (nw != n_use)
    {
        printf("DoRowWeighting: size mis-match   \n");
        printf("A[%dx%d] , w[%d]  ind0,inde,n_use=%d,%d,%d \n",\
                n1,n2,nw,ind0,inde,n_use);
    }
    for (int i = 0;i<n_use;i++)
    {
        A.row(i+ind0) *= w(i);
    }
}

