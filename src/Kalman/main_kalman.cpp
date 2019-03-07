/***************************************
*     Author:Xu Peng
*     Date:2018-12-25 21:15
*     Filename:main_kalman.cpp
*     Description:
*
*     Last modified:2018-12-25 21:15
****************************************/
#include "kalman_2019.h"

/*============================================================================*/
int main( int argc,char **argv )
{
    const int   nt      = 1000;         // num of time step
    const float dt      = 1;            // time interval (second)
    const float sgm_acc = 1e-4;         // Standard deviation of acceleration
    const float sgm_obs = 10;           // Standard deviation of displacement(gps-reading)
    const KIV kiv_x(dt,sgm_acc,sgm_obs);
    const KIV kiv_y(dt,sgm_acc,sgm_obs);

    /* state x y */
    fvec x_esti(3,1); 
    fvec y_esti(3,1); 
    fvec gps_xn(nt); 
    fvec gps_yn(nt); 
    fvec gps_x_esti(nt); 
    fvec gps_y_esti(nt); 
    fmat Kx(3,3);
    fmat Ky(3,3);

    /* read data ( all time  steps ) */
    Get_data(nt, gps_xn, gps_yn);

    /* init  */
    x_esti.zeros();
    x_esti(0) = gps_xn(0);
    x_esti(1) = (gps_xn(20) - gps_xn(0))/(dt*20);
    y_esti.zeros();
    y_esti(0) = gps_yn(0);
    y_esti(1) = (gps_yn(20) - gps_yn(0))/(dt*20);
    Kx.eye();
    Ky.eye();
    /* init done */

    /* TIME LOOP CALCULATION */
    for (int i=0;i<nt;i++)
    {
        float Accx_in = 0;  // input acceleration
        float Accy_in = 0;  // input acceleration
        Kalman_Calc( i, Accx_in, gps_xn(i), gps_x_esti(i), x_esti, Kx, kiv_x);
        Kalman_Calc( i, Accy_in, gps_yn(i), gps_y_esti(i), y_esti, Ky, kiv_y);
    }

    /* out put !*/
    string dir =    "/home/peng/Ex_WSL/kalman/";
    string fn  =    dir +  "noised_gpsxy_nt1000_dt1_sgm80_out.dat";

    FILE * fp  = fopen_xp(fn,"w");
    fwrite( &gps_x_esti(0), sizeof(float), nt, fp );
    fwrite( &gps_y_esti(0), sizeof(float), nt, fp );

    cout << " out-put data = " << fn << endl; 
    return 0;
}

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


