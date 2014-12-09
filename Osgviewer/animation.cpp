/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */

///#include <QApplication.h>
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
// #include "main.h"

#include "animation.h"
#include "rotation.h"
#include "bvh.h"
#include "settings.h"

Animation::Animation(BVH* newBVH,const QString& bvhFile) :
  frame(0),totalFrames(0),mirrored(false)
{
  qDebug("Animation::Animation(%lx)",(unsigned long) this);

  bvh=newBVH;
  if(!bvh)
  {
    qDebug("Animation::Animation(): BVH initialisation failed.");
    return;
  }

  QString fileName;

  // pick up path from src.pro qmake file's DEFINES section, i.e. usr/share/qavimator
  //dataPath=QAVIMATOR_DATAPATH;
//#ifdef __APPLE__
//  dataPath=QApplication::applicationDirPath() + "/../Resources";
//#else
//  dataPath=QAVIMATOR_DATAPATH;
//#endif

  // load BVH that defines motion
  if (!bvhFile.isEmpty())
    fileName=bvhFile;
  else
    fileName=DEFAULT_POSE;

  loadBVH(fileName);

  useRotationLimits(true);
 // setNumberOfFrames(bvh->lastLoadedNumberOfFrames);
  setNumberOfFrames(1500);
  setAvatarScale(bvh->lastLoadedAvatarScale);
  setFigureType(bvh->lastLoadedFigureType);
  setLoopInPoint(bvh->lastLoadedLoopIn);
  setLoopOutPoint(bvh->lastLoadedLoopOut);
//  setFrameTime(bvh->lastLoadedFrameTime);
  setFrameTime(0.04);
  positionNode=bvh->lastLoadedPositionNode;
  addKeyFrameAllJoints();

//  ikTree.set(frames);
//  setIK(IK_LHAND, false);
//  setIK(IK_RHAND, false);
//  setIK(IK_LFOOT, false);
//  setIK(IK_RFOOT, false);

  setLoop(false);
  setDirty(false);

  currentPlayTime=0.0;
  setPlaystate(PLAYSTATE_STOPPED);
  //防止sdk全局变量时出错
  //connect(&timer,SIGNAL(timeout()),this,SLOT(playbackTimeout()));
  }

Animation::~Animation()
{
  qDebug("Animation::~Animation(%lx)",(unsigned long) this);

  // remove rotation keyframes
  if(frames) bvh->bvhDelete(frames);
  // remove position keyframes
  if(positionNode) delete positionNode;
}
BVHNode* Animation::getPositionNode()
{
	return positionNode;
}
void Animation::loadBVH(const QString& bvhFile)
{
  qDebug("Animation::loadBVH(%s)",bvhFile.toLatin1().constData());
  QString limFile=LIMITS_FILE;
  frames=bvh->animRead(bvhFile, limFile);
  setFrame(0);
}

void Animation::saveBVH(const QString& bvhFile)
{
  qDebug("Animation::saveBVH(%s)",bvhFile.toLatin1().constData());
  bvh->animWrite(this,bvhFile);
  setDirty(false);
}

int Animation::fps() const
{
  if(frames)
    return framesPerSecond;
  else
    return 0;
}

void Animation::setFPS(int fps)
{
  if(frames)
  {
    framesPerSecond=fps;
    setDirty(true);
  }
}

// convenience
void Animation::setFrameTime(double frameTime)
{
  framesPerSecond=(int) (1.0/frameTime+0.5);
}

// convenience
double Animation::frameTime() const
{
  return 1.0/framesPerSecond;
}

int Animation::getNumberOfFrames()
{
// qDebug("Animation::getNumberOfFrames()=%d",totalFrames);
  return totalFrames;
}

void Animation::setNumberOfFrames(int num)
{
  qDebug("Animation::setNumberOfFrames(%d)",num);
  totalFrames=num;
  setDirty(true);
  //emit numberOfFrames(num);
}

int Animation::getFrame()
{
  return frame;
}

void Animation::setFrame(int frameNumber)
{
  if(frameNumber>=0 && frameNumber<totalFrames && frame!=frameNumber)
  {
//    for (int i=0; i<NUM_IK; i++) {
//      setIK((IKPartType)i, false);
//    }
    frame=frameNumber;


    //emit currentFrame(frame);
//    emit frameChanged();
  }
}

void Animation::setMirrored(bool mirror)
{
  mirrored=mirror;
}

bool Animation::getMirrored()
{
  return mirrored;
}

// get next frame and take care of looping
int Animation::stepForward()
{
  if(frames)
  {
    int nextFrame=(frame+1) % totalFrames;

    if(loop)
    {
      if(!nextFrame || nextFrame>loopOutPoint) nextFrame=loopInPoint;
    }
    else
    {
      // do not wrap to the beginning when not looping
      if(!nextFrame) nextFrame=frame;
    }
    setFrame(nextFrame);
	

    return nextFrame;
  }
  return 0;
}

void Animation::setEaseIn(BVHNode* node,int frameNum,bool state)
{
  if(!node)
  {
    qDebug("Animation::setEaseIn(): node==0!");
    return;
  }

  if(node->isKeyframe(frameNum))
  {
    setDirty(true);
    node->setEaseIn(frameNum,state);
    // tell main class that the keyframe has changed
    //emit redrawTrack(getPartIndex(node));
  }
}

void Animation::setEaseOut(BVHNode* node,int frameNum,bool state)
{
  if(!node)
  {
    qDebug("Animation::setEaseOut(): node==0!");
    return;
  }

  if(node->isKeyframe(frameNum))
  {
    setDirty(true);
    node->setEaseOut(frameNum,state);
    // tell main class that the keyframe has changed
    //emit redrawTrack(getPartIndex(node));
  }
}

bool Animation::easeIn(BVHNode* node,int frameNum)
{
  if(!node)
  {
    qDebug("Animation::easeIn(): node==0!");
    return false;
  }

  if(node->isKeyframe(frameNum))
  {
    return node->easeIn(frameNum);
  }
  qDebug("Animation::easeIn(%s): requested easeIn for non-keyframe!",node->name().toLatin1().constData());
  return false;
}

bool Animation::easeOut(BVHNode* node,int frameNum)
{
  if(!node)
  {
    qDebug("Animation::easeOut(): node==0!");
    return false;
  }

  if(node->isKeyframe(frameNum))
  {
    return node->easeOut(frameNum);
  }
  qDebug("Animation::easeOut(%s): requested easeOut for non-keyframe!",node->name().toLatin1().constData());
  return false;
}

void Animation::setLoopInPoint(int inFrame)
{
//  qDebug("Animation::setLoopInPoint(%d)",inFrame);
  loopInPoint=inFrame;
  setDirty(true);
}

int Animation::getLoopInPoint()
{
  return loopInPoint;
}

void Animation::setLoopOutPoint(int outFrame)
{
//  qDebug("Animation::setLoopOutPoint(%d)",outFrame);
  loopOutPoint=outFrame;
  setDirty(true);
}

int Animation::getLoopOutPoint()
{
  return loopOutPoint;
}

void Animation::setRotation(BVHNode* node,double w,double x,double y,double z)
{
  if (node)
  {
    //qDebug(QString("Animation::setRotation(")+jointName+")");
    if(node->isKeyframe(frame))
    	node->setKeyframeRotation(frame,Rotation(w,x,y,z));
    else
    {
      node->addKeyframe(frame,node->frameData(frame).position(),Rotation(w,x,y,z));
      setEaseIn(node,frame,Settings::easeIn());
      setEaseOut(node,frame,Settings::easeOut());
    }

    //      node->dumpKeyframes();
    /*
    BVHNode* mirrorNode=node->getMirror();
    if(mirrored && mirrorNode)
    {
      // new keyframe system
      if(mirrorNode->isKeyframe(frame))
        mirrorNode->setKeyframeRotation(frame,Rotation(x,-y,-z));
      else
      {
        mirrorNode->addKeyframe(frame,node->frameData(frame).position(),Rotation(x,-y,-z));
        setEaseIn(mirrorNode,frame,Settings::easeIn());
        setEaseOut(mirrorNode,frame,Settings::easeOut());
      }

      // tell timeline that this mirrored keyframe has changed (added or changed is the same here)
      emit redrawTrack(getPartIndex(mirrorNode));
    }
    */
//    setDirty(true);
    // tell timeline that this keyframe has changed (added or changed is the same here)
//    emit redrawTrack(getPartIndex(node));
    //emit frameChanged();
  }
  else
  {
    qDebug("Animaiton::setRotation(): node==0!");
  }
}

Rotation Animation::getRotation(BVHNode* node)
{
  if(node)
  {
    return node->frameData(frame).rotation();
  }
  qDebug("Animation::getRotation(): node==0!");
  return Rotation();
}

void Animation::useRotationLimits(bool flag)
{
  limits=flag;

}

RotationLimits Animation::getRotationLimits(BVHNode* node)
{
  if(node)
  {
    double xMin,yMin,zMin,xMax,yMax,zMax;

    if(node->type==BVH_POS)
    {
      xMin=yMin=zMin=xMax=yMax=zMax=0;
    }
//    else if(limits)
//    {
//      bvh->bvhGetChannelLimits(node,BVH_XROT,&xMin,&xMax);
//      bvh->bvhGetChannelLimits(node,BVH_YROT,&yMin,&yMax);
//      bvh->bvhGetChannelLimits(node,BVH_ZROT,&zMin,&zMax);
//    }
    else
    {
      xMin=yMin=-180;
      xMax=yMax=180;
      zMin=-90;
      zMax=90;
    }

    RotationLimits rotLimit(node->name(),xMin,xMax,yMin,yMax,zMin,zMax);
    return rotLimit;
  }
  qDebug("Animation::getRotationLimits(): node==0!");
  return RotationLimits(QString::null,0,0,0,0,0,0);
}

int Animation::getRotationOrder(const QString& jointName)
{
  BVHNode* node=bvh->bvhFindNode(frames,jointName);
  if(node)
  {
    return node->channelOrder;
  }
  return 0;
}

void Animation::setPosition(double x,double y,double z)
{
  // new keyframe system
  if(positionNode->isKeyframe(frame))
    positionNode->setKeyframePosition(frame,Position(x,y,z));
  else
  {
    positionNode->addKeyframe(frame,Position(x,y,z),Rotation());
    setEaseIn(positionNode,frame,Settings::easeIn());
    setEaseOut(positionNode,frame,Settings::easeOut());
  }
  setDirty(true);
  // tell timeline that this keyframe has changed (added or changed is the same here)
  //emit redrawTrack(0);
  //emit frameChanged();
}

Position Animation::getPosition()
{
  return positionNode->frameData(frame).position();
}

const QString Animation::getPartName(int index) const
{
  //  exception fot position pseudonode
  if(index==0) return positionNode->name();
  return bvh->bvhGetName(frames,index);
}

int Animation::getPartIndex(BVHNode* node)
{
  if(node==positionNode) return 0;
  return bvh->bvhGetIndex(frames,node->name());
}

BVHNode* Animation::getMotion()
{
  return frames;
}

BVHNode* Animation::getEndSite(const QString& rootName)
{
  BVHNode* node=bvh->bvhFindNode(frames,rootName);
  while(node && node->numChildren()>0)
  {
    node=node->child(0);
  }
  return node;
}

void Animation::recursiveAddKeyFrame(BVHNode* joint)
{
  if(joint->type!=BVH_END)
    addKeyFrame(joint);

  for(int i=0;i<joint->numChildren();i++)
    recursiveAddKeyFrame(joint->child(i));

  setDirty(true);
}

void Animation::addKeyFrameAllJoints()
{
  addKeyFrame(getNode(0));
  recursiveAddKeyFrame(frames);
}

void Animation::addKeyFrame(BVHNode* joint)
{
  joint->addKeyframe(frame,getPosition(),getRotation(joint));

  setEaseIn(joint,frame,Settings::easeIn());
  setEaseOut(joint,frame,Settings::easeOut());

  setDirty(true);

  //emit redrawTrack(getPartIndex(joint));
  //emit frameChanged();
}

bool Animation::isKeyFrameHelper(BVHNode *joint)
{
  if(joint->isKeyframe(frame))
    return true;

  for (int i=0;i<joint->numChildren();i++)
  {
    if(isKeyFrameHelper(joint->child(i)))
    {
      return true;
    }
  }

  return false;
}

bool Animation::isKeyFrame(const QString& jointName)
{
  if(jointName.isEmpty())
     return isKeyFrame();
  else
  {
    BVHNode* node=bvh->bvhFindNode(frames,jointName);
    return node->isKeyframe(frame);
  }
  qDebug("Animation::isKeyFrame('%s'): no node found.",jointName.toLatin1().constData());
}

bool Animation::isKeyFrame()
{
  if(positionNode->isKeyframe(frame)) return true;
  return isKeyFrameHelper(frames);
}

bool Animation::isKeyFrame(int jointNumber,int frame)
{
  const BVHNode* joint=getNode(jointNumber);
  return joint->isKeyframe(frame);
}

void Animation::deleteKeyFrame(BVHNode* joint,int frameNum,bool silent)
{
  // never delete first keyframe
  if(frameNum)
  {
    joint->deleteKeyframe(frameNum);
    setDirty(true);
  }

  // if silent is true then only send a signal to the timeline but not to the animation view
  //if(!silent) emit frameChanged();
 // emit redrawTrack(getPartIndex(joint));
}

void Animation::deleteKeyFrame(int jointNumber,int frameNum)
{
  // frames should always be current frame, but better play safe for future enhancements
  setFrame(frameNum);
  if(jointNumber)
  {
     BVHNode* joint=getNode(jointNumber);
     if(joint->isKeyframe(frameNum)) deleteKeyFrame(joint,frameNum);
  }
  else if(isKeyFrame()) deleteKeyFrameAllJoints();

  // tell main class that the keyframe has changed
  //emit currentFrame(frameNum);
}

void Animation::recursiveDeleteKeyFrame(BVHNode* joint)
{
  deleteKeyFrame(joint,frame);

  for(int i=0;i<joint->numChildren();i++)
    recursiveDeleteKeyFrame(joint->child(i));
}

void Animation::deleteKeyFrameAllJoints()
{
  // never delete the first keyframe
  if(frame==0) return;
  deleteKeyFrame(getNode(0),frame);
  recursiveDeleteKeyFrame(frames);
}

bool Animation::toggleKeyFrame(BVHNode* node)
{
//  qDebug("Animation::toggleKeyFrame(node): node %ld",(unsigned long) node);
  if(!node)
    return toggleKeyFrameAllJoints();
  else
  {
    if (node->isKeyframe(frame))
    {
      deleteKeyFrame(node,frame);
      return false;
    }
    else
    {
      addKeyFrame(node);
      return true;
    }
  }
}

// returns TRUE if frame is now a keyframe for entire animation, FALSE if not
bool Animation::toggleKeyFrameAllJoints()
{
  if(frame==0)
    return true;  // first frame will always stay keyframe

  if(isKeyFrame())
  {
    deleteKeyFrameAllJoints();
    return false;
  }
  else
  {
    addKeyFrameAllJoints();
    return true;
  }
}

void Animation::cutFrame()
{
  // copy frame data into copy buffer
  copyFrame();
  // always delete frame from all tracks
  deleteFrame(0,frame);
}

void Animation::copyFrame()
{
  bvh->bvhGetFrameData(frames,frame);
}

void Animation::pasteFrame()
{
  bvh->bvhSetFrameData(frames,frame);
  addKeyFrameAllJoints();
}



int Animation::numKeyFrames(int jointNumber)
{
  BVHNode* node=bvh->bvhFindNode(frames,getPartName(jointNumber));
//  qDebug(QString("Animation::numKeyFrames(): joint number %1 has %2 keyframes").arg(jointNumber).arg(node->numKeyFrames));
  return node->numKeyframes();
}

// copies the position and rotation data of one body part to another key frame position
void Animation::copyKeyFrame(int jointNumber,int from,int to)
{
  // move keyframe in copy mode
  moveKeyFrame(jointNumber,from,to,true);
}

// moves the position and rotation data of one body part to another key frame position
void Animation::moveKeyFrame(int jointNumber,int from,int to,bool copy)
{
  qDebug("Animation::moveKeyFrame(): jointNumber: %d",jointNumber);

  // make sure we don't drag a trail of mirror keys behind
  setMirrored(false);

  // set frame pointer to source frame position
  setFrame(from);

  // get the joint structure
  BVHNode* joint=getNode(jointNumber);
  const FrameData& frameData=joint->frameData(from);
//  frameData.dump();

  // block all further signals to avoid flickering
  blockSignals(true);

  // silently (true) delete key frame if not copy mode
  // we do copy mode here to avoid code duplication
  if(!copy) deleteKeyFrame(joint,from,true);

  setFrame(to);

  // move rotation or position of the body part
  if(joint->type==BVH_POS)
  {
    Position pos=frameData.position();
    setPosition(pos.x,pos.y,pos.z);
  }
  else
  {
    Rotation rot=frameData.rotation();
    setRotation(joint,rot.w,rot.x,rot.y,rot.z);
  }
  // only now set ease in/out, because setRotation/setPosition sets to default when the
  // target position has no keyframe yet
  joint->setEaseIn(to,frameData.easeIn());
  joint->setEaseOut(to,frameData.easeOut());
  // now re-enable signals so we get updates on screen
  blockSignals(false);

  // tell timeline where we are now
  //emit currentFrame(frame);
}

bool Animation::compareFrames(const BVHNode* node,int key1,int key2) const
{
  if(node) return node->compareFrames(key1,key2);
  qDebug("Animation::compareFrames(node): node==0!");
  return false;
}

const FrameData Animation::keyframeDataByIndex(int jointNumber,int index)
{
  BVHNode* joint=getNode(jointNumber);
  return joint->keyframeDataByIndex(index);
}

BVHNode* Animation::getNode(int jointNumber)
{
  //####
  // joint number 0 needs to return the hip position pseudonode
  if(jointNumber==0) return positionNode;
  // get the joint structure
  return bvh->bvhFindNode(frames,getPartName(jointNumber));
}

void Animation::insertFrameHelper(BVHNode* joint,int frame)
{
  joint->insertFrame(frame);
  for(int i=0;i<joint->numChildren();i++)
    insertFrameHelper(joint->child(i),frame);
}

void Animation::insertFrame(int track,int pos)
{
  if(track==-1)
  {
    // insert positional frame
    BVHNode* joint=getNode(0);
    if(joint) joint->insertFrame(pos);
    // insert all rotational frames
    insertFrameHelper(frames,pos);
  }
  else
  {
    BVHNode* joint=getNode(track);
    if(joint) joint->insertFrame(frame);
  }
  setDirty(true);
  //emit frameChanged();
}

// recursively remove frames from joint and all its children
void Animation::deleteFrameHelper(BVHNode* joint,int frame)
{
//  qDebug("Animation::deleteFrameHelper(joint %s,frame %d)",joint->name().toLatin1().constData(),frame);
  joint->deleteFrame(frame);
  for(int i=0;i<joint->numChildren();i++)
    deleteFrameHelper(joint->child(i),frame);
  //emit redrawTrack(getPartIndex(joint));
}

// delete frame from a joint, if track==0 recursively delete from all joints
void Animation::deleteFrame(int track,int pos)
{
//  qDebug("Animation::deleteFrame(joint %d,frame %d)",track,frame);

  if(track==-1)
  {
    // delete positional frame
    BVHNode* joint=getNode(0);
    if(joint) joint->deleteFrame(pos);
    // delete all rotational frames
    deleteFrameHelper(frames,pos);
  }
  else
  {
    BVHNode* joint=getNode(track);
    if(joint) joint->deleteFrame(pos);
  }
  setDirty(true);
  //emit frameChanged();
}

void Animation::optimizeHelper(BVHNode* joint)
{
  if(joint->type!=BVH_END)
  {
    joint->optimize();
    //emit redrawTrack(getPartIndex(joint));
  }

  for(int i=0;i<joint->numChildren();i++)
    optimizeHelper(joint->child(i));
}

void Animation::optimize()
{
  optimizeHelper(positionNode);
  optimizeHelper(frames);
  setDirty(true);
}



bool Animation::dirty() const
{
  return isDirty;
}

void Animation::setDirty(bool state)
{
  isDirty=state;
  //emit animationDirty(state);
}

void Animation::setLoop(bool on)
{
  loop=on;
}

float Animation::getAvatarScale()
{
  return avatarScale;
}

void Animation::setAvatarScale(float newScale)
{
  avatarScale=newScale;
}

Animation::FigureType Animation::getFigureType()
{
  return figureType;
}

void Animation::setFigureType(FigureType type)
{
  // safety check if figure is valid
  if(type>=0 && type<NUM_FIGURES)
    figureType=type;
}

void Animation::nextPlaystate()
{
}

void Animation::setPlaystate(PlayState state)
{
  switch(state)
  {
    case PLAYSTATE_LOOPING:
    case PLAYSTATE_PLAYING:
	  //tandunming修改 解决构造函数导致的运行错误
	  connect(&timer,SIGNAL(timeout()),this,SLOT(playbackTimeout()));
      timer.start(PLAYBACK_RESOLUTION);
      break;
    case PLAYSTATE_STOPPED:
      timer.stop();
      break;
    default:
      break;
  }

  playstate=state;
}

// slot
void Animation::playbackTimeout()
{
//  qDebug("Animation::playbackTimeout()");

  currentPlayTime+=PLAYBACK_RESOLUTION/1000.0;
	qDebug("step \n");
  while(currentPlayTime>=frameTime())
  {
//    qDebug("Animation::playbackTimeout(%lx): !!! currentPlayTime=%f frameTime=%f",this,currentPlayTime,frameTime);
    currentPlayTime-=frameTime();
    stepForward();
  }
}
