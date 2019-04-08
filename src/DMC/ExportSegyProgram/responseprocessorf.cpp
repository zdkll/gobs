#include "responseprocessorf.h"

#define PI              3.14159f

//最小和最大频率
#define  MIN_F       3
#define MAX_F       80

#include <complex>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <memory.h>

using namespace std;

bool ResponseProcessorf::preProcess(int n, float dt)
{
    m_inNum = n;//输入个数
    m_outNum = (m_inNum/2)+1;//输出个数
    m_dt = dt;

    float df = 1/(n*dt); //频率间隔
    //f1 -f2 :f 限制
    m_nf1 = MIN_F/df;
    m_nf2 =  m_outNum-1;// 默认最大频率 1/(2*dt) std::min<int>(MAX_F/df, m_outNum-1);
    if(m_nf2<m_nf1 || m_nf1>=(m_outNum-1))
        return false;

    //开辟空间
    m_data = new float[n];
    m_dataComplex = (std::complex<float>*) fftwf_malloc(sizeof(std::complex<float>) * m_outNum);

    //构造好fftwf_plan
    m_r2cPlan = fftwf_plan_dft_r2c_1d(m_inNum,m_data,reinterpret_cast<fftwf_complex*>(m_dataComplex),FFTW_MEASURE);
    m_c2rPlan = fftwf_plan_dft_c2r_1d(m_inNum,reinterpret_cast<fftwf_complex*>(m_dataComplex),m_data,FFTW_MEASURE);

    //构造Hs
    m_hs =  new std::complex<float>[m_nf2 - m_nf1 + 1];
    int nh = 0;
    complex<float> fac1(359.4f,0);
    complex<float> fac2(27.18f,0);
    complex<float> fac3(24.72f,0);
    complex<float> fac4(10.42f,0);
    complex<float> fac5(358.7f,0);
    complex<float> fac6(40.7f,0);
    for(int i=m_nf1;i<=m_nf2;i++){
        complex<float> s(0, 2*PI*(df*i));

        m_hs[nh] = ((s+fac1)*(s+fac2)*(s+fac3)*(s+fac4))/(float(497.0033)*(s*s)*(s+fac5)*(s+fac6)) ;

        nh++;
    }

    return true;
}

void ResponseProcessorf::fftwResponseProcess(float *data)
{
    //拷贝到输入数据
    memcpy(m_data,data,sizeof(float)*m_inNum);

    //FFTW变换
    fftwf_execute(m_r2cPlan);

    //f1-f2 清零;m_dataComplex[0:m_nf1) = 0;m_dataComplex(m_nf2:m_outNum] = 0
    int n1 = std::min(m_nf1,m_outNum-1);//m_outNum 是个数；m_f1,m_nf2是序号
    if(n1>0)
        memset(m_dataComplex,0,(n1)*sizeof(std::complex<float>));
    if(m_outNum-1>m_nf2){
        memset(m_dataComplex+m_nf2+1,0,(m_outNum-m_nf2-1)*sizeof(std::complex<float>));
    }

    //m_dataComplex*hs
    int nh = 0;
    for(int i=m_nf1;i<=m_nf2;i++){
        m_dataComplex[i]*= m_hs[nh];
        nh++;
    }

    //FFTW逆变换
    fftwf_execute(m_c2rPlan);

//    for(int i=0;i<m_inNum;i++){
//        m_data[i] /= m_inNum;
//    }

    //拷贝到输出
    memcpy(data,m_data,sizeof(float)*m_inNum);
}

bool ResponseProcessorf::postProcess()
{
    fftwf_destroy_plan(m_r2cPlan);
    fftwf_destroy_plan(m_c2rPlan);

    delete[] m_data;
    fftwf_free(m_dataComplex);

    return true;
}

