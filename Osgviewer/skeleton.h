#ifndef SKELETON_H
#define SKELETON_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
using namespace std;
#include <vector>

#include "findNodeVisitor.h"
#include "DeviceBuffer.h"
#include <osg/MatrixTransform>
#include <osgText/Font>
#include <osgText/Text>
//相机跟随、
//文字编号、

//阴影 --st --sm (no full screen)

//多人 、拾取、方框（动态材质）、

class Skeleton:public osg::Referenced
{
public:
	Skeleton(osg::Group* root1, osg::Node* n);
	void updateSegments();
	sDataBufferEngine sBuffer0, sBuffer1;
	void GetSkePos(float &x, float &y, float &z);
	void SetLable(char* str);
	void ShowLable(bool bShow);
	void UpdateScale();
protected:
	string segmentName[VMSENS_CHANNEL_NUM_ENG] ;
	osg::MatrixTransform* NodeBone[VMSENS_CHANNEL_NUM_ENG];	
	float fScale[VMSENS_CHANNEL_NUM_ENG], fLen[VMSENS_CHANNEL_NUM_ENG];
	int iHierachy[VMSENS_CHANNEL_NUM_ENG];
	osg::MatrixTransform* Text;
	osgText::Text* skeLable;
};


//update callbacks
class humanNodeCallback : public osg::NodeCallback 
{
public:
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<Skeleton> skeleton = 
			dynamic_cast<Skeleton*> (node->getUserData() );
		if(skeleton != NULL)
		{
			skeleton->updateSegments();
		}
		traverse(node, nv); 
	}
};

#endif