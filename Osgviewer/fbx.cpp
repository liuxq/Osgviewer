#include "stdafx.h"
#include "commonData.h"
#include "bvh.h"
#include "fbx.h"
#include "insLib.h"
#include "math3d.h"
#include <QMessageBox>
#include "DeviceBuffer.h"
using namespace Math3D;

extern list<vector<sDataBufferEngine>> framedatas;
extern map<QString,int> dataNameMap;
extern CCriticalSection g_cs;


#define FBX_SUPPORT

#include "fbxsdk.h"

FBX::FBX()
{
	curFrameIndex = 0;
}
FBX::~FBX()
{

}


void CreateFBXNode(FbxScene *scene, BVHNode *bvhNode, FbxNode *fbxNode, int curFrameIndex)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;

	FbxNode* fbxSubNode = FbxNode::Create(scene, bvhNode->name().toAscii());

	if(bvhNode->name() == "Site")
		return;

	// 相对于父节点，应该减去父节点位置
	//Position pos = bvhNode->frameData(0).position();
	int nodeIndex = dataNameMap[bvhNode->name()];
	Position pos(framedata[0].mChannel[nodeIndex].mT[0],framedata[0].mChannel[nodeIndex].mT[1],
		framedata[0].mChannel[nodeIndex].mT[2]);
	//Rotation rot = bvhNode->frameData(0).rotation();
	float x = 0, y = 0, z = 0;
	BVHNode* pFather = bvhNode->getFather();
	if(pFather)
	{
		int fa_nodeIndex = dataNameMap[pFather->name()];
		Position posFather(framedata[0].mChannel[fa_nodeIndex].mT[0],framedata[0].mChannel[fa_nodeIndex].mT[1],
			framedata[0].mChannel[fa_nodeIndex].mT[2]);
		x = pos.x - posFather.x;
		y = pos.y - posFather.y;
		z = pos.z - posFather.z;
	}
	FbxSkeleton* fbxSubSkeleton = FbxSkeleton::Create(scene, bvhNode->name().toAscii()+"Skeleton");
	fbxSubSkeleton->SetSkeletonType(FbxSkeleton::eLimbNode);
	fbxSubNode->SetNodeAttribute(fbxSubSkeleton);

	FbxVector4 lT, lR, lS;
	FbxAMatrix lGM;
	//lT.Set(x*100, y*100, z*100); lGM.SetT(lT);
	lT.Set(x*1, y*1, z*1); lGM.SetT(lT);
	lR.Set(0, 0, 0); lGM.SetR(lR);
	fbxSubNode->LclTranslation.Set(lGM.GetT());
	fbxSubNode->LclRotation.Set(lGM.GetR());
	fbxSubNode->LclScaling.Set(lGM.GetS());

	fbxNode->AddChild(fbxSubNode);
		
	for(int i=0;i<bvhNode->numChildren();i++)
	{
		CreateFBXNode(scene, bvhNode->child(i), fbxSubNode, curFrameIndex);
	}
}

//当前节点相对父节点的
void ExportAnimation(int nFrames, float fFrameTime, FbxAnimLayer* animLayer, BVHNode *bvhNode, FbxNode *fbxNode, int curFrameIndex)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	if(!bvhNode || !fbxNode)
		return;

	QString lxq = bvhNode->name();
	if (lxq == "Site")
	{
		return;
	}

	FbxTime lTime;
	FbxAnimCurve* lCurve;
	int lKeyIndex = 0;
	float   F[3][3],FI[3][3],T[3][3],R[3][3];

	//能否利用ExportScene05的SetGlobalDefaultPosition实现相对旋转？
	lCurve = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{
			//Rotation rot=bvhNode->frameData(i).rotation();
			int nodeIndex = dataNameMap[bvhNode->name()];
			Rotation rot(framedata[i].mChannel[nodeIndex].mQ[0],framedata[i].mChannel[nodeIndex].mQ[1],
				framedata[i].mChannel[nodeIndex].mQ[2],framedata[i].mChannel[nodeIndex].mQ[3]);

			double qua[4]={rot.w,rot.x,rot.y,rot.z};
			double dcm[3][3];
			double eulrx,eulry,eulrz;
			if ( bvhNode->getFather() != NULL && bvhNode->getFather()->name() != "position" && bvhNode->name() != "position")
			{		
				
				float   F[3][3],FI[3][3],T[3][3],TP[3][3],TPI[3][3],R[3][3];
				float tmp[3][3];
				//Rotation fa_rot=bvhNode->getFather()->frameData(i).rotation();
				int fa_nodeIndex = dataNameMap[bvhNode->getFather()->name()];
				Rotation fa_rot(framedata[i].mChannel[fa_nodeIndex].mQ[0],framedata[i].mChannel[fa_nodeIndex].mQ[1],
					framedata[i].mChannel[fa_nodeIndex].mQ[2],framedata[i].mChannel[fa_nodeIndex].mQ[3]);

				double fa_qua[4]={fa_rot.w,fa_rot.x,fa_rot.y,fa_rot.z};
				QUA2DCM(fa_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(F, eulrx,eulry,eulrz);

				MatrixInverse(FI, F);
				QUA2DCM(qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(T, eulrx, eulry ,eulrz);
				MatrixxMatrix(R, T, FI);
				float x,y,z;
				toEuler( x , y , z, R);
				rot.x = x;
				rot.y = y;
				rot.z = z;
			}
			else
			{
				double hip_qua[4]={rot.w,rot.x,rot.y,rot.z};
				QUA2DCM(hip_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				rot.x = eulrx;
				rot.y = eulry;
				rot.z = eulrz;
			}
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, rot.x);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}
	lCurve = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{
			int nodeIndex = dataNameMap[bvhNode->name()];
			Rotation rot(framedata[i].mChannel[nodeIndex].mQ[0],framedata[i].mChannel[nodeIndex].mQ[1],
				framedata[i].mChannel[nodeIndex].mQ[2],framedata[i].mChannel[nodeIndex].mQ[3]);
			double qua[4]={rot.w,rot.x,rot.y,rot.z};
			double dcm[3][3];
			double eulrx,eulry,eulrz;
			if ( bvhNode->getFather() != NULL && bvhNode->getFather()->name() != "position" && bvhNode->name() != "position")
			{
				float   F[3][3],FP[3][3],FPI[3][3],FI[3][3],T[3][3],TP[3][3],TPI[3][3],R[3][3];
				float tmp[3][3];
				int fa_nodeIndex = dataNameMap[bvhNode->getFather()->name()];
				Rotation fa_rot(framedata[i].mChannel[fa_nodeIndex].mQ[0],framedata[i].mChannel[fa_nodeIndex].mQ[1],
					framedata[i].mChannel[fa_nodeIndex].mQ[2],framedata[i].mChannel[fa_nodeIndex].mQ[3]);

				double fa_qua[4]={fa_rot.w,fa_rot.x,fa_rot.y,fa_rot.z};
				QUA2DCM(fa_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(F, eulrx,eulry,eulrz);

				MatrixInverse(FI, F);
				QUA2DCM(qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(T, eulrx, eulry ,eulrz);
				MatrixxMatrix(R, T, FI);
				float x,y,z;
				toEuler( x , y , z, R);
				rot.x = x;
				rot.y = y;
				rot.z = z;
			}
			else
			{
				double hip_qua[4]={rot.w,rot.x,rot.y,rot.z};
				QUA2DCM(hip_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				rot.x = eulrx;
				rot.y = eulry;
				rot.z = eulrz;
			}
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, rot.y);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}
	lCurve = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{
			int nodeIndex = dataNameMap[bvhNode->name()];
			Rotation rot(framedata[i].mChannel[nodeIndex].mQ[0],framedata[i].mChannel[nodeIndex].mQ[1],
				framedata[i].mChannel[nodeIndex].mQ[2],framedata[i].mChannel[nodeIndex].mQ[3]);
			double qua[4]={rot.w,rot.x,rot.y,rot.z};
			double dcm[3][3];
			double eulrx,eulry,eulrz;
			if ( bvhNode->getFather() != NULL && bvhNode->getFather()->name() != "position" && bvhNode->name() != "position")
			{		
				
			    float   F[3][3],FP[3][3],FPI[3][3],FI[3][3],T[3][3],TP[3][3],TPI[3][3],R[3][3];
				float tmp[3][3];
				int fa_nodeIndex = dataNameMap[bvhNode->getFather()->name()];
				Rotation fa_rot(framedata[i].mChannel[fa_nodeIndex].mQ[0],framedata[i].mChannel[fa_nodeIndex].mQ[1],
					framedata[i].mChannel[fa_nodeIndex].mQ[2],framedata[i].mChannel[fa_nodeIndex].mQ[3]);

				double fa_qua[4]={fa_rot.w,fa_rot.x,fa_rot.y,fa_rot.z};
				QUA2DCM(fa_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(F, eulrx,eulry,eulrz);

				MatrixInverse(FI, F);
				QUA2DCM(qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				toMatrix(T, eulrx, eulry ,eulrz);
				MatrixxMatrix(R, T, FI);
				float x,y,z;
				toEuler( x , y , z, R);
				rot.x = x;
				rot.y = y;
				rot.z = z;
			}
			else
			{
				double hip_qua[4]={rot.w,rot.x,rot.y,rot.z};
				QUA2DCM(hip_qua,dcm);
				eulry=atan2(-dcm[2][0],dcm[0][0]);
				eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
				eulrx=atan2(-dcm[1][2],dcm[1][1]);
				eulrx=eulrx/PI*180;
				eulry=eulry/PI*180;
				eulrz=eulrz/PI*180;
				rot.x = eulrx;
				rot.y = eulry;
				rot.z = eulrz;
			}
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, rot.z);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}

	for(int i=0;i<bvhNode->numChildren();i++)
	{
		ExportAnimation(nFrames, fFrameTime, animLayer, bvhNode->child(i), fbxNode->GetChild(i), curFrameIndex);
	}
}

int findAsciiFormat (FbxManager *mgr)
{
	// Write in fall back format in less no ASCII format found
	int fileFormat = mgr->GetIOPluginRegistry()->GetNativeWriterFormat();

	//Try to export in ASCII if possible
	int lFormatIndex, lFormatCount = mgr->GetIOPluginRegistry()->GetWriterFormatCount();

	for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
	{
		if (mgr->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
		{
			FbxString lDesc =mgr->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
			char *lASCII = "ascii";
			if (lDesc.Find(lASCII)>=0)
			{
				fileFormat = lFormatIndex;
				break;
			}
		}
	}

	return fileFormat;

}

void FBX::fbxWrite(const QString& file, int startFrame, int endFrame)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	if (startFrame >= endFrame || startFrame < 0 || endFrame >= framedata.size())
	{
		return;
	}
	
	BVH bvh_;
	BVHNode* root = bvh_.readSkeletonFromAvm("TPose.avm");

	//fbx
	FbxManager *fbxSdkManager = FbxManager::Create();
	FbxScene *scene = FbxScene::Create( fbxSdkManager, "VMSENS_Scene" );

	FbxNode* positionNode = FbxNode::Create(scene,"PositionNode");

	//创建根节点
	FbxSkeleton* skeletonPosition = FbxSkeleton::Create(scene,"PositionSkeleton");
	skeletonPosition->SetSkeletonType(FbxSkeleton::eRoot);
	positionNode->SetNodeAttribute(skeletonPosition);

	FbxVector4 lT, lR, lS;
	FbxAMatrix lGM;
	lT.Set(0, 0, 0); lGM.SetT(lT);
	lR.Set(0, 0, 0); lGM.SetR(lR);
	positionNode->LclTranslation.Set(lGM.GetT());
	positionNode->LclRotation.Set(lGM.GetR());
	positionNode->LclScaling.Set(lGM.GetS());
	//创建关节
	CreateFBXNode(scene, root, positionNode, curFrameIndex);

	scene->GetRootNode()->AddChild(positionNode);

	//Animate Skeleton
	FbxAnimStack* animStack = FbxAnimStack::Create(scene, "Cube Animation Stack");
	FbxAnimLayer* animLayer = FbxAnimLayer::Create(scene, "Base Layer");
	animStack->AddMember(animLayer);

	FbxTime lTime;
	FbxAnimCurve* lCurve;
	int lKeyIndex = 0;
	int nFrames = endFrame;
	float fFrameTime = 0.04;
	//输出positionNode的位移动画（采集过程中有行走）
	lCurve = positionNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{			
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, framedata[i].mChannel[0].mT[0]*1);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}
	lCurve = positionNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{			
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, framedata[i].mChannel[0].mT[1]*1);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}
	lCurve = positionNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	if (lCurve)
	{
		lCurve->KeyModifyBegin();
		for(int i = 0; i < nFrames; i++)
		{			
			lTime.SetSecondDouble(i*fFrameTime);
			lKeyIndex = lCurve->KeyAdd(lTime);
			lCurve->KeySetValue(lKeyIndex, framedata[i].mChannel[0].mT[2]*1);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
		}
		lCurve->KeyModifyEnd();
	}
	//输出关节的相对转角
	ExportAnimation(nFrames, fFrameTime, animLayer, root, positionNode->GetChild(0), curFrameIndex);

	//Export scene to fbx
	FbxIOPluginRegistry *pluginRegistry = fbxSdkManager->GetIOPluginRegistry();

	//Create an exporter
	FbxExporter *exporter = FbxExporter::Create( fbxSdkManager, "ADN_Exporter" );

	//Initize settings
	FbxIOSettings * ios = FbxIOSettings::Create(fbxSdkManager, IOSROOT );
	ios->SetBoolProp(EXP_FBX_ANIMATION, true); // Do not need animation in this example
	fbxSdkManager->SetIOSettings(ios);


	int fileFormat = findAsciiFormat(fbxSdkManager);

	// Initialize the exporter by providing a filename.
	if(exporter->Initialize(file.toLocal8Bit(), fileFormat, fbxSdkManager->GetIOSettings()) == false)
	{
		FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
		//FBXSDK_printf("Error returned: %s\n\n", exporter->GetLastErrorString());
		return;
	}
	if (!exporter->Export(scene))
		FBXSDK_printf("Call to FbxExporter::Export() failed.\n");

	fbxSdkManager->Destroy();
}