#include "kalman.h"



/*============================================================================*/
/**
 * ------ Calculation Step of Kalman filtering ------
 * Input  :
 *          i       = time index
 *          d       = data of time(i)
 *          acc_in  = input acceleration
 *          kiv     = Kalman_InnerVars
 * Output :
 *          d_esti  = "filtered data" of time(i)
 *
 * Input & Output :
 *          K       = IN  : K(i  ,i-1)
 *                    OUT : K(i+1,i  )
 *          x_esti  = IN  : estimated "state var" of time(i-1),
 *                    OUT : overwriten as estimated "state var" of time(i)
 */
int Kalman_Calc( int i, float acc_in, float d, float & d_esti, fvec & x_esti, \
                 fmat & K , const KIV & kiv)
{
    const fmat  & Q1 = kiv.Q1;
    const float & Q2 = kiv.Q2;
    const fmat  & C  = kiv.C;
    const fmat  & F  = kiv.F;

    /* Calc G       ----------*/

    float R_alpha =  as_scalar( C*K*C.t()+Q2 );
    fmat  Gc(3,1);
    Gc = K*C.t() / (R_alpha) ;

    /* 1-step prediction of x_esti */
    x_esti(2) += acc_in;
    fvec x_esti_1p  = F * x_esti;

    /* Calc alpha   ----------*/
    float d_esti_1p = as_scalar (C * x_esti_1p);
    float alpha  = d - d_esti_1p;

    /* Calc x_esti  ----------*/
    x_esti = x_esti_1p + Gc*alpha;    // update!
    d_esti = as_scalar (C * x_esti);  // update!

    /* UPDATE K(i+1,i)----------*/
    fmat P     = K - Gc*C*K;
    K = F*P*F.t() + Q1;

    return 0;
}

/*============================================================================*/
/* Read data (gps-samples)
 */
int Get_data(int nt, fvec & gps_xn,fvec & gps_yn)
{
    string dir = "/home/peng/Ex_WSL/kalman/";
    string fn  = dir + "noised_gpsxy_nt1000_dt1_sgm80.dat";

    FILE * fp  =  fopen_xp(fn,"r");
    fread( &gps_xn(0), sizeof(float), nt, fp );
    fread( &gps_yn(0), sizeof(float), nt, fp );
    fclose(fp);
    return 0;

}
