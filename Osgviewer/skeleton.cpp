#include "stdafx.h"

#include "skeleton.h"


Skeleton::Skeleton(osg::Group* root1, osg::Node* n)
{
	string name[VMSENS_CHANNEL_NUM_ENG] = {"hip","hip1","hip2","hip3","chest","neck","head","",
		"lCollar","lShldr","lForeArm","lHand","",
		"rCollar","rShldr","rForeArm","rHand","",
		"lThigh","lShin","lFoot","lFoot1","",
		"rThigh","rShin","rFoot","rFoot1","",
	};
	for (int i=0; i < VMSENS_CHANNEL_NUM_ENG ;i++)
		segmentName[i] = name[i];

	for (int i = 0; i< VMSENS_CHANNEL_NUM_ENG ;i++)
	{
		NodeBone[i] = NULL;
		if (name[i] != "")
		{
			findNodeVisitor tempfind(segmentName[i]);
			n->accept(tempfind);
			
			NodeBone[i] = new osg::MatrixTransform();
			osg::Node* bone = tempfind.getFirst();
			NodeBone[i]->addChild(bone);
			root1->addChild(NodeBone[i]);
		}
	}
	{
		Text = new osg::MatrixTransform();
		skeLable = new osgText::Text();
		skeLable->setCharacterSize(24);
		skeLable->setFontResolution( 128, 128 );
		skeLable->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
		skeLable->setFont("times.ttf");
		skeLable->setText("Ske #1");
		skeLable->setAxisAlignment(osgText::Text::SCREEN);
		//skeLable->setDrawMode(osgText::Text::TEXT | osgText::Text::ALIGNMENT);

		skeLable->setAlignment(osgText::Text::CENTER_TOP);
		skeLable->setPosition( osg::Vec3(0,0.4,0) );
		skeLable->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );

		osg::Geode* skeLableGeode = new osg::Geode();
		osg::StateSet* stateset = skeLableGeode->getOrCreateStateSet(); 
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		skeLableGeode->addDrawable(skeLable);
		Text->addChild(skeLableGeode);
		Text->setNodeMask(0x1);
		root1->addChild(Text);
	}
	for (int i = 0; i < VMSENS_CHANNEL_NUM_ENG; i++)
	{
		sBuffer1.mChannel[i].mR[0] = 0;
		sBuffer1.mChannel[i].mR[1] = 0;
		sBuffer1.mChannel[i].mR[2] = 0;
	}
	iHierachy[0] = -1; iHierachy[1] = 0; iHierachy[2] = 1; iHierachy[3] = 2; iHierachy[4] = 3;
	iHierachy[5] = 4; iHierachy[6] = 5; iHierachy[7] = 6; 
	iHierachy[8] = 4; iHierachy[9] = 8;	iHierachy[10] = 9; iHierachy[11] = 10; iHierachy[12] = 11; 
	iHierachy[13] = 4; iHierachy[14] = 13;	iHierachy[15] = 14; iHierachy[16] = 15; iHierachy[17] = 16; 
	iHierachy[18] = 0; iHierachy[19] = 18;	iHierachy[20] = 19; iHierachy[21] = 20; iHierachy[22] = 21; 
	iHierachy[23] = 0; iHierachy[24] = 23;	iHierachy[25] = 24; iHierachy[26] = 25; iHierachy[27] = 26; 
	for (int i = 0; i < VMSENS_CHANNEL_NUM_ENG; i++) // 这样做不对，应该以其长度为例
	{
		fScale[i] = 1.0f;
		fLen[i] = 0.0f;
		int father = iHierachy[i];
		if(father == -1)
			continue;
		float dx0 = sBuffer0.mChannel[i].mT[0] - sBuffer0.mChannel[father].mT[0];
		float dy0 = sBuffer0.mChannel[i].mT[1] - sBuffer0.mChannel[father].mT[1];
		float dz0 = sBuffer0.mChannel[i].mT[2] - sBuffer0.mChannel[father].mT[2];
		float len = sqrt(dx0*dx0 + dy0*dy0 + dz0*dz0);
		if(fLen[father] == 0.0f)
			fLen[father] = len;
	} 
}

//接下来应该找一个真实的数据（socket）测试一下
void Skeleton::updateSegments()
{
	for (int i = 0; i < VMSENS_CHANNEL_NUM_ENG; i++)
	{
		if(NodeBone[i])
		{
			float x0, y0, z0, x1, y1, z1, rx, ry, rz;
			x0 = sBuffer0.mChannel[i].mT[0]*0.01;
			y0 = sBuffer0.mChannel[i].mT[1]*0.01;
			z0 = sBuffer0.mChannel[i].mT[2]*0.01;
			x1 = sBuffer1.mChannel[i].mT[0]*0.01;
			y1 = sBuffer1.mChannel[i].mT[1]*0.01;
			z1 = sBuffer1.mChannel[i].mT[2]*0.01;
			rx = sBuffer1.mChannel[i].mR[0]/180.0*3.14159265;
			ry = sBuffer1.mChannel[i].mR[1]/180.0*3.14159265;
			rz = sBuffer1.mChannel[i].mR[2]/180.0*3.14159265;
			osg::Vec3 ax(1, 0, 0), ay(0, 1, 0), az(0, 0, 1);

			osg::Matrix trans0, trans1, rotx, roty, rotz, scale;
			trans0.makeTranslate(x1, y1, z1);
			rotx.makeRotate(rx, ax);
			roty.makeRotate(ry, ay);
			rotz.makeRotate(rz, az);
			scale.makeScale(fScale[i], fScale[i], fScale[i]);
			//rotate.mak
			trans1.makeTranslate(-x0, -y0, -z0);
			osg::Matrix transformation = trans1 * scale * rotx * rotz * roty * trans0;

			NodeBone[i]->setMatrix(transformation);
		}
	}
	osg::Matrix pos;
	float x = sBuffer1.mChannel[2].mT[0]*0.01;
	float y = sBuffer1.mChannel[2].mT[1]*0.01 + 0.3;
	float z = sBuffer1.mChannel[2].mT[2]*0.01;
	pos.makeTranslate(x, y, z);
	Text->setMatrix(pos);
}

void Skeleton::GetSkePos(float &x, float &y, float &z)
{
	x = sBuffer1.mChannel[0].mT[0]*0.01;
	y = sBuffer1.mChannel[0].mT[1]*0.01;
	z = sBuffer1.mChannel[0].mT[2]*0.01;
}

void Skeleton::SetLable(char* str)
{
	skeLable->setText(str);
}
void Skeleton::ShowLable(bool bShow)
{
	if(bShow)
		Text->setNodeMask(0x1);
	else
		Text->setNodeMask(0x0);
}
void Skeleton::UpdateScale()
{
	for (int i = 0; i < VMSENS_CHANNEL_NUM_ENG; i++) // 这样做不对，应该以其长度为例
	{
		fScale[i] = 0.0f;
		int father = iHierachy[i];
		if(father == -1)
			continue;
		if(fScale[father] == 0.0f)
		{
			float dx1 = sBuffer1.mChannel[i].mT[0] - sBuffer1.mChannel[father].mT[0];
			float dy1 = sBuffer1.mChannel[i].mT[1] - sBuffer1.mChannel[father].mT[1];
			float dz1 = sBuffer1.mChannel[i].mT[2] - sBuffer1.mChannel[father].mT[2];
			float len = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
			fScale[father] = len / fLen[father];	
		}
	}
}