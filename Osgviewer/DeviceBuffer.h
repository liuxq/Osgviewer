#ifndef DEVICEBUFFER_H
#define DEVICEBUFFER_H

enum AttributeType
{
	Pos = 0,
	Rot,
	Quar,
	Accel,
	RotateRate
};
#define VMSENS_CHANNEL_NUM_ENG      28

struct sDataBufferEngine
{
	sDataBufferEngine()
	{
		mTime = 0;
		mCounter = 0;                     
		SetupBuffer();
	}

	unsigned __int64 	mTime;				//包时间戳
	int		mCounter;                     //包计数
	struct  
	{
		double      mT[3];
		double      mR[3];  //关节点转动量(角度表示)
		double      mQ[4];  //关节点转动量(四元数表示)
		double	   mAcc[3];
		double	   mGyro[3];
	} mChannel[VMSENS_CHANNEL_NUM_ENG];


	void SetupBuffer()
	{
	
		//0:hip
		mChannel[0].mT[0] = 0.0;
		mChannel[0].mT[1] = 88.60;
		mChannel[0].mT[2] = 0.0;
		mChannel[0].mR[0] = 0.0;
		mChannel[0].mR[1] = 0.0;
		mChannel[0].mR[2] = 0.0;

		//1:hip1
		mChannel[1].mT[0] = 0.0;
		mChannel[1].mT[1] = 100.60;
		mChannel[1].mT[2] = 0.0;
		mChannel[1].mR[0] = 0.0;
		mChannel[1].mR[1] = 0.0;
		mChannel[1].mR[2] = 0.0;

		//2:hip2
		mChannel[2].mT[0] = 0.0;
		mChannel[2].mT[1] = 111.10;
		mChannel[2].mT[2] = 0.0;
		mChannel[2].mR[0] = 0.0;
		mChannel[2].mR[1] = 0.0;
		mChannel[2].mR[2] = 0.0;

		//3:hip3
		mChannel[3].mT[0] = 0.0;
		mChannel[3].mT[1] = 121.10;
		mChannel[3].mT[2] = 0.0;
		mChannel[3].mR[0] = 0.0;
		mChannel[3].mR[1] = 0.0;
		mChannel[3].mR[2] = 0.0;

		//4:chest
		mChannel[4].mT[0] = 0.0;
		mChannel[4].mT[1] = 131.10;
		mChannel[4].mT[2] = 0.0;
		mChannel[4].mR[0] = 0.0;
		mChannel[4].mR[1] = 0.0;
		mChannel[4].mR[2] = 0.0;

		//5:neck
		mChannel[5].mT[0] = 0.0;
		mChannel[5].mT[1] = 141.10;
		mChannel[5].mT[2] = 0.0;
		mChannel[5].mR[0] = 0.0;
		mChannel[5].mR[1] = 0.0;
		mChannel[5].mR[2] = 0.0;

		//6:head
		mChannel[6].mT[0] = 0.0;
		mChannel[6].mT[1] = 153.366162;
		mChannel[6].mT[2] = 2.27;
		mChannel[6].mR[0] = 0.0;
		mChannel[6].mR[1] = 0.0;
		mChannel[6].mR[2] = 0.0;

		//7:End Site
		mChannel[7].mT[0] = 0.0;
		mChannel[7].mT[1] = 167.749272;
		mChannel[7].mT[2] = 2.27;
		mChannel[7].mR[0] = 0.0;
		mChannel[7].mR[1] = 0.0;
		mChannel[7].mR[2] = 0.0;


		//8:lCollar
		mChannel[8].mT[0] = 3.70;
		mChannel[8].mT[1] = 139.56;
		mChannel[8].mT[2] = 0.90;
		mChannel[8].mR[0] = 90.0;
		mChannel[8].mR[1] = 180.0;
		mChannel[8].mR[2] = -30.0;

		//9:lShldr
		mChannel[9].mT[0] = 18.70;
		mChannel[9].mT[1] = 139.56;
		mChannel[9].mT[2] = 0.90;
		mChannel[9].mR[0] = 0.0;
		mChannel[9].mR[1] = 0.0;
		mChannel[9].mR[2] = 0.0;

		//10:lForeArm
		mChannel[10].mT[0] = 49.40;
		mChannel[10].mT[1] = 139.56;
		mChannel[10].mT[2] = 0.90;
		mChannel[10].mR[0] = 0.0;
		mChannel[10].mR[1] = 0.0;
		mChannel[10].mR[2] = 0.0;

		//11:lHand
		mChannel[11].mT[0] = 74.21;
		mChannel[11].mT[1] = 139.56;
		mChannel[11].mT[2] = 0.90;
		mChannel[11].mR[0] = 0.0;
		mChannel[11].mR[1] = 90.0;
		mChannel[11].mR[2] = 0.0;

		//12:End Site lHand 
		mChannel[12].mT[0] = 92.57;
		mChannel[12].mT[1] = 139.56;
		mChannel[12].mT[2] = 0.90;
		mChannel[12].mR[0] = 0.0;
		mChannel[12].mR[1] = 0.0;
		mChannel[12].mR[2] = 0.0;

		//13:rCollar
		mChannel[13].mT[0] = -3.70;
		mChannel[13].mT[1] = 139.56;
		mChannel[13].mT[2] = 0.90;
		mChannel[13].mR[0] = -90.0;
		mChannel[13].mR[1] = 0.0;
		mChannel[13].mR[2] = -30.0;

		//14:rShldr
		mChannel[14].mT[0] = -18.70;
		mChannel[14].mT[1] = 139.56;
		mChannel[14].mT[2] = 0.90;
		mChannel[14].mR[0] = 0.0;
		mChannel[14].mR[1] = 0.0;
		mChannel[14].mR[2] = 0.0;

		//15:rForeArm
		mChannel[15].mT[0] = -49.40;
		mChannel[15].mT[1] = 139.56;
		mChannel[15].mT[2] = 0.90;
		mChannel[15].mR[0] = 0.0;
		mChannel[15].mR[1] = 0.0;
		mChannel[15].mR[2] = 0.0;

		//16:rHand
		mChannel[16].mT[0] = -74.21;
		mChannel[16].mT[1] = 139.56;
		mChannel[16].mT[2] = 0.90;
		mChannel[16].mR[0] = 0.0;
		mChannel[16].mR[1] = 90.0;
		mChannel[16].mR[2] = 0.0;

		//17:End Site rHand
		mChannel[17].mT[0] = -92.57;
		mChannel[17].mT[1] = 139.56;
		mChannel[17].mT[2] = 0.90;
		mChannel[17].mR[0] = 0.0;
		mChannel[17].mR[1] = 0.0;
		mChannel[17].mR[2] = 0.0;

		//18:lThigh
		mChannel[18].mT[0] = 7.00;
		mChannel[18].mT[1] = 88.60;
		mChannel[18].mT[2] = 0.00;
		mChannel[18].mR[0] = 0.0;
		mChannel[18].mR[1] = 0.0;
		mChannel[18].mR[2] = -90.0;

		//19:lShin
		mChannel[19].mT[0] = 8.00;
		mChannel[19].mT[1] = 46.95;
		mChannel[19].mT[2] = -2.65;
		mChannel[19].mR[0] = 180.0;
		mChannel[19].mR[1] = 0.0;
		mChannel[19].mR[2] = -90.0;

		//20:lFoot
		mChannel[20].mT[0] = 7.14;
		mChannel[20].mT[1] = 7.13;
		mChannel[20].mT[2] = -6.27;
		mChannel[20].mR[0] = 0.0;
		mChannel[20].mR[1] = 0.0;
		mChannel[20].mR[2] = 0.0;

		//21:lFoot1
		mChannel[21].mT[0] = 7.14;
		mChannel[21].mT[1] = 1.51;
		mChannel[21].mT[2] = 5.84;
		mChannel[21].mR[0] = 0.0;
		mChannel[21].mR[1] = 0.0;
		mChannel[21].mR[2] = 0.0;

		//22:End Site lFoot
		mChannel[22].mT[0] = 7.14;
		mChannel[22].mT[1] = 1.51;
		mChannel[22].mT[2] = 15.84;
		mChannel[22].mR[0] = 0.0;
		mChannel[22].mR[1] = 0.0;
		mChannel[22].mR[2] = 0.0;

		//23:rThigh
		mChannel[23].mT[0] = -7.00;
		mChannel[23].mT[1] = 88.60;
		mChannel[23].mT[2] = 0.00;
		mChannel[23].mR[0] = 180.0;
		mChannel[23].mR[1] = 0.0;
		mChannel[23].mR[2] = -90.0;

		//24:rShin
		mChannel[24].mT[0] = -6.00;
		mChannel[24].mT[1] = 46.95;
		mChannel[24].mT[2] = -2.65;
		mChannel[24].mR[0] = 0.0;
		mChannel[24].mR[1] = 0.0;
		mChannel[24].mR[2] = -90.0;

		//25:rFoot
		mChannel[25].mT[0] = -6.86;
		mChannel[25].mT[1] = 7.13;
		mChannel[25].mT[2] = -6.27;
		mChannel[25].mR[0] = 0.0;
		mChannel[25].mR[1] = 0.0;
		mChannel[25].mR[2] = 0.0;

		//26:rFoot1
		mChannel[26].mT[0] = -6.86;
		mChannel[26].mT[1] = 1.51;
		mChannel[26].mT[2] = 5.84;
		mChannel[26].mR[0] = 0.0;
		mChannel[26].mR[1] = 0.0;
		mChannel[26].mR[2] = 0.0;

		//27:End Site rFoot
		mChannel[27].mT[0] = -6.86;
		mChannel[27].mT[1] = 1.51;
		mChannel[27].mT[2] = 15.84;
		mChannel[27].mR[0] = 0.0;
		mChannel[27].mR[1] = 0.0;
		mChannel[27].mR[2] = 0.0;
	}
};


#endif