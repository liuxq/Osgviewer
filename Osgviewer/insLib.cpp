/*
insLib.cpp
Created on: 2013-01
		by: scape
some INS lib, include eulr,qua,dcm's Trans and Matrix math
*/

#include "stdafx.h"

#include "insLib.h"
int EULR2DCM(double eulr[3],double dcm[3][3])
{
	double phi = eulr[0];
	double theta = eulr[1];
	double psi = eulr[2];

	/*
	double cpsi2 = cos(psi/2.0);
	double spsi2 = sin(psi/2.0);
	double cthe2 = cos(theta/2.0);
	double sthe2 = sin(theta/2.0);
	double cphi2 = cos(phi/2.0);
	double sphi2 = sin(phi/2.0);

	double a = cphi2*cthe2*cpsi2 + sphi2*sthe2*spsi2;
	double b = sphi2*cthe2*cpsi2 - cphi2*sthe2*spsi2;
	double c = cphi2*sthe2*cpsi2 + sphi2*cthe2*spsi2;
	double d = cphi2*cthe2*spsi2 + sphi2*sthe2*cpsi2;
	dcm[0][0] = a*a + b*b - c*c - d*d;
	dcm[0][1] = 2*(b*c - a*d);
	dcm[0][2] = 2*(b*d + a*c);
	dcm[1][0] = 2*(b*c + a*d);
	dcm[1][1] = a*a - b*b + c*c - d*d;
	dcm[1][2] = 2*(c*d - a*b);
	dcm[2][0] = 2*(b*d - a*c);
	dcm[2][1] = 2*(c*d + a*b);
	dcm[2][2] = a*a - b*b - c*c + d*d;
	*/
	double cpsi,cphi,cthe;
	double spsi,sphi,sthe;
	cpsi = cos(psi); spsi = sin(psi);
	cthe = cos(theta); sthe = sin(theta);
	cphi = cos(phi); sphi = sin(phi);

	double C1[3][3] = {{cpsi,spsi,0},{-spsi,cpsi,0},{0,0,1}};
	double C2[3][3] = {{cthe,0,-sthe},{0,1,0},{sthe,0,cthe}};
	double C3[3][3] = {{1,0,0},{0,cphi,sphi},{0,-sphi,cphi}};

	double tmp_mtx[3][3];
	MTXxxMTX(tmp_mtx,C3,C2);
	MTXxxMTX(dcm,tmp_mtx,C1);

	return 0;
}
int DCM2EULR(double dcm[3][3],double eulr[3])
{
	eulr[0] = atan(dcm[2][1]/dcm[2][2]);
	eulr[1] = asin(-dcm[2][0]);
	eulr[2] = atan2(dcm[1][0],dcm[0][0]);
	return 0;
}

int EULR2QUA(double eulr[3],double qua[4])
{
	double phi = eulr[0];
	double theta = eulr[1];
	double psi = eulr[2];

	double cpsi2 = cos(psi/2.0);
	double spsi2 = sin(psi/2.0);
	double cthe2 = cos(theta/2.0);
	double sthe2 = sin(theta/2.0);
	double cphi2 = cos(phi/2.0);
	double sphi2 = sin(phi/2.0);

	qua[0] = cphi2*cthe2*cpsi2 + sphi2*sthe2*spsi2;
	qua[1] = sphi2*cthe2*cpsi2 - cphi2*sthe2*spsi2;
	qua[2] = cphi2*sthe2*cpsi2 + sphi2*cthe2*spsi2;
	qua[3] = cphi2*cthe2*spsi2 + sphi2*sthe2*cpsi2;

	return 0;
}
int QUA3EULR(double qua[4],double eulr[3])
{
	double dcm[3][3];
	QUA2DCM(qua,dcm);
	DCM2EULR(dcm,eulr);

	return 0;
}

int QUA2DCM(double qua[4],double dcm[3][3])
{

	double NormQ=sqrt(qua[0]*qua[0]+qua[1]*qua[1]+qua[2]*qua[2]+qua[3]*qua[3]);
	qua[0]=qua[0]/NormQ;
	qua[1]=qua[1]/NormQ;
	qua[2]=qua[2]/NormQ;
	qua[3]=qua[3]/NormQ;
	
	
	double a=qua[0];
	double b=qua[1];
	double c=qua[2];
	double d=qua[3];
	

	dcm[0][0] = a*a + b*b - c*c - d*d;
	dcm[0][1] = 2*(b*c - a*d);
	dcm[0][2] = 2*(b*d + a*c);
	dcm[1][0] = 2*(b*c + a*d);
	dcm[1][1] = a*a - b*b + c*c - d*d;
	dcm[1][2] = 2*(c*d - a*b);
	dcm[2][0] = 2*(b*d - a*c);
	dcm[2][1] = 2*(c*d + a*b);
	dcm[2][2] = a*a - b*b - c*c + d*d;
	
	return 0;
}
int DCM2QUA(double dcmX[3][3],double qua[4])
{
	/*double tmp;
	qua[0] = 0.5*sqrt(1+dcm[0][0]+dcm[1][1]+dcm[2][2]);
	tmp = 1.0/(4.0*qua[0]);
	qua[1] = tmp*(dcm[2][1]-dcm[1][2]);
	qua[2] = tmp*(dcm[0][2]-dcm[2][0]);
	qua[3] = tmp*(dcm[1][0]-dcm[0][1]);
	return 0;*/
    double dcm[3][3];
	double pi=3.141592653;
	  for(int i=0;i<3;i++)
	  {
		  for(int j=0;j<3;j++)
		      dcm[i][j]=dcmX[j][i];
	  }
      double tmp;
      double tr=dcm[0][0]+dcm[1][1]+dcm[2][2];
      double d[3];
	  double sqdip1;
      if(tr>=0)
      {
        qua[0] = 0.5*sqrt(1+dcm[0][0]+dcm[1][1]+dcm[2][2]);
      	tmp = 1.0/(4.0*qua[0]);
      	qua[1] = tmp*(dcm[1][2]-dcm[2][1]);
      	qua[2] = tmp*(dcm[2][0]-dcm[0][2]);
      	qua[3] = tmp*(dcm[0][1]-dcm[1][0]);  
      }
      else
      {
      	d[0]=dcm[0][0];
            d[1]=dcm[1][1];
            d[2]=dcm[2][2];

            if((d[1]>d[0])&&d[1]>d[2])
            {
                  sqdip1= sqrt(d[1] - d[0] - d[2] + 1.0 );

                  qua[2] = 0.5*sqdip1; 
            
                  if ( sqdip1 != 0 )
                        sqdip1 = 0.5/sqdip1;
            
                  qua[0] = (dcm[2][0] - dcm[0][2])*sqdip1; 
                  qua[1] = (dcm[0][1] + dcm[1][0])*sqdip1; 
                  qua[3] = (dcm[1][2] + dcm[2][1])*sqdip1;           
            }
            else
            {
				sqdip1 = sqrt(d[2] - d[0] - d[1] + 1.0 );
                  if(d[2]>d[0])
                  {
				        qua[3] = 0.5*sqdip1; 
            
						if ( sqdip1 != 0 )
							sqdip1 = 0.5/sqdip1;
						
						
						qua[0] = (dcm[0][1] - dcm[1][0])*sqdip1;
						qua[1] = (dcm[2][0] + dcm[0][2])*sqdip1; 
						qua[2] = (dcm[1][2] + dcm[2][1])*sqdip1; 
                  }
                  else
                  {
						sqdip1 = sqrt(d[0] - d[1] - d[2] + 1.0 );
            
						qua[1] = 0.5*sqdip1; 
						
						if ( sqdip1 != 0 )
							sqdip1 = 0.5/sqdip1;
						
						qua[0] = (dcm[1][2] - dcm[2][1])*sqdip1; 
						qua[2] = (dcm[0][1] + dcm[1][0])*sqdip1; 
						qua[3] = (dcm[2][0] + dcm[0][2])*sqdip1; 
                  }
            }
            
      }
	return 0;
}

int MatrixInv(double mtx[3][3])
{
	int i,j;
	double tmp[3][3];
	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{
			tmp[j][i]=mtx[i][j];
		}
	}
	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{
			mtx[i][j]=tmp[i][j];
		}
	}

	return 0;
}
int MTXxxMTX(double re[3][3],double A[3][3],double B[3][3])
{
	int i,j,k;
	
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			re[i][j]=0;
			for(k=0; k<3; k++)
				re[i][j]+=A[i][k]*B[k][j];
			//Mdcm[i][j]=DCMDCM[N][i][j];
		}
	}

	return 0;
}
int mtxcpy(double re[3][3],double sr[3][3])
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			re[i][j]=sr[i][j];

	return 0;
}

int mtxNorm( double mtx[3][3] )
{
		double a,b,c,d;
		double tmp;
		a = 0.5*sqrt(1+mtx[0][0]+mtx[1][1]+mtx[2][2]);
		tmp = 1/(4*a);
		b = tmp*(mtx[2][1]-mtx[1][2]);
		c = tmp*(mtx[0][2]-mtx[2][0]);
		d = tmp*(mtx[1][0]-mtx[0][1]);

		tmp=sqrt(a*a+b*b+c*c+d*d);
		a/=tmp;
		b/=tmp;
		c/=tmp;
		d/=tmp;


		mtx[0][0] = a*a + b*b - c*c - d*d;
		mtx[0][1] = 2*(b*c - a*d);
		mtx[0][2] = 2*(b*d + a*c);
		mtx[1][0] = 2*(b*c + a*d);
		mtx[1][1] = a*a - b*b + c*c - d*d;
		mtx[1][2] = 2*(c*d - a*b);
		mtx[2][0] = 2*(b*d - a*c);
		mtx[2][1] = 2*(c*d + a*b);
		mtx[2][2] = a*a - b*b - c*c + d*d;

		return 0;
}

int vecNorm( double vec[3] )
{
	double tmp;
	int i;
	tmp=0;
	for(i=0; i<3; i++)
		tmp+=vec[i]*vec[i];
	tmp=sqrt(tmp);

	if(tmp<=1e-9)
		return 1;
	for (i=0; i<3; i++)
		vec[i]/=tmp;

	return 0;
}

void vecCross( double vC[3],double vA[3],double vB[3] )
{

	vC[0]=vA[1]*vB[2]-vA[2]*vB[1];
	vC[1]=vA[2]*vB[0]-vA[0]*vB[2];
	vC[2]=vA[0]*vB[1]-vA[1]*vB[0];
}

void eulr2dcmX(double ang[3], double dcm[3][3])
{
	double phi = ang[0];
	double theta = ang[1];
	double psi = ang[2];


	double cpsi2 = cos(psi/2.0);
	double spsi2 = sin(psi/2.0);
	double cthe2 = cos(theta/2.0);
	double sthe2 = sin(theta/2.0);
	double cphi2 = cos(phi/2.0);
	double sphi2 = sin(phi/2.0);

	double a = cphi2*cthe2*cpsi2 + sphi2*sthe2*spsi2;
	double b = sphi2*cthe2*cpsi2 - cphi2*sthe2*spsi2;
	double c = cphi2*sthe2*cpsi2 + sphi2*cthe2*spsi2;
	double d = cphi2*cthe2*spsi2 + sphi2*sthe2*cpsi2;

	dcm[0][0] = a*a + b*b - c*c - d*d;
	dcm[0][1] = 2*(b*c - a*d);
	dcm[0][2] = 2*(b*d + a*c);
	dcm[1][0] = 2*(b*c + a*d);
	dcm[1][1] = a*a - b*b + c*c - d*d;
	dcm[1][2] = 2*(c*d - a*b);
	dcm[2][0] = 2*(b*d - a*c);
	dcm[2][1] = 2*(c*d + a*b);
	dcm[2][2] = a*a - b*b - c*c + d*d;
}
