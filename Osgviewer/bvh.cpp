/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *ILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Genera
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABl Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */
#include "stdafx.h"
#include "commonData.h"

#include <QMessageBox>
#include "math3d.h"
using namespace Math3D;
#include "bvh.h"
#include "insLib.h"
#include "DeviceBuffer.h"



extern list<vector<sDataBufferEngine>> framedatas;
extern map<QString,int> dataNameMap;
extern CCriticalSection g_cs;




BVH::BVH()
{
  bvhTypeName.append("POS");
  bvhTypeName.append("ROOT");
  bvhTypeName.append("JOINT");
  bvhTypeName.append("End");
  bvhTypeName.append("NoSL");
  bvhChannelName.append("Xposition");
  bvhChannelName.append("Yposition");
  bvhChannelName.append("Zposition");
  bvhChannelName.append("Xrotation");
  bvhChannelName.append("Yrotation");
  bvhChannelName.append("Zrotation");
  
  validNodes << "hip" << "hip1" << "hip2"<< "hip3"<<"chest"<< "neck"<< "head"
             << "lCollar" << "lShldr" << "lForeArm" << "lHand"
             << "rCollar" << "rShldr" << "rForeArm" << "rHand"
             << "lThigh" << "lShin" << "lFoot"<< "lFoot1"
             << "rThigh" << "rShin" << "rFoot"<< "rFoot1"
             << "Site";
  curFrameIndex = 0;
}

BVH::~ BVH()
{
}

QString BVH::token()
{
  if(tokenPos>=tokens.size())
  {
    qDebug("BVH::token(): no more tokens at index %d",tokenPos);
    return QString();
  }
  return tokens[tokenPos++];
}

bool BVH::expect_token(const QString& name)
{
//  qDebug("BVH::expect_token('%s')",name.toLatin1().constData());

  if(name!=token())
  {
    qDebug("BVH::expect_token(): Bad or outdated animation file: %s missing\n", name.toLatin1().constData());
    return false;
  }
  return true;
}

BVHNode* BVH::bvhReadNode()
{
  qDebug("BVH::bvhReadNode()");

  QString type=token();
  if(type=="}") return NULL;

  // check for node type first
  BVHNodeType nodeType;
  if      (type=="ROOT")  nodeType=BVH_ROOT;
  else if (type=="JOINT") nodeType=BVH_JOINT;
  else if (type=="End")   nodeType=BVH_END;
  else
  {
    qDebug("BVH::bvhReadNode(): Bad animation file: unknown node type: '%s'\n",type.toLatin1().constData());
    return NULL;
  }

  // add node with name
  BVHNode* node=new BVHNode(token());
  if(!validNodes.contains(node->name()))
    node->type=BVH_NO_SL;
  else
    // set node type
    node->type=nodeType;

  expect_token("{");
  expect_token("OFFSET");
  node->offset[0]=token().toFloat()/100.0f;
  node->offset[1]=token().toFloat()/100.0f;
  node->offset[2]=token().toFloat()/100.0f;
//  node->ikOn=false;
//  node->ikWeight=0.0;
  if(node->type!=BVH_END)
  {
    expect_token("CHANNELS");
    node->numChannels=token().toFloat();

    // rotation order for this node
    QString order(QString::null);
    for(int i=0;i<node->numChannels;i++)
    {
      node->channelMin[i] = -10000;
      node->channelMax[i] = 10000;
      type=token();
      if     (type=="Xposition") node->channelType[i]=BVH_XPOS;
      else if(type=="Yposition") node->channelType[i]=BVH_YPOS;
      else if(type=="Zposition") node->channelType[i]=BVH_ZPOS;
      else if(type=="Xrotation")
      {
        node->channelType[i]=BVH_XROT;
        order+='X';
      }
      else if(type=="Yrotation")
      {
        node->channelType[i]=BVH_YROT;
        order+='Y';
      }
      else if(type=="Zrotation")
      {
        node->channelType[i]=BVH_ZROT;
        order+='Z';
      }
    }

    if     (order=="XYZ") node->channelOrder=BVH_XYZ;
    else if(order=="ZYX") node->channelOrder=BVH_ZYX;
    else if(order=="YZX") node->channelOrder=BVH_YZX;
    else if(order=="XZY") node->channelOrder=BVH_XZY;
    else if(order=="YXZ") node->channelOrder=BVH_YXZ;
    else if(order=="ZXY") node->channelOrder=BVH_ZXY;
  }

  BVHNode* child=NULL;
  do
  {
    if((child=bvhReadNode()))
    {
      node->addChild(child);
      child->addFather(node);
    }
  } while (child!=NULL);

  return node;
}

void BVH::removeNoSLNodes(BVHNode* root)
{
  qDebug("BVH::removeNoSLNodes()");
  // walk through list of all child nodes
  for(int i=0;i<root->numChildren();i++)
  {
    BVHNode* child=root->child(i);
    // if this is an unsupported node, remove it
    if(child->type==BVH_NO_SL)
    {
      qDebug("BVH::removeNoSLNodes(): removing node '%s'",child->name().toLatin1().constData());
      // find all child joints of the unsupported child
      for(int j=0;j<child->numChildren();j++)
      {
        // move child nodes to the current parent joint
        root->insertChild(child->child(j),i);
      }
      // remove unsupported child node
      root->removeChild(child);
      // start checking for nodes at this point over again
      removeNoSLNodes(root);
      return;
    }
    // check next parent node
    removeNoSLNodes(root->child(i));
  }
}

void BVH::assignChannels(BVHNode* node,int frame)
{
//  qDebug("BVH::assignChannels()");

  // create new rotation and position objects
  Rotation* rot=new Rotation();
  Position* pos=new Position();

  for(int i=0;i<node->numChannels;i++)
  {
    double value=token().toFloat();
    BVHChannelType type=node->channelType[i];
    if     (type==BVH_XPOS) pos->x=value/100.0f;
    else if(type==BVH_YPOS) pos->y=value/100.0f;
    else if(type==BVH_ZPOS) pos->z=value/100.0f;
    else if(type==BVH_XROT) rot->x=value;
    else if(type==BVH_YROT) rot->y=value;
    else if(type==BVH_ZROT) rot->z=value;
    else qDebug("BVH::assignChannels(): unknown channel type %d",(int) type);
  }

  // put rotation and position into the node's cache for later keyframe referencing
  node->cacheRotation(rot);
  node->cachePosition(pos);

  for(int i=0;i<node->numChildren();i++)
    assignChannels(node->child(i),frame);
}

void BVH::setChannelLimits(BVHNode *node,BVHChannelType type,double min,double max) const
{
  qDebug("BVH::setChannelLimits()");

  int i;
  if(!node) return;
  for(i=0;i<node->numChannels;i++)
  {
    if(node->channelType[i]==type)
    {
      node->channelMin[i]=min;
      node->channelMax[i]=max;
      return;
    }
  }
}


// read joint limits file
void BVH::parseLimFile(BVHNode* root,const QString& limFile) const
{
  qDebug("BVH::parseLimFile('%s')",limFile.toLatin1().constData());

  QFile limit(limFile);

  if(!limit.open(QIODevice::ReadOnly))
  {
    QMessageBox::critical(0,QObject::tr("Missing Limits File"),QObject::tr("<qt>Limits file not found at:<br>%1</qt>").arg(limFile));
    return;
  }

  while(!limit.atEnd())
  {
    QString line=limit.readLine(4096).trimmed();
    if(line.isEmpty())
    {
      QMessageBox::critical(0,QObject::tr("Error reading limits file"),QObject::tr("Error reading limits file."));
      return;
    }

    QStringList parameters=line.split(' ');
    QString name=parameters[0];
//    double weight=parameters[1].toDouble();

    BVHNode* node=bvhFindNode(root,name);
    if(node)
    {
 //     node->ikWeight=weight;

      for(int i=0;i<3;i++)
      {
        QString channel=parameters[i*3+1];
        double min=parameters[i*3+2].toDouble();
        double max=parameters[i*3+3].toDouble();

        if     (channel.startsWith("X")) setChannelLimits(node,BVH_XROT,min,max);
        else if(channel.startsWith("Y")) setChannelLimits(node,BVH_YROT,min,max);
        else if(channel.startsWith("Z")) setChannelLimits(node,BVH_ZROT,min,max);
      } // for
    }
    else
      qDebug("BVH::parseLimFile(): Node '%s' not in animation. This will lead to problems!",name.toLatin1().constData());
  }
  limit.close();
}

// in BVH files, this is necessary so that
// all frames but the start and last aren't
// blown away by interpolation
void BVH::setAllKeyFramesHelper(BVHNode* node,int numberOfFrames) const
{
//  qDebug("BVH::setAllKeyFramesHelper()");

  for(int i=0;i<numberOfFrames;i++)
  {
    const Rotation* rot=node->getCachedRotation(i);
    const Position* pos=node->getCachedPosition(i);

    if(node->type!=BVH_END)
		node->addKeyframe(i,Position(pos->x/100.0f,pos->y/100.0f,pos->z/100.0f),Rotation(rot->w,rot->x,rot->y,rot->z));
  }

  for(int i=0;i<node->numChildren();i++)
    setAllKeyFramesHelper(node->child(i),numberOfFrames);
}

void BVH::setAllKeyFrames(Animation* anim) const
{
  qDebug("BVH::setAllKeyFrames()");

  setAllKeyFramesHelper(anim->getMotion(),anim->getNumberOfFrames());
}

void BVH::bvhWriteFrameNew(BVHNode* node,QTextStream& out,int frame)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	int nodeIndex = dataNameMap[node->name()];
	Rotation rot(framedata[frame].mChannel[nodeIndex].mQ[0],framedata[frame].mChannel[nodeIndex].mQ[1],
		framedata[frame].mChannel[nodeIndex].mQ[2], framedata[frame].mChannel[nodeIndex].mQ[3]);
	Position pos(framedata[frame].mChannel[nodeIndex].mT[0],framedata[frame].mChannel[nodeIndex].mT[1],
		framedata[frame].mChannel[nodeIndex].mT[2]);
	double qua[4]={rot.w,rot.x,rot.y,rot.z};
	double dcm[3][3];
	double eulrx,eulry,eulrz;

	if (node->getFather() != NULL && node->getFather()->name() != "position" && node->name() != "position")
	{
		float   F[3][3],FI[3][3],T[3][3],R[3][3];
		//Rotation fa_rot=node->getFather()->frameData(frame).rotation();
		int fa_nodeIndex = dataNameMap[node->getFather()->name()];
		Rotation fa_rot(framedata[frame].mChannel[fa_nodeIndex].mQ[0],framedata[frame].mChannel[fa_nodeIndex].mQ[1],
			framedata[frame].mChannel[fa_nodeIndex].mQ[2], framedata[frame].mChannel[fa_nodeIndex].mQ[3]);
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
	else{
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
	for (int i=0; i<node->numChannels; i++)
	{
		float value=0.0;
		bool iship = false;
		BVHChannelType type=node->channelType[i];

		if     (type==BVH_XPOS) value=pos.x*1;
		else if(type==BVH_YPOS) value=pos.y*1;
		else if(type==BVH_ZPOS) value=pos.z*1;

		else if(type==BVH_XROT) value=rot.x;
		else if(type==BVH_YROT) value=rot.y;
		else if(type==BVH_ZROT) value=rot.z;
		if (value < 0)
			out << value << "\t";
		else
			out <<" "<< value << "\t";
	}

	for(int i=0;i<node->numChildren();i++)
		bvhWriteFrameNew(node->child(i),out,frame);
}
void BVH::bvhWriteFrameNew( BVHNode* node,QTextStream& out,int frame,int writeFlag )
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	QString name=node->name();
	int nodeIndex = dataNameMap[name];
	Rotation rot_origin(framedata[frame].mChannel[nodeIndex].mQ[0],framedata[frame].mChannel[nodeIndex].mQ[1],
		framedata[frame].mChannel[nodeIndex].mQ[2], framedata[frame].mChannel[nodeIndex].mQ[3]);
	Position pos_origin(framedata[frame].mChannel[nodeIndex].mT[0],framedata[frame].mChannel[nodeIndex].mT[1],
		framedata[frame].mChannel[nodeIndex].mT[2]);


	if (writeFlag==1)
	{
		if (name=="rHand"||name=="rForeArm"||name=="rShldr")
		{
			double TA[3][3]={0,1,0,-1,0,0,0,0,1};
			double TAI[3][3]={0,1,0,-1,0,0,0,0,1};
			double qua[4]={rot_origin.w,rot_origin.x,rot_origin.y,rot_origin.z};
			double dcm[3][3];
			QUA2DCM(qua,dcm);
			double dcmtmp[3][3];
			MTXxxMTX(dcmtmp,dcm,TA);
			DCM2QUA(dcmtmp,qua);

			//node->addKeyframe(frame,node->frameData(frame).position(),Rotation(qua[0],qua[1],qua[2],qua[3]));
			for (int i = 0; i < 4; i++)
			{
				framedata[frame].mChannel[nodeIndex].mQ[i] = qua[i];
			}
			
			//node->setKeyframeRotation(frame,Rotation(qua[0],qua[1],qua[2],qua[3]));
		}
		if (name=="lHand"||name=="lForeArm"||name=="lShldr")
		{
			double TA[3][3]={0,1,0,-1,0,0,0,0,1};
			double TAI[3][3]={0,1,0,-1,0,0,0,0,1};
			double qua[4]={rot_origin.w,rot_origin.x,rot_origin.y,rot_origin.z};
			double dcm[3][3];
			QUA2DCM(qua,dcm);
			double dcmtmp[3][3];
			MatrixInv(TA);
			MTXxxMTX(dcmtmp,dcm,TA);
			DCM2QUA(dcmtmp,qua);

			for (int i = 0; i < 4; i++)
			{
				framedata[frame].mChannel[nodeIndex].mQ[i] = qua[i];
			}
			//node->addKeyframe(frame,node->frameData(frame).position(),Rotation(qua[0],qua[1],qua[2],qua[3]));
			//node->setKeyframeRotation(frame,Rotation(qua[0],qua[1],qua[2],qua[3]));
		}
	}

	Rotation rot(framedata[frame].mChannel[nodeIndex].mQ[0],framedata[frame].mChannel[nodeIndex].mQ[1],
		framedata[frame].mChannel[nodeIndex].mQ[2], framedata[frame].mChannel[nodeIndex].mQ[3]);
	Position pos(framedata[frame].mChannel[nodeIndex].mT[0],framedata[frame].mChannel[nodeIndex].mT[1],
		framedata[frame].mChannel[nodeIndex].mT[2]);

	double qua[4]={rot.w,rot.x,rot.y,rot.z};
	double dcm[3][3];
	double eulrx,eulry,eulrz;
	//QUA2DCM(qua,dcm);

	//Position pos=positionNode->frameData(frame).position();

	// preserve channel order while writing
	if (node->getFather() != NULL && node->getFather()->name() != "position" && node->name() != "position")
	{
		float   F[3][3],FI[3][3],T[3][3],R[3][3];
		//Rotation fa_rot=node->getFather()->frameData(frame).rotation();
		int fa_nodeIndex = dataNameMap[node->getFather()->name()];
		Rotation fa_rot(framedata[frame].mChannel[fa_nodeIndex].mQ[0],framedata[frame].mChannel[fa_nodeIndex].mQ[1],
			framedata[frame].mChannel[fa_nodeIndex].mQ[2], framedata[frame].mChannel[fa_nodeIndex].mQ[3]);
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
	else{
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
	for (int i=0; i<node->numChannels; i++)
	{
		float value=0.0;
		bool iship = false;
		BVHChannelType type=node->channelType[i];

		if     (type==BVH_XPOS) value=pos.x*1;
		else if(type==BVH_YPOS) value=pos.y*1;
		else if(type==BVH_ZPOS) value=pos.z*1;

		else if(type==BVH_XROT) value=rot.x;
		else if(type==BVH_YROT) value=rot.y;
		else if(type==BVH_ZROT) value=rot.z;
		if (value < 0)
			out << value << "\t";
		else
			out <<" "<< value << "\t";
	}

	for(int i=0;i<node->numChildren();i++)
		bvhWriteFrameNew(node->child(i),out,frame,1);
}

BVHNode* BVH::readSkeletonFromAvm(const QString& file)
{
	QFile animationFile(file);
	if(!animationFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(0,QObject::tr("File not found"),QObject::tr("BVH File not found: %1").arg(file.toLatin1().constData()));
		return NULL;
	}
	tokenPos=0;
	inputFile=animationFile.readAll();
	animationFile.close();

	inputFile=inputFile.simplified();
	tokens=inputFile.split(' ');

	expect_token("HIERARCHY");

	return bvhReadNode();
}
void BVH::bvhWrite(const QString& file, int startFrame, int endFrame, bool flag3dMaxBVH)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	if (startFrame >= endFrame || startFrame < 0 || endFrame >= framedata.size())
	{
		return;
	}
	
	BVHNode* root = readSkeletonFromAvm("TPose.avm");

	int i;
	QFile f(file);
	f.open(QFile::WriteOnly);
	QTextStream out(&f);
	out.setNumberFlags(QTextStream::ForcePoint);
	out.setRealNumberPrecision(5);

	/*BVHNode* root=anim->getMotion();
	positionNode=anim->getNode(0);*/

	out << "HIERARCHY" << endl;

	if( flag3dMaxBVH == 0 )
		bvhWriteNode(root,out,0);
	else
		bvhWriteNode(root,out,0,1,0);

	out << "MOTION" << endl;

	out << "Frames:\t" << endFrame - startFrame << endl;
	out << "Frame Time:\t" << 0.04  << endl;

	for(i = startFrame; i < endFrame;i++)
	{
		if( flag3dMaxBVH == 0 )
			bvhWriteFrameNew(root,out,i);
		else
			bvhWriteFrameNew(root,out,i,1);
		out << endl;
	}
	f.close();
}
BVHNode* BVH::bvhRead(const QString& file)
{
  qDebug("BVH::bvhRead('%s')",file.toLatin1().constData());

  QFile animationFile(file);
  if(!animationFile.open(QIODevice::ReadOnly))
  {
    QMessageBox::critical(0,QObject::tr("File not found"),QObject::tr("BVH File not found: %1").arg(file.toLatin1().constData()));
    return NULL;
  }

  inputFile=animationFile.readAll();
  animationFile.close();

  inputFile=inputFile.simplified();
  tokens=inputFile.split(' ');

  expect_token("HIERARCHY");

  BVHNode* root=bvhReadNode();

  expect_token("MOTION");
  expect_token("Frames:");
  int totalFrames=token().toInt();
  lastLoadedNumberOfFrames=totalFrames;

  expect_token("Frame");
  expect_token("Time:");

  // store FPS
  lastLoadedFrameTime=token().toFloat();

  for(int i=0;i<totalFrames;i++)
    assignChannels(root,i);

  setAllKeyFramesHelper(root,totalFrames);

  return(root);
}

void BVH::avmReadKeyFrame(BVHNode* root)
{
  // NOTE: new system needs frame 0 as key frame
  // FIXME: find a better way without code duplication
  const Rotation* rot=root->getCachedRotation(0);
  const Position* pos=root->getCachedPosition(0);
  root->addKeyframe(0,Position(pos->x,pos->y,pos->z),Rotation(rot->w,rot->x,rot->y,rot->z));
  if(root->type==BVH_ROOT)
	  lastLoadedPositionNode->addKeyframe(0,Position(pos->x,pos->y,pos->z),Rotation(rot->w,rot->x,rot->y,rot->z));

  int numKeyFrames=token().toInt();
  for(int i=0;i<numKeyFrames;i++)
  {
    int key=token().toInt();

    if(key<lastLoadedNumberOfFrames)
    {
      const Rotation* rot=root->getCachedRotation(key);
      const Position* pos=root->getCachedPosition(key);
      root->addKeyframe(key,Position(pos->x,pos->y,pos->z),Rotation(rot->w,rot->x,rot->y,rot->z));
    }
  }

  // all keyframes are found, flush the node's cache to free up memory
  root->flushFrameCache();
//  root->dumpKeyframes();

  for(int i=0;i<root->numChildren();i++)
    avmReadKeyFrame(root->child(i));
}

// reads ease in / out data
void BVH::avmReadKeyFrameProperties(BVHNode* root)
{
  // NOTE: key frame properties save key 0, too, so numKeyFrames here will be one higher than before

  qDebug("BVH::avmReadKeyFrameProperties()");

  QString numKeys=token();
  if(numKeys.isEmpty()) return;

  int numKeyFrames=numKeys.toInt();
  qDebug("BVH::avmReadKeyFrameProperties(): reading properties for %d key frames",numKeyFrames);

  for(int i=0;i<numKeyFrames;i++)
  {
    int key=token().toInt();

    if(key & 1) root->setEaseIn(root->keyframeDataByIndex(i).frameNumber(),true);
    if(key & 2) root->setEaseOut(root->keyframeDataByIndex(i).frameNumber(),true);
  }

  for(int i=0;i<root->numChildren();i++)
    avmReadKeyFrameProperties(root->child(i));
}

// for debugging only
void BVH::dumpNodes(BVHNode* node,QString indent)
{
  qDebug("%s %s (%d)",indent.toLatin1().constData(),node->name().toLatin1().constData(),node->numChildren());
  indent+="+--";
  for(int i=0;i<node->numChildren();i++)
  {
    dumpNodes(node->child(i),indent);
  }
}

/* .avm files look suspiciously like .bvh files, except
   with keyframe data tacked at the end -- Lex Neva */
BVHNode* BVH::avmRead(const QString& file)
{
  qDebug("BVH::avmRead(%s)",file.toLatin1().constData());

  QFile animationFile(file);
  animationFile.open(QIODevice::ReadOnly);
  inputFile=animationFile.readAll();
  animationFile.close();

  inputFile=inputFile.simplified();
  tokens=inputFile.split(' ');

  expect_token("HIERARCHY");

  BVHNode* root=bvhReadNode();

  expect_token("MOTION");
  expect_token("Frames:");
  int totalFrames=token().toInt();
  lastLoadedNumberOfFrames=totalFrames;
  lastLoadedLoopOut=totalFrames;
//  qDebug("BVH::avmRead(): set loop out to totalFrames");

  expect_token("Frame");
  expect_token("Time:");

  // set FPS
  lastLoadedFrameTime=token().toFloat();

  for(int i=0;i<totalFrames;i++)
  {
    assignChannels(root,i);
  }

  avmReadKeyFrame(root);

  if(expect_token("Properties"))
    avmReadKeyFrameProperties(root);

  // read remaining properties
  QString propertyName;
  while(!(propertyName=token()).isEmpty())
  {
      QString propertyValue=token();

      if(!propertyValue.isEmpty())
      {
        qDebug("BVH::avmRead(): Found extended property: '%s=%s'",propertyName.toLatin1().constData(),
                                                                  propertyValue.toLatin1().constData());
        if(propertyName=="Scale:")
        {
          lastLoadedAvatarScale=propertyValue.toFloat();
        }
        else if(propertyName=="Figure:")
        {
          lastLoadedFigureType=static_cast<Animation::FigureType>(propertyValue.toInt());
        }
        else if(propertyName=="LoopIn:")
        {
//          qDebug("BVH::avmRead(): set loop in to "+propertyValue);
          lastLoadedLoopIn=propertyValue.toInt();
        }
        else if(propertyName=="LoopOut:")
        {
//          qDebug("BVH::avmRead(): set loop out to "+propertyValue);
          lastLoadedLoopOut=propertyValue.toInt();
        }
        else if(propertyName=="Positions:")
        {
          // remember that this is a new animation that has seperate position keyframes
          havePositionKeys=true;

          int num=propertyValue.toInt();
          qDebug("Reading %d Positions:",num);
          for(int index=0;index<num;index++)
          {
            int key=token().toInt();
            qDebug("Reading position frame %d",key);
            const FrameData& frameData=root->frameData(key);
            lastLoadedPositionNode->addKeyframe(key,frameData.position(),Rotation());
          } // for
        }
        else if(propertyName=="PositionsEase:")
        {
          int num=propertyValue.toInt();
          qDebug("Reading %d PositionsEases:",num);
          for(int index=0;index<num;index++)
          {
            int key=token().toInt();
            qDebug("Reading position ease for key index %d: %d",index,key);

            if(key & 1) lastLoadedPositionNode->setEaseIn(lastLoadedPositionNode->keyframeDataByIndex(index).frameNumber(),true);
            if(key & 2) lastLoadedPositionNode->setEaseOut(lastLoadedPositionNode->keyframeDataByIndex(index).frameNumber(),true);

          } // for
        }
        else
          qDebug("BVH::avmRead(): Unknown extended property '%s' (%s), ignoring.",propertyName.toLatin1().constData(),
                                                                                  propertyValue.toLatin1().constData());
    }
  } // while

  return(root);
}

BVHNode* BVH::animRead(const QString& file  ,const QString& limFile)
{
  BVHNode* root;

  // positions pseudonode
  lastLoadedPositionNode=new BVHNode("position");
  lastLoadedPositionNode->type=BVH_POS;
  // default avatar scale for BVH and AVM files
  lastLoadedAvatarScale=1.0;
  // default figure type
  lastLoadedFigureType=Animation::FIGURE_FEMALE;
  // indicates "no loop points set"
  lastLoadedLoopIn=-1;
//  qDebug("BVH::animRead(): set loop in to -1 to indicate missing loop points");
  // reset token position
  tokenPos=0;

  // assume old style animation format for compatibility
  havePositionKeys=false;
  // rudimentary file type identification from filename
  if(file.endsWith(".bvh",Qt::CaseInsensitive))
    root = bvhRead(file);
  else if(file.endsWith(".avm",Qt::CaseInsensitive))
    root = avmRead(file);
  else
    return NULL;

  if(!limFile.isEmpty())
     parseLimFile(root,limFile);

   removeNoSLNodes(root);
//  dumpNodes(root,QString::null);

  // old style animation or BVH format means we need to add position keyframes ourselves
  if(!havePositionKeys)
  {
    for(int index=0;index<root->numKeyframes();index++)
    {
      const FrameData& frameData=root->keyframeDataByIndex(index);
      int frameNum=frameData.frameNumber();
      lastLoadedPositionNode->addKeyframe(frameNum,frameData.position(),Rotation());
      lastLoadedPositionNode->setEaseIn(frameNum,frameData.easeIn());
      lastLoadedPositionNode->setEaseOut(frameNum,frameData.easeOut());
    }
  }

  return root;
}

void BVH::bvhIndent(QTextStream& out,int depth)
{
  for(int i=0;i<depth;i++)
    out << "\t";
}

void BVH::bvhWriteNode(BVHNode* node,QTextStream& out,int depth)
{
  bvhIndent(out,depth);

  QString NameForWrite;
  if(node->name() == "hip")
	  NameForWrite  = "Hips";
  else if(node->name() == "hip1")
	  NameForWrite  = "Chest";
  else if(node->name() == "hip2")
	  NameForWrite  = "Chest2";
  else if(node->name() == "hip3")
	  NameForWrite  = "Chest3";
  else if(node->name() == "chest")
	  NameForWrite  = "Chest4";
  else if(node->name() == "neck")
	  NameForWrite  = "Neck";
  else if(node->name() == "head")
	  NameForWrite  = "Head";
  else if(node->name() == "lCollar")
	  NameForWrite  = "LeftCollar";
  else if(node->name() == "lShldr")
	  NameForWrite  = "LeftShoulder";
  else if(node->name() == "lForeArm")
	  NameForWrite  = "LeftElbow";
  else if(node->name() == "lHand")
	  NameForWrite  = "LeftWrist";
  else if(node->name() == "rCollar")
	  NameForWrite  = "RightCollar";
  else if(node->name() == "rShldr")
	  NameForWrite  = "RightShoulder";
  else if(node->name() == "rForeArm")
	  NameForWrite  = "RightElbow";
  else if(node->name() == "rHand")
	  NameForWrite  = "RightWrist";
  else if(node->name() == "lThigh")
	  NameForWrite  = "LeftHip";
  else if(node->name() == "lShin")
	  NameForWrite  = "LeftKnee";
  else if(node->name() == "lFoot")
	  NameForWrite  = "LeftAnkle";
  else if(node->name() == "lFoot1")
	  NameForWrite  = "LeftToe";
  else if(node->name() == "rThigh")
	  NameForWrite  = "RightHip";
  else if(node->name() == "rShin")
	  NameForWrite  = "RightKnee";
  else if(node->name() == "rFoot")
	  NameForWrite  = "RightAnkle";
  else if(node->name() == "rFoot1")
	  NameForWrite  = "RightToe";
  else if(node->name() == "Site")
	  NameForWrite  = "Site";

  out << bvhTypeName[node->type] << " " << NameForWrite << endl;

  bvhIndent(out, depth);
  out << "{" << endl;

  bvhIndent(out,depth+1);
  QString name=node->name();
  out << "OFFSET " << node->offset[0]*100 << " " << node->offset[1]*100 << " " << node->offset[2]*100 << endl;

  if(node->type!=BVH_END)
  {
    bvhIndent(out,depth+1);
    out << "CHANNELS " << node->numChannels << " ";
    for (int i=0;i<node->numChannels;i++)
      out << bvhChannelName[node->channelType[i]] << " ";

    out << endl;
  }
  for(int i=0;i<node->numChildren();i++)
    bvhWriteNode(node->child(i),out,depth+1);

  bvhIndent(out,depth);
  out << "}" << endl;
}

void BVH::bvhWriteNode( BVHNode* node,QTextStream& out,int depth, int writeFlag, int flagNextNode )
{
	bvhIndent(out,depth);
	QString NameForWrite;
	if(node->name() == "hip")
		NameForWrite  = "Hips";
	else if(node->name() == "hip1")
		NameForWrite  = "Chest";
	else if(node->name() == "hip2")
		NameForWrite  = "Chest2";
	else if(node->name() == "hip3")
		NameForWrite  = "Chest3";
	else if(node->name() == "chest")
		NameForWrite  = "Chest4";
	else if(node->name() == "neck")
		NameForWrite  = "Neck";
	else if(node->name() == "head")
		NameForWrite  = "Head";
	else if(node->name() == "lCollar")
		NameForWrite  = "LeftCollar";
	else if(node->name() == "lShldr")
		NameForWrite  = "LeftShoulder";
	else if(node->name() == "lForeArm")
		NameForWrite  = "LeftElbow";
	else if(node->name() == "lHand")
		NameForWrite  = "LeftWrist";
	else if(node->name() == "rCollar")
		NameForWrite  = "RightCollar";
	else if(node->name() == "rShldr")
		NameForWrite  = "RightShoulder";
	else if(node->name() == "rForeArm")
		NameForWrite  = "RightElbow";
	else if(node->name() == "rHand")
		NameForWrite  = "RightWrist";
	else if(node->name() == "lThigh")
		NameForWrite  = "LeftHip";
	else if(node->name() == "lShin")
		NameForWrite  = "LeftKnee";
	else if(node->name() == "lFoot")
		NameForWrite  = "LeftAnkle";
	else if(node->name() == "lFoot1")
		NameForWrite  = "LeftToe";
	else if(node->name() == "rThigh")
		NameForWrite  = "RightHip";
	else if(node->name() == "rShin")
		NameForWrite  = "RightKnee";
	else if(node->name() == "rFoot")
		NameForWrite  = "RightAnkle";
	else if(node->name() == "rFoot1")
		NameForWrite  = "RightToe";
	else if(node->name() == "Site")
		NameForWrite  = "Site";
	out << bvhTypeName[node->type] << " " << NameForWrite << endl;
	bvhIndent(out, depth);
	out << "{" << endl;
	bvhIndent(out,depth+1);
	QString name=node->name();
	if (writeFlag==1)
	{
		if(name=="lForeArm")
			flagNextNode=-1;
		if(name=="rForeArm")
			flagNextNode=1;
	}
	if (flagNextNode==-1)
		out << "OFFSET " << node->offset[1]*100 << " " << node->offset[0]*(-100) << " " << node->offset[2]*100 << endl;
	else if(flagNextNode==1)
		out << "OFFSET " << node->offset[1]*100 << " " << node->offset[0]*100 << " " << node->offset[2]*100 << endl;
	else
		out << "OFFSET " << node->offset[0]*100 << " " << node->offset[1]*100 << " " << node->offset[2]*100 << endl;
	if(node->type!=BVH_END)
	{
		bvhIndent(out,depth+1);
		out << "CHANNELS " << node->numChannels << " ";
		for (int i=0;i<node->numChannels;i++)
			out << bvhChannelName[node->channelType[i]] << " ";
		out << endl;
	}
	for(int i=0;i<node->numChildren();i++)
		bvhWriteNode(node->child(i),out,depth+1,writeFlag,flagNextNode);
	bvhIndent(out,depth);
	out << "}" << endl;
}
void BVH::bvhWriteFrame(BVHNode* node,QTextStream& out,int frame)
{
  Rotation rot=node->frameData(frame).rotation();
  Position pos=positionNode->frameData(frame).position();
  double qua[4]={rot.w,rot.x,rot.y,rot.z};
  double dcm[3][3];
  double eulrx,eulry,eulrz;
  //QUA2DCM(qua,dcm);

  //Position pos=positionNode->frameData(frame).position();

  // preserve channel order while writing
  if (node->getFather() != NULL && node->getFather()->name() != "position" && node->name() != "position")
  {
	   float   F[3][3],FI[3][3],T[3][3],R[3][3];
	   Rotation fa_rot=node->getFather()->frameData(frame).rotation();
	   double fa_qua[4]={fa_rot.w,fa_rot.x,fa_rot.y,fa_rot.z};
	   QUA2DCM(fa_qua,dcm);
	   eulry=atan2(-dcm[2][0],dcm[0][0]);
	   eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
	   eulrx=atan2(-dcm[1][2],dcm[1][1]);
	   eulrx=eulrx/PI*180;
	   eulry=eulry/PI*180;
	   eulrz=eulrz/PI*180;
	   toMatrix(F, eulrx,eulry,eulrz);
//	   if (node->getFather()->isCorrect == true)
//	   {
//		   toMatrix(FP, node->getFather()->correct[0],node->getFather()->correct[1], node->getFather()->correct[2]);
//		   MatrixxMatrix(tmp, F, FP);
//		   MatrixInverse(FPI,FP);
//		   MatrixxMatrix(F,FPI,tmp);
//		   	   for (int i = 0; i < 3; i++)
//		   		   for( int j=0; j<3; j++)
//		   			   F[i][j] = tmp[i][j];
//	   }

	   MatrixInverse(FI, F);
	   QUA2DCM(qua,dcm);
	   eulry=atan2(-dcm[2][0],dcm[0][0]);
	   eulrz=atan2(dcm[1][0],dcm[0][0]/cos(eulry));
	   eulrx=atan2(-dcm[1][2],dcm[1][1]);
	   eulrx=eulrx/PI*180;
	   eulry=eulry/PI*180;
	   eulrz=eulrz/PI*180;
	   toMatrix(T, eulrx, eulry ,eulrz);
//	   if (node->isCorrect == true)
//	   {
//		   toMatrix(TP, node->correct[0],node->correct[1], node->correct[2]);
//		   MatrixxMatrix(tmp, T, TP);
//		   MatrixInverse(TPI,TP);
//		   MatrixxMatrix(T,TPI,tmp);
//		   	   for (int i = 0; i < 3; i++)
//		   		   for( int j=0; j<3; j++)
//		   			   T[i][j] = tmp[i][j];
//	   }
	   MatrixxMatrix(R, T, FI);
	   float x,y,z;
	   toEuler( x , y , z, R);
	   rot.x = x;
	   rot.y = y;
	   rot.z = z;

  }
  else{
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
  for (int i=0; i<node->numChannels; i++)
  {
    float value=0.0;
	bool iship = false;
    BVHChannelType type=node->channelType[i];

	if     (type==BVH_XPOS) value=pos.x*100;
	else if(type==BVH_YPOS) value=pos.y*100;
	else if(type==BVH_ZPOS) value=pos.z*100;

	else if(type==BVH_XROT) value=rot.x;
	else if(type==BVH_YROT) value=rot.y;
	else if(type==BVH_ZROT) value=rot.z;
    if (value < 0)
    	out << value << "\t";
    else
    	out <<" "<< value << "\t";
  }

  for(int i=0;i<node->numChildren();i++)
    bvhWriteFrame(node->child(i),out,frame);
}

void BVH::bvhWriteFrame( BVHNode* node,QTextStream& out,int frame,int writeFlag )
{
	QString name=node->name();
	Rotation rot_origin=node->frameData(frame).rotation();
	Position pos_origin=positionNode->frameData(frame).position();
	if (writeFlag==1)
	{
		if (name=="rHand"||name=="rForeArm"||name=="rShldr")
		{
			double TA[3][3]={0,1,0,-1,0,0,0,0,1};
			double TAI[3][3]={0,1,0,-1,0,0,0,0,1};
			double qua[4]={rot_origin.w,rot_origin.x,rot_origin.y,rot_origin.z};
			double dcm[3][3];
			QUA2DCM(qua,dcm);
			double dcmtmp[3][3];
			MTXxxMTX(dcmtmp,dcm,TA);
			DCM2QUA(dcmtmp,qua);
			node->addKeyframe(frame,node->frameData(frame).position(),Rotation(qua[0],qua[1],qua[2],qua[3]));
		}
		if (name=="lHand"||name=="lForeArm"||name=="lShldr")
		{
			double TA[3][3]={0,1,0,-1,0,0,0,0,1};
			double TAI[3][3]={0,1,0,-1,0,0,0,0,1};
			double qua[4]={rot_origin.w,rot_origin.x,rot_origin.y,rot_origin.z};
			double dcm[3][3];
			QUA2DCM(qua,dcm);
			double dcmtmp[3][3];
			MatrixInv(TA);
			MTXxxMTX(dcmtmp,dcm,TA);
			DCM2QUA(dcmtmp,qua);
			node->addKeyframe(frame,node->frameData(frame).position(),Rotation(qua[0],qua[1],qua[2],qua[3]));
		}
	}
	Rotation rot=node->frameData(frame).rotation();
	Position pos=positionNode->frameData(frame).position();
	double qua[4]={rot.w,rot.x,rot.y,rot.z};
	double dcm[3][3];
	double eulrx,eulry,eulrz;
	if (node->getFather() != NULL && node->getFather()->name() != "position" && node->name() != "position")
	{
		float   F[3][3],FP[3][3],FPI[3][3],FI[3][3],T[3][3],TP[3][3],TPI[3][3],R[3][3];
		float tmp[3][3];
		Rotation fa_rot=node->getFather()->frameData(frame).rotation();
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
	else{
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
	for (int i=0; i<node->numChannels; i++)
	{
		float value=0.0;
		bool iship = false;
		BVHChannelType type=node->channelType[i];
		if     (type==BVH_XPOS) value=pos.x*100;
		else if(type==BVH_YPOS) value=pos.y*100;
		else if(type==BVH_ZPOS) value=pos.z*100;
		else if(type==BVH_XROT) value=rot.x;
		else if(type==BVH_YROT) value=rot.y;
		else if(type==BVH_ZROT) value=rot.z;
		if (value < 0)
			out << value << "\t";
		else
			out <<" "<< value << "\t";
	}
	for(int i=0;i<node->numChildren();i++)
		bvhWriteFrame(node->child(i),out,frame,1);
}
void BVH::bvhWrite(Animation* anim, const QString& file)
{
  int i;
  QFile f(file);
  f.open(QFile::WriteOnly);
  QTextStream out(&f);
  out.setNumberFlags(QTextStream::ForcePoint);
  out.setRealNumberPrecision(5);

  BVHNode* root=anim->getMotion();
  positionNode=anim->getNode(0);

  out << "HIERARCHY" << endl;
  bool flag3dMaxBVH = true;
  if(flag3dMaxBVH==0)
	bvhWriteNode(root,out,0);
  else
	bvhWriteNode(root,out,0,1,0);

  out << "MOTION" << endl;

  out << "Frames:\t" << anim->getNumberOfFrames() << endl;
  out << "Frame Time:\t" << anim->frameTime() << endl;

  for(i=0;i<anim->getNumberOfFrames();i++)
  {
	if(flag3dMaxBVH==0)
		bvhWriteFrame(root,out,i);
	else
		bvhWriteFrame(root,out,i,1);
    out << endl;
  }

  f.close();
}

void BVH::avmWriteKeyFrame(BVHNode* root,QTextStream& out)
{
  const QList<int> keys=root->keyframeList();
  // no key frames (usually at joint ends), just write a 0\n line
  if(keys.count()==0)
    out << "0" << endl;

  // write line of key files
  else
  {
    // write number of key files
    out << keys.count()-1 << " ";

    // skip frame 0 (always key frame) while saving and write all keys in a row
    for(unsigned int i=1;i< (unsigned int) keys.count();i++)
      out << keys[i] << " ";

    out << endl;
  }

  for(int i=0;i<root->numChildren();i++)
    avmWriteKeyFrame(root->child(i),out);
}

// writes ease in / out data
void BVH::avmWriteKeyFrameProperties(BVHNode* root,QTextStream& out)
{
  const QList<int> keys=root->keyframeList();

  // NOTE: remember, ease in/out data always takes first frame into account
  out << keys.count() << " ";

  // NOTE: remember, ease in/out data always takes first frame into account
  for(unsigned int i=0;i< (unsigned int) keys.count();i++)
  {
    int type=0;

    if(root->keyframeDataByIndex(i).easeIn()) type|=1;
    if(root->keyframeDataByIndex(i).easeOut()) type|=2;

    out << type << " ";
  }
  out << endl;

  for(int i=0;i<root->numChildren();i++)
    avmWriteKeyFrameProperties(root->child(i),out);
}

void BVH::avmWrite(Animation* anim,const QString& file)
{
  QFile f(file);
  f.open(QFile::WriteOnly);
  QTextStream out(&f);
  out.setNumberFlags(QTextStream::ForcePoint);
  out.setRealNumberPrecision(7);

  BVHNode* root=anim->getMotion();
  positionNode=anim->getNode(0);

  out << "HIERARCHY" << endl;
  bvhWriteNode(root,out,0);

  out << "MOTION" << endl;

  out << "Frames:\t" << anim->getNumberOfFrames() << endl;

  out << "Frame Time:\t" << anim->frameTime() << endl;
  for(int i=0;i<anim->getNumberOfFrames();i++)
  {
    bvhWriteFrame(root,out,i);
    out << endl;
  }

  avmWriteKeyFrame(root,out);
  out << "Properties" << endl;

  avmWriteKeyFrameProperties(root,out);

  // write remaining properties
  out << "Scale: " << anim->getAvatarScale() << endl;
  out << "Figure: " << anim->getFigureType() << endl;
  out << "LoopIn: " << anim->getLoopInPoint() << endl;
  out << "LoopOut: " << anim->getLoopOutPoint() << endl;

  // HACK: add-on for position key support - this needs to be revised badly
  // HACK: we need a new file format that makes it easier to add new features
  out << "Positions: ";
  avmWriteKeyFrame(positionNode,out);

  out << "PositionsEase: ";
  avmWriteKeyFrameProperties(positionNode,out);
   
  f.close();
}

void BVH::animWrite(Animation* anim,const QString& file)
{
  // rudimentary file type identification from filename
  if(file.endsWith(".bvh",Qt::CaseInsensitive))
    bvhWrite(anim,file);
  else if(file.endsWith(".avm",Qt::CaseInsensitive))
    avmWrite(anim,file);
}

void BVH::bvhPrintNode(BVHNode* n,int depth)
{
  int i;
  QString indent;
  for(i=0;i<depth;i++)
    indent+="    ";
  qDebug() << indent << n->name() << " (" << n->offset[0] << n->offset[1] << n->offset[2] << ")";
  for(i=0;i<n->numChildren();i++)
    bvhPrintNode(n->child(i), depth+1);
}

BVHNode* BVH::bvhFindNode(BVHNode* root,const QString& name) const
{
  BVHNode *node;
  if(!root) return NULL;
  if(root->name()==name) return root;

  for(int i=0;i<root->numChildren();i++)
  {
    if((node=bvhFindNode(root->child(i),name))) return node;
  }

  return NULL;
}

void BVH::bvhGetChannelLimits(BVHNode* node,BVHChannelType type,double* min, double* max)
{
  int i;
  if(!node)
  {
    *min=-10000;
    *max=10000;
    return;
  }
  for(i=0;i<node->numChannels;i++)
  {
    if(node->channelType[i]==type)
    {
      *min=node->channelMin[i];
      *max=node->channelMax[i];
    }
  }
}


const QString BVH::bvhGetNameHelper(BVHNode* node,int index)
{
  nodeCount++;
  if(nodeCount==index) return node->name();

  for(int i=0;i<node->numChildren();i++)
  {
    const QString val=bvhGetNameHelper(node->child(i),index);
    if(!val.isEmpty()) return val;
  }
  return QString();
}

const QString BVH::bvhGetName(BVHNode* node,int index)
{
  nodeCount=0;
  return bvhGetNameHelper(node, index);
}

int BVH::bvhGetIndexHelper(BVHNode* node,const QString& name)
{
  nodeCount++;
  if(node->name()==name) return nodeCount;

  for(int i=0;i<node->numChildren();i++)
  {
    int val;
    if((val=bvhGetIndexHelper(node->child(i),name)))
      return val;
  }
  return 0;
}

int BVH::bvhGetIndex(BVHNode* node,const QString& name)
{
  nodeCount=0;
  return bvhGetIndexHelper(node,name);
}

void BVH::bvhCopyOffsets(BVHNode* dst,BVHNode* src)
{
  int i;
  if(!dst || !src) return;
  dst->offset[0]=src->offset[0];
  dst->offset[1]=src->offset[1];
  dst->offset[2]=src->offset[2];
  for(i=0;i<src->numChildren();i++)
  {
    bvhCopyOffsets(dst->child(i),src->child(i));
  }
}

void BVH::bvhGetFrameDataHelper(BVHNode* node,int frame)
{
  if(node->type!=BVH_END)
  {
    rotationCopyBuffer.append(node->frameData(frame).rotation());

//    rotationCopyBuffer[rotationCopyBuffer.count()-1].bodyPart=node->name(); // not necessary but nice for debugging
//    qDebug(QString("copying frame data for %1 frame number %2 (%3)")
//          .arg(node->name()).arg(frame).arg(positionCopyBuffer[rotationCopyBuffer.count()-1].bodyPart));
  }


  for(int i=0;i<node->numChildren();i++)
    bvhGetFrameDataHelper(node->child(i),frame);
}

void BVH::bvhGetFrameData(BVHNode* node,int frame)
{
  if(!node) return;

  rotationCopyBuffer.clear();
  positionCopyBuffer=lastLoadedPositionNode->frameData(frame).position();
  bvhGetFrameDataHelper(node,frame);
}

void BVH::bvhSetFrameDataHelper(BVHNode* node,int frame)
{
  // if this node is not the end of a joint child structure
  if(node->type!=BVH_END)
  {
    // add the node as key frame
    node->addKeyframe(frame,Position(),rotationCopyBuffer[pasteIndex]);
//    qDebug(QString("pasting frame data for %1 frame number %2 (%3)").arg(node->name()).arg(frame).arg(rotationCopyBuffer[pasteIndex].bodyPart));
    // increment paste buffer counter
    pasteIndex++;
  }

  // traverse down the child list, call this function recursively
  for(int i=0;i<node->numChildren();i++)
    bvhSetFrameDataHelper(node->child(i),frame);
}

void BVH::bvhSetFrameData(BVHNode* node,int frame)
{
  if(!node) return;

  // reset paste buffer counter
  pasteIndex=0;
  lastLoadedPositionNode->addKeyframe(frame,positionCopyBuffer,Rotation());
  // paste all keyframes for all joints
  bvhSetFrameDataHelper(node,frame);
}

void BVH::bvhDelete(BVHNode* node)
{
  if(node)
  {
    for(int i=0;i<node->numChildren();i++)
      bvhDelete(node->child(i));

    delete node;
  }
}
