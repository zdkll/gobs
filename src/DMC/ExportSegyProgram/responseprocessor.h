#ifndef RESPONSEPROCESSOR_H
#define RESPONSEPROCESSOR_H

#include <complex>
#include "fftw3.h"

class ResponseProcessor
{
public:
    ResponseProcessor(){}

    //设置处理参数-预处理-,N:采样点；dt：采样间隔
    bool preProcess(int n,float dt);

    //处理函数:一道/分量-默认为 m_inNum 输入，采样间隔m_dt
    void fftwResponseProcess(double *data);

    //后处理
    void postProcess();

private:
    int     m_inNum,m_outNum;//每次处理点数,输入输出
    float  m_dt;//采样点间隔
    int      m_nf1,m_nf2;//频率范围（平路数据的位置）
    //数据输入输出,提前开好内存
    double             *m_data;//fftw变换实数
    fftw_complex  *m_dataComplex;//fftw变换复数
    std::complex<double>   *m_hs;//Hs 提前算好

    fftw_plan          m_r2cPlan,m_c2rPlan;
};

#endif // RESPONSEPROCESSOR_H
