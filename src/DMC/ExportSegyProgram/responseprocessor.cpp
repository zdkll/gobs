#include "responseprocessor.h"

#define PI              3.141592653

//最小和最大频率
#define  MIN_F       3
#define MAX_F       80

#include <complex>
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;

bool ResponseProcessor::preProcess(int n, float dt)
{
    m_inNum = n;//输入个数
    m_outNum = (m_inNum/2)+1;//输出个数
    m_dt = dt;

    float df = 1/(n*dt); //频率间隔
    //f1 -f2 :f 限制
    m_nf1 = MIN_F/df;
    m_nf2 =std::min<int>(MAX_F/df, m_outNum-1);
    if(m_nf2<m_nf1 || m_nf1>=(m_outNum-1))
        return false;

    //开辟空间
    m_data = new double[n];
    m_dataComplex = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_outNum);

    //构造好fftw_plan
    m_r2cPlan = fftw_plan_dft_r2c_1d(m_inNum,m_data,m_dataComplex,FFTW_MEASURE);
    m_c2rPlan = fftw_plan_dft_c2r_1d(m_outNum,m_dataComplex,m_data,FFTW_MEASURE);

    //构造Hs
    m_hs =  new std::complex<double>[m_nf2 - m_nf1 + 1];
    int nh = 0;
    for(int i=m_nf1;i<=m_nf2;i++){
        double s = 2*PI*(df*i);

        complex<double> fac1(359.4,s);
        complex<double> fac2(27.18,s);
        complex<double> fac3(24.72,s);
        complex<double> fac4(10.42,s);
        complex<double> fac5(358.7,s);
        complex<double> fac6(40.7,s);

        complex<double> cs(0,s);
        m_hs[nh] = (fac1*fac2*fac3*fac4)/(497.0033*(cs*cs)*fac5*fac6) ;

        nh++;
    }

    return true;
}

void ResponseProcessor::fftwResponseProcess(double *data)
{
    //拷贝到输入数据
    memcpy(m_data,data,sizeof(double)*m_inNum);

    //FFTW变换
    fftw_execute(m_r2cPlan);

    //m_dataComplex[0:m_nf1) = 0;m_dataComplex(m_nf2:m_outNum] = 0
    int n1 = std::min(m_nf1,m_outNum);
    if(n1>0)
        memset(m_dataComplex,0,(n1-1)*sizeof(fftw_complex));
    if(m_outNum>m_nf2){
        memset(m_dataComplex+m_nf2+1,0,(m_outNum-m_nf2)*sizeof(fftw_complex));
    }

    //m_dataComplex*hs
    int nh = 0;
    std::complex<double> temp;
    for(int i=m_nf1;i<=m_nf2;i++){
        temp.real(m_dataComplex[i][0]);
        temp.imag(m_dataComplex[i][1]);
        temp*=m_hs[nh];

        m_dataComplex[i][0] = temp.real();
        m_dataComplex[i][1] = temp.imag();

        nh++;
    }

    //FFTW逆变换
    fftw_execute(m_c2rPlan);

    //拷贝到输出
    memcpy(data,m_data,sizeof(double)*m_inNum);
}

void ResponseProcessor::postProcess()
{
    fftw_destroy_plan(m_r2cPlan);
    fftw_destroy_plan(m_c2rPlan);

    delete[] m_data;
    fftw_free(m_dataComplex);
}

