#include "stdafx.h"
#include "commonData.h"
#include "bvh.h"
#include "c3d.h"
#include "insLib.h"
#include "math3d.h"
#include <QMessageBox>
#include "DeviceBuffer.h"
using namespace Math3D;

extern list<vector<sDataBufferEngine>> framedatas;
extern map<QString,int> dataNameMap;
extern CCriticalSection g_cs;

#import "c3dserver.dll"
#include <atlbase.h>


C3D::C3D()
{
	curFrameIndex = 0;
}

C3D::~ C3D()
{
}
void ExportC3DAnimation(int startFrame, int nFrames,  int &index, BVHNode *bvhNode, C3DSERVERLib::IC3DPtr &p, int curFrameIndex)
{
	if(!bvhNode || bvhNode->type == BVH_END)
		return;

	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;

	int i = 0;
	_variant_t data;
	float x, y, z;
	for(int frm = 1; frm <= nFrames; frm++)
	{
		int nodeIndex = dataNameMap[bvhNode->name()];
		Position pos(framedata[frm+startFrame-1].mChannel[nodeIndex].mT[0],framedata[frm+startFrame-1].mChannel[nodeIndex].mT[1],
			framedata[frm+startFrame-1].mChannel[nodeIndex].mT[2]);

		//Position pos=bvhNode->frameData(frm-1).position();
		x = pos.x; 
		data = -x*10.0f;
		p->SetPointData(index, 0, frm, data);
		y = pos.z; 
		data = y*10.0f;
		p->SetPointData(index, 1, frm, data);
		z = pos.y; 
		data = z*10.0f;
		p->SetPointData(index, 2, frm, data);

		float residual = 0.01f;
		data = residual;
		p->SetPointData(index,3,frm,data); // set the residual value
		data = "1111111";
		p->SetPointData(index,4,frm,data); // set the camera mask
	}
	
	int p_index = p->GetParameterIndex("POINT", "LABELS");
	p->SetParameterLock(p_index, 0);
	string name = bvhNode->name().toAscii();
	CComBSTR cname(name.c_str());	
	//int nd = p->GetParameterNumberDim(p_index);
	//int n = p->GetParameterDimension(p_index, index);
	//p->SetPa
	//int nl = p->GetParameterLength(p_index);
	p->SetParameterValue(p_index, index, BSTR(cname));

	index++;
	for(int i=0;i<bvhNode->numChildren();i++)
	{
		ExportC3DAnimation(startFrame, nFrames, index, bvhNode->child(i), p, curFrameIndex);
	}
}

void C3D::saveC3DFile(const char* sBVHFilePath, int startFrame, int endFrame)
{
	try
	{ 
		C3DSERVERLib::IC3DPtr p(_uuidof(C3DSERVERLib::C3D));
		CComBSTR file(sBVHFilePath);
		int nFileType = 1; // the type of file: PC, DEC, SUN
		int nDataType = 1; // type of data storage for analog/video data
		int nAnalogChannels = 0; // number of analog channels
		int nAVRatio = 0; // analof frames per frame of video
		int nNumMarkers = VMSENS_CHANNEL_NUM_ENG/*ChannelCount*/; // number of markers
		//float fRate = 0.04; // sampling rate of video data
		float fRate = 1/0.04; // sampling rate of video data
		float fScale = 1.0f; // POINT:SCALE factor
		int nFrames = endFrame - startFrame + 1; // number of video frames
		if(nFrames < 3)
			nFrames = 3;

		// create a new file based on the above parameters - when created this will still be in memory
		p->NewFile(BSTR(file),nFileType,nDataType,nAnalogChannels,nAVRatio,nNumMarkers,fRate,fScale,nFrames);
		p->SetStrictParameterChecking(0);
		p->CompressParameterBlocks(0);
		/*
		int ndim[2] = {1, 1}; 
		_variant_t d(ndim);
		char* sC[2] = {"1", "2"};
		_variant_t v(sC);
		
		_bstr_t sName("TEST");
		_bstr_t sDesc("My test of parameters");
		_bstr_t sGroup("POINT");
		unsigned char lock = 0;
		int type = -1;
		int dim = 2;
		p->AddParameter(sName, sDesc, sGroup, lock, type, dim, d, v);
		*/

		// example to write some values to an analog channel

		BVH bvh_;
		BVHNode* root = bvh_.readSkeletonFromAvm("TPose.avm");

		int index = 0;
		ExportC3DAnimation(startFrame, nFrames, index, root, p, curFrameIndex);

		p->SaveFile(BSTR(file),nFileType);
	}
	catch(const _com_error& Err)
	{

	}
}