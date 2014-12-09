#include <cmath>
#define PIx 3.141592653
int EULR2DCM(double eulr[3],double dcm[3][3]);
int DCM2EULR(double dcm[3][3],double eulr[3]);

int EULR2QUA(double eulr[3],double qua[4]);
int QUA3EULR(double qua[4],double eulr[3]);

int QUA2DCM(double qua[4],double dcm[3][3]);
int DCM2QUA(double dcm[3][3],double qua[4]);

int MatrixInv(double mtx[3][3]);
int MTXxxMTX(double re[3][3],double A[3][3],double B[3][3]);
int mtxcpy(double re[3][3],double sr[3][3]);

int mtxNorm(double mtx[3][3]);
int vecNorm(double vec[3]);

void vecCross(double vC[3],double vA[3],double vB[3]);
void eulr2dcmX(double ang[3], double dcm[3][3]);