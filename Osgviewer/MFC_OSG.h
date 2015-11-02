#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>

#include <string>
#include "skeleton.h"

static int ReceivesShadowTraversalMask = 0x1;
static int CastsShadowTraversalMask = 0x2;
class cOSG
{
public:
    cOSG(HWND hWnd);
    ~cOSG();

    void InitOSG(std::string filename);
    void InitManipulators(void);
    void InitSceneGraph(void);
	static void addSkeleton();
	void ResetSkeleton();
	void RemoveSkeleton(osg::Node* node);
    void InitCameraConfig(void);
    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    static void Render(void* ptr);

	void UpdateFrame();

    osgViewer::Viewer* getViewer() { return mViewer; }
	bool Follow(int follow);
	void UpdateCameraFollowPos(float sx, float sy,float sz);
	void CameraFollow();
	void UpdateScale();
	void ShowFloor(bool bShow);
	void ShowShadow(bool bShow);
	void ShowLable(bool bShow);
	void SetPosAndRot(float* pr);
	
	static vector<Skeleton*> skeletonArray;
	static vector<osg::Node*> skeletonGeoArray;
	SOCKET *socketArray;
	float cam_x, cam_y, cam_z;
	int iFollow;
	int iButton;//那車㊣那赤??‘～∩?邦㏒?
	int x, y, x0, y0;//那車㊣那辰??‘?～o車米?℅?㊣那
	int zdelta;
	float sx, sy, sz;
	float cx,cy, cz;
	float transX,transY;
private:
    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
    osgViewer::Viewer* mViewer;
	osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene;	

	osg::ref_ptr<osg::Node> mFloor;
    osg::ref_ptr<osgGA::TrackballManipulator> trackball;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;
	float *prdata;
	int iframes;
};

