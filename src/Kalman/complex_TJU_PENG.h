#ifndef complex_class_TJU_PENG_H
#define complex_class_TJU_PENG_H
/* TYPEDEFS */
typedef struct _complex_f_struct
{ 
    float r;
    float i;
} complex_TJU_PENG;

#ifndef __cplusplus
#define complex  complex_TJU_PENG
#define fcomplex complex_TJU_PENG
#else 
#include<complex>
using std::complex;
typedef complex<float> fcomplex;
const fcomplex unit_imag  = fcomplex(0.0f,1.0f);
const fcomplex unit_real  = fcomplex(0.0f,1.0f);
#endif

/* FUNCTION PROTOTYPES */
#ifndef __cplusplus 
/* complex number manipulation */
fcomplex cadd (fcomplex a, fcomplex b);
fcomplex csub (fcomplex a, fcomplex b);
fcomplex cmul (fcomplex a, fcomplex b);
fcomplex cdiv (fcomplex a, fcomplex b);
float rcabs (fcomplex z);
fcomplex cmplx (float re, float im);
fcomplex conjg (fcomplex z);
fcomplex cneg (fcomplex z);
fcomplex cinv (fcomplex z);
fcomplex crmul (fcomplex a, float x);

#endif








#endif /* complex_class_TJU_PENG.h */
