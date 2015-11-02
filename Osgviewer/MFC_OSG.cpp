// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"
#include "winsock2.h"
#include "MainFrm.h"
#include "OutputWnd.h"
#include "resource.h"
#include "ConnectDlg.h"



using namespace std;

//extern vector<vector<float>> framedata;
//extern CCriticalSection g_cs;
//extern COutputWnd* gOutputWnd;
vector<Skeleton*> cOSG::skeletonArray;
vector<osg::Node*> cOSG::skeletonGeoArray;

cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd) 
{
	socketArray = NULL;

	cam_x = 0.0f, cam_y = 1.0f, cam_z = 3.0f;

	zdelta = 0;
	iFollow = 1;
	prdata = NULL;
	iframes = 0;
	transX = transY = 0;
}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    //Sleep(1000);
    mViewer->stopThreading();
	if(socketArray)
	{
		closesocket( socketArray[0] );
		delete [] socketArray;
	}
    delete mViewer;
}

void cOSG::InitOSG(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void cOSG::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();
	trackball->setAllowThrow(false);

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}


void cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
	shadowedScene = new osgShadow::ShadowedScene;
	shadowedScene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	shadowedScene->setCastsShadowTraversalMask(CastsShadowTraversalMask);
	// 定义一个阴影技法类，比如下面代码所述的ShadowMap类，并指定给阴影节点
	osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
	int mapres = 1024;
	sm->setTextureSize(osg::Vec2s(mapres,mapres));
	//shadowedScene->setShadowTechnique(sm.get());

	osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
	shadowedScene->setShadowTechnique(st.get());
	// 定义一个新的光源，并设置其参数
	osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
	osg::Vec4 lightpos(1.0,1.0,1.0,0.0);
	ls->getLight()->setPosition(lightpos);
	ls->getLight()->setAmbient(osg::Vec4(0.6,0.6,0.6,1.0));
	ls->getLight()->setDiffuse(osg::Vec4(0.8,0.8,0.8,1.0));
	ls->getLight()->setSpecular(osg::Vec4(0.4,0.4,0.4,1.0));

	//mModel->setNodeMask(mModel->getNodeMask() & (~ReceivesShadowTraversalMask));
	mFloor = osgDB::readNodeFile("floor.3ds");	

	mFloor->setNodeMask(mFloor->getNodeMask() & (~CastsShadowTraversalMask));
	shadowedScene->addChild(mFloor.get());
	shadowedScene->addChild(ls.get());

	ResetSkeleton();
}
void cOSG::addSkeleton()
{
	// Load the Model from the model name
	osg::ref_ptr<osg::Node> mModel = osgDB::readNodeFile("biped.3ds"/*m_ModelName*/);
	osg::Group* newSkeleton_geo = new osg::Group;
	Skeleton* newSke = new Skeleton(newSkeleton_geo, mModel);
	newSkeleton_geo->setUserData( newSke );
	newSkeleton_geo->setUpdateCallback(new humanNodeCallback);

	// Optimize the model
	osgUtil::Optimizer optimizer;
	optimizer.optimize(newSkeleton_geo);
	optimizer.reset();

	skeletonArray.push_back(newSke);
	skeletonGeoArray.push_back(newSkeleton_geo);
}
void cOSG::ResetSkeleton()
{
	for (vector<osg::Node*>::iterator i = skeletonGeoArray.begin(); i != skeletonGeoArray.end(); i++)
	{
		if( !(shadowedScene->containsNode(*i)));
			shadowedScene->addChild(*i);
	}
	
}

void cOSG::RemoveSkeleton(osg::Node* node)
{
	if((shadowedScene->containsNode(node)))
		shadowedScene->removeChild(node);
}
void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);
    
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init a new Camera (Master for this View)
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.69f, 0.69f, 0.69f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

	osg::Vec3 eye(0, 1.7, 8), center(0, 1.7, 6), up(0, 1, 0);
	keyswitchManipulator->setHomePosition(eye, center, up);

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

    // Set the Scene Data
    mViewer->setSceneData(shadowedScene.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void cOSG::UpdateFrame()
{
	this->PreFrameUpdate();
	//if(this->iFollow)
	{
		this->CameraFollow();
	}
	this->getViewer()->frame();

	this->PostFrameUpdate();
}
void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

	viewer->run();

	_endthread();

	return;
}

bool cOSG::Follow(int follow)
{
	iFollow = follow;
	if(iFollow)
	{
		getViewer()->setCameraManipulator(NULL);
	}
	else
	{
		CameraFollow();

	}
	return true;
}

void cOSG::UpdateCameraFollowPos(float sx, float sy,float sz)
{
	osg::Vec3 sp(sx, sy, sz), cp(cam_x, cam_y, cam_z);
	osg::Vec3 dir = sp - cp;
	dir.normalize();
	//cp = sp - dir * 5.0f;
	if(iFollow == 1)
		cp = sp + osg::Vec3(0, 0, 4);
	else if(iFollow == 2)
		cp = sp + osg::Vec3(4, 0, 0);
	else if(iFollow == 3)
		cp = sp + osg::Vec3(0, 4, 0);
	cam_x = cp[0], cam_y = cp[1], cam_z = cp[2];
}

void cOSG::CameraFollow()
{
	int d_x, d_y;
	float xx, zz, dx, dz, r;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->curFrameIndex;
	if (pMain->curFrameIndex >= skeletonArray.size())
	{
		sx = 0;
		sy = 0;
		sz = 0;
	}
	else
		skeletonArray[pMain->curFrameIndex]->GetSkePos(sx, sy, sz);

	if(iFollow)
	{
		cx = sx;
		cy = sy;
		cz = sz;
		UpdateCameraFollowPos(sx, sy, sz);
	}


	osg::Matrix mat, trans;
	osg::Vec3 up;
	if(iFollow != 3)
		up = osg::Vec3(0, 1, 0);
	else
		up = osg::Vec3(0, 0, -1);


	if(!iFollow)
	{	
		//keyswitchManipulator->setByInverseMatrix( mat );
		if(iButton == 1)
		{
			d_x = x - x0;
			d_y = y - y0;


			dx = cam_x - cx;
			dz = cam_z - cz;
			r = sqrt(dx*dx+dz*dz);
			xx = dx*cos(d_x/100.0f) - dz*sin(d_x/100.0f);
			zz = dx*sin(d_x/100.0f) + dz*cos(d_x/100.0f);
			cam_x = cx + xx;
			cam_z = cz + zz;
			cam_y += d_y * 0.01; 
			trans.makeTranslate(transX, transY, 0);
			mat.makeLookAt(osg::Vec3(cam_x, cam_y, cam_z), osg::Vec3(sx, sy, sz), up);
			mat = mat * trans;
			mViewer->getCamera()->setViewMatrix(mat);
			x0 = x;
			y0 = y;
		}
		else if(iButton == 2)
		{
			 d_x = x - x0;
			 d_y = y - y0;

			 dx = cx - cam_x;
			 dz = cz - cam_z;
			 r = sqrt(dx*dx+dz*dz);

			 transX += d_x/1000.0f*r;
			 transY -= d_y/1000.0f*r;

			
			 trans.makeTranslate(transX, transY, 0);
			 mat.makeLookAt(osg::Vec3(cam_x, cam_y, cam_z), osg::Vec3(sx, sy, sz), up);
			 mat = mat * trans;
			mViewer->getCamera()->setViewMatrix(mat);
			x0 = x;
			y0 = y;
		}
		else if(zdelta)
		{
			dx = cx - cam_x;
			dz = cz - cam_z;
			if(zdelta > 0)
			{
				cam_x += 0.08*dx;
				cam_z += 0.08*dz;
			}
			else
			{
				cam_x -= 0.08*dx;
				cam_z -= 0.08*dz;
			}

			trans.makeTranslate(transX, transY, 0);
			mat.makeLookAt(osg::Vec3(cam_x, cam_y, cam_z), osg::Vec3(sx, sy, sz), up);
			mat = mat * trans;
			mViewer->getCamera()->setViewMatrix(mat);
			zdelta = 0;
		}
	}
	else
	{
		trans.makeTranslate(transX, transY, 0);
		mat.makeLookAt(osg::Vec3(cam_x, cam_y, cam_z), osg::Vec3(sx, sy, sz), up);
		mat = mat * trans;
		mViewer->getCamera()->setViewMatrix(mat);
	}
	//char buf[32] = {0};
	//sprintf(buf, "%.1f, %.1f", sx, sy);
	//skeleton->SetLable(buf);
}
void cOSG::UpdateScale()
{
	for (int i = 0; i < skeletonArray.size(); i++)
	{
		skeletonArray[i]->UpdateScale();
	}
}
void cOSG::ShowFloor(bool bShow)
{
	if(bShow)
		shadowedScene->addChild(mFloor);
	else
		shadowedScene->removeChild(mFloor);
}
void cOSG::ShowShadow(bool bShow)
{
	if(bShow)
		mFloor->setNodeMask(ReceivesShadowTraversalMask);
	else
		mFloor->setNodeMask(CastsShadowTraversalMask);
}
void cOSG::ShowLable(bool bShow)
{
	for (int i = 0; i < skeletonArray.size(); i++)
	{
		skeletonArray[i]->ShowLable(bShow);
	}
}
void cOSG::SetPosAndRot(float* pr) 
{
	prdata = pr;
}