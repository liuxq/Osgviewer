/***************************************************************************
 *   Copyright (C) 2006 by Zi Ree   *
 *   Zi Ree @ SecondLife   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "stdafx.h"
#include <math.h>

#include "bvhnode.h"

BVHNode::BVHNode(const QString& name)
{
//  qDebug(QString("BVHNode::BVHNode(%1)").arg(name));
  setName(name);

  // clean out lists
  keyframes.clear();
  children.clear();
  father = NULL;

  // have clean one-time cache
  flushFrameCache();

  numChannels=0;
  isCorrect = false;
  isScale = false;
  scale = 1;
  isNCalibrate = false;
  n_calibrate[0] = 0;
  n_calibrate[1] = 0;
  n_calibrate[2] = 0;
  isN2Calibrate = false;
  n2_calibrate[0] = 0;
  n2_calibrate[1] = 0;
  n2_calibrate[2] = 0;
  t_pose[0] = 0;
  t_pose[1] = 0;
  t_pose[2] = 0;


}

BVHNode::~BVHNode()
{
//  qDebug(QString("BVHNode::~BVHNode(%1)").arg(name()));
}

const QString& BVHNode::name() const
{
  return m_name;
}

void BVHNode::setName(const QString& newName)
{
  m_name=newName;
}

int BVHNode::numChildren() const
{
  return children.count();
}

BVHNode* BVHNode::child(int num)
{
  return children.at(num);
}

void BVHNode::addChild(BVHNode* newChild)
{
// qDebug(QString("BVHNode(%1): addChild(%2)").arg(name()).arg(newChild->name()));
  children.append(newChild);
}

void BVHNode::insertChild(BVHNode* newChild,int index)
{
// qDebug(QString("BVHNode(%1): insertChild(%2,%3)").arg(name()).arg(newChild->name()).arg(index));
  children.insert(index,newChild);
}

void BVHNode::removeChild(BVHNode* child)
{
// qDebug(QString("BVHNode(%1): removeChild(%2)").arg(name()).arg(child->name()));
  children.removeAll(child);
}
BVHNode* BVHNode::getFather()
{
	return father;
}
void BVHNode::addFather(BVHNode* fa)
{
	father = fa;
}

void BVHNode::removeFather()
{
	father = NULL;
}
void BVHNode::addKeyframe(int frame,Position pos,Rotation rot)
{
//  qDebug(QString("addKeyframe(%1)").arg(frame));
 keyframes[frame]=FrameData(frame,pos,rot);
//  if(frame==0 && name()=="Hips") qDebug(QString("BVHNode::addKeyframe(%1,<%2,%3,%4>,<%5,%6,%7>) %8").arg(frame).arg(pos.x).arg(pos.y).arg(pos.z).arg(rot.x).arg(rot.y).arg(rot.z).arg(pos.bodyPart));
}

void BVHNode::setKeyframePosition(int frame,const Position& pos)
{
//  qDebug(QString("setKeyframePosition(%1)").arg(frame));
  if(!isKeyframe(frame)) qDebug("setKeyframePosition(%d): not a keyframe!",frame);
  else
  {
    FrameData& key=keyframes[frame];
    key.setPosition(pos);
  }
}

void BVHNode::setKeyframeRotation(int frame,const Rotation& rot)
{
//  qDebug(QString("setKeyframeRotation(%1)").arg(frame));
  if(!isKeyframe(frame)) qDebug("setKeyframeRotation(%d): not a keyframe!",frame);
  else
  {
    FrameData& key=keyframes[frame];
    key.setRotation(rot);
  }
}

void BVHNode::deleteKeyframe(int frame)
{
  keyframes.remove(frame);
}

void BVHNode::insertFrame(int frame)
{
  QMap<int,FrameData>::iterator itCurrent;
  if(isKeyframe(frame))
    // if this actually is a keyframe, use this
    itCurrent=keyframes.find(frame);
  else
  {
    // find next keyframe number
    frame=getKeyframeNumberAfter(frame);
    // past the end? then do nothing
    if(frame==-1) return;
    // get current iterator
    itCurrent=keyframes.find(frame);
  }
  // step back one key
  itCurrent--;

  // start looking for keys at the end
  QMap<int,FrameData>::iterator itLook=keyframes.end();
  // step back to get the last keyframe
  itLook--;

  do
  {
    // get current keyframe's position
    int frame=(*itLook).frameNumber();
    // increment frame number in frame data
    (*itLook).setFrameNumber(frame+1);
    // decrement iterator here already, so it does not get invalidated by remove() later
    itLook--;
    // copy frame data into next frame
    keyframes[frame+1]=keyframes[frame];
    // remove old frame
    keyframes.remove(frame);
  } while(itLook!=itCurrent);
}

// delete a frame and move all keys back one frame
void BVHNode::deleteFrame(int frame)
{
//  qDebug("BVHNode::deleteFrame(%d)",frame);
  // if this is a keyframe, remove it
  if(isKeyframe(frame)) deleteKeyframe(frame);

  // find next keyframe number
  frame=getKeyframeNumberAfter(frame);
  // past the end? then do nothing
  if(frame==-1) return;
  // set up iterator with the following keyframe
  QMap<int,FrameData>::iterator itCurrent=keyframes.find(frame);

  // loop until we reach the end of the keyframe list
  while(itCurrent!=keyframes.end())
  {
    // get current keyframe's position
    int frame=(*itCurrent).frameNumber();
    // decrement frame number in frame data
    (*itCurrent).setFrameNumber(frame-1);
    // increment iterator here already, so it does not get invalidated by remove() later
    itCurrent++;
    // copy frame data into previous frame
    keyframes[frame-1]=keyframes[frame];
    // remove old frame
    keyframes.remove(frame);
  }
}

bool BVHNode::isKeyframe(int frame) const
{
  return keyframes.contains(frame);
}

double BVHNode::interpolate(double from,double to,int steps,int pos,bool easeOut,bool easeIn) const
{
  bool ease=false;

  // do not start any calculation if there's nothing to do
  if(from==to) return from;

  if(pos<=(steps/2) && easeOut) ease=true;
  if(pos>(steps/2) && easeIn) ease=true;

  // sine interpolation for ease in / out
  if(ease)
  {
    double distance=to-from;
    double step=3.1415/(steps);

    return from+(0.5-cos(step*(double) pos)/2)*distance;
  }
  // classic linear interpolation
  else
  {
//    double distance=to-from;
//    double increment=distance/(double) steps;
//    return from+increment*(double) pos;

	  return from;
  }
}

const FrameData BVHNode::frameData(int frame) const
{
  // return empty frame data on end site nodes
//  if(type==BVH_END) return FrameData();
  // if the keyframe exists, return the data
  if(isKeyframe(frame)) return keyframes[frame];

  // get keyframes before and after desired frame
  FrameData before=getKeyframeBefore(frame);
  FrameData after=getNextKeyframe(before.frameNumber());

  int frameBefore=before.frameNumber();
  int frameAfter=after.frameNumber();

  // if before and after frames are the same, there are no more keyframes left, so
  // we return the last keyframe data
  if(frameBefore==frameAfter) return before;

  Rotation rotBefore=before.rotation();
  Rotation rotAfter=after.rotation();
  Position posBefore=before.position();
  Position posAfter=after.position();

  Rotation iRot;
  Position iPos;

  iRot.x=interpolate(rotBefore.x,rotAfter.x,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());
  iRot.y=interpolate(rotBefore.y,rotAfter.y,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());
  iRot.z=interpolate(rotBefore.z,rotAfter.z,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());

  iPos.x=interpolate(posBefore.x,posAfter.x,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());
  iPos.y=interpolate(posBefore.y,posAfter.y,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());
  iPos.z=interpolate(posBefore.z,posAfter.z,frameAfter-frameBefore,frame-frameBefore,before.easeOut(),after.easeIn());

// qDebug(QString("iRot.x %1 frame %2: %3").arg(rotBefore.bodyPart).arg(before.frameNumber()).arg(iRot.x));

  // return interpolated frame data here
  return FrameData(frame,iPos,iRot);
}

const FrameData BVHNode::getKeyframeBefore(int frame) const
{
  if(frame==0)
  {
    // should never happen
    qDebug("BVHNode::getKeyframeBefore(int frame): frame==0!");
    return keyframes[0];
  }
  return frameData(getKeyframeNumberBefore(frame));
}

const FrameData BVHNode::getNextKeyframe(int frame) const
{
  QMap<int,FrameData>::const_iterator itCurrent=keyframes.find(frame);
  itCurrent++;
  // if we are asked for a keyframe past the last one, return the last one
  if(itCurrent==keyframes.end()) --itCurrent;
  return (*itCurrent);
}

int BVHNode::getKeyframeNumberBefore(int frame) const
{
  if(frame==0)
  {
    // should never happen
    qDebug("BVHNode::getKeyframeNumberBefore(int frame): frame==0!");
    return 0;
  }

  // find previous key
  while(--frame && !isKeyframe(frame)) {};

  return frame;
}

int BVHNode::getKeyframeNumberAfter(int frame) const
{
  // get a list of all keyframe numbers
  QList<int> keys=keyframeList();

  // past the end? return -1
  if(frame>(int) keys[keyframes.count()-1])
    return -1;

  // find next key
  while(++frame && !isKeyframe(frame)) {};

  return frame;
}

const FrameData BVHNode::keyframeDataByIndex(int index) const
{
  // get a list of all keyframe numbers
  QList<int> keys=keyframeList();
  // get frame number of keyframe at given index
  int number=keys[index];
  // return keyframe data
  return keyframes[number];
}

const QList<int> BVHNode::keyframeList() const
{
  return keyframes.keys();
}

int BVHNode::numKeyframes() const
{
  return keyframes.count();
}

const Rotation* BVHNode::getCachedRotation(int frame)
{
  return rotations.at(frame);
}

const Position* BVHNode::getCachedPosition(int frame)
{
  return positions.at(frame);
}

void BVHNode::cacheRotation(Rotation* rot)
{
  rotations.append(rot);
}

void BVHNode::cachePosition(Position* pos)
{
  positions.append(pos);
}

void BVHNode::flushFrameCache()
{
  // cal delete on all rotations and positions in the list
  qDeleteAll(rotations);
  qDeleteAll(positions);
  // remove all references to the now deleted list items
  rotations.clear();
  positions.clear();
}

void BVHNode::dumpKeyframes()
{
  QList<int> keys=keyframeList();
  for(unsigned int index=0;index< (unsigned int) keyframes.count();index++)
  {
    Rotation rot=frameData(keys[index]).rotation();
    Position pos=frameData(keys[index]).position();

    qDebug(QString("%1: %2 - Pos: <%3,%4,%5> Rot: <%6,%7,%8>").arg(name()).arg(keys[index]).arg(pos.x).arg(pos.y).arg(pos.z).arg(rot.x).arg(rot.y).arg(rot.z).toLatin1().constData());
  }
}

void BVHNode::setEaseIn(int frame,bool state)
{
  (*keyframes.find(frame)).setEaseIn(state);
}

void BVHNode::setEaseOut(int frame,bool state)
{
  (*keyframes.find(frame)).setEaseOut(state);
}

bool BVHNode::easeIn(int frame)
{
  if(keyframes.contains(frame))
    return (*keyframes.find(frame)).easeIn();

  qDebug("BVHNode::easeIn(): asked on non-keyframe!");
  return false;
}

bool BVHNode::easeOut(int frame)
{
  if(keyframes.contains(frame))
    return (*keyframes.find(frame)).easeOut();

  qDebug("BVHNode::easeOut(): asked on non-keyframe!");
  return false;
}

bool BVHNode::compareFrames(int key1,int key2) const
{
  if(type==BVH_POS)
  {
    const Position pos1=frameData(key1).position();
    const Position pos2=frameData(key2).position();

    if(pos1.x!=pos2.x) return false;
    if(pos1.y!=pos2.y) return false;
    if(pos1.z!=pos2.z) return false;
  }
  else
  {
    const Rotation rot1=frameData(key1).rotation();
    const Rotation rot2=frameData(key2).rotation();

    if(rot1.x!=rot2.x) return false;
    if(rot1.y!=rot2.y) return false;
    if(rot1.z!=rot2.z) return false;
  }

  return true;
}

void BVHNode::optimize()
{
  // PASS 1 - remove identical keyframes

  // get a list of all keyframe numbers
  QList<int> keys=keyframeList();
  QList<int> keysToDelete;

  // build a list of all identical keyframes to delete
  for(unsigned int i=1;i< (unsigned int) keys.count();i++)
  {
    // if we're comparing the last keyframe, it only makes sense to check for the one before
    if(i==(unsigned int) keys.count()-1)
    {
      if(compareFrames(keys[i],keys[i-1]))
        keysToDelete.append(keys[i]);
    }
    // otherwise check for the one before and the one after
    else if(compareFrames(keys[i],keys[i-1]) && compareFrames(keys[i],keys[i+1]))
      keysToDelete.append(keys[i]);
  }

  // delete keyframes on the delete list
  for(unsigned int i=0;i< (unsigned int) keysToDelete.count();i++)
    deleteKeyframe(keysToDelete[i]);

  // PASS 2 - remove keyframes that are superfluous due to linear interpolation

  Rotation oldRDifference;
  Position oldPDifference;

  // get first frame to compare - we even compare frame 1 here because we need
  // the initial "distance" and "difference" values. The first keyframe will
  // never be deleted, though
  QMap<int,FrameData>::const_iterator itBefore=keyframes.begin();

  if(itBefore==keyframes.end()) return;

  // make "current" frame one frame after "before" frame
  QMap<int,FrameData>::const_iterator itCurrent=itBefore;
  itCurrent++;

  if(itCurrent==keyframes.end()) return;

  // defines how much difference from anticipated change is acceptable for optimizing
  double tolerance=0.01;

  // loop as long as there are keyframes left
  while(itCurrent!=keyframes.end())
  {
    int distance=itCurrent.key()-itBefore.key();

    // optimize positions if this is the position node
    if(type==BVH_POS)
    {
      Position pDifference=Position::difference((*itBefore).position(),(*itCurrent).position());

      pDifference.x/=distance;
      pDifference.y/=distance;
      pDifference.z/=distance;

      if(fabs(pDifference.x-oldPDifference.x)<tolerance &&
         fabs(pDifference.y-oldPDifference.y)<tolerance &&
         fabs(pDifference.z-oldPDifference.z)<tolerance)
      {
        // never delete the key in the first frame
        if(itBefore.key()!=0) keyframes.remove(itBefore.key());
      }

      oldPDifference=pDifference;
    }
    // otherwise optimize rotations
    else
    {
      Rotation rDifference=Rotation::difference((*itBefore).rotation(),(*itCurrent).rotation());

      rDifference.x/=distance;
      rDifference.y/=distance;
      rDifference.z/=distance;

      if(fabs(rDifference.x-oldRDifference.x)<tolerance &&
        fabs(rDifference.y-oldRDifference.y)<tolerance &&
        fabs(rDifference.z-oldRDifference.z)<tolerance)
      {
          // never delete the key in the first frame
          if(itBefore.key()!=0) keyframes.remove(itBefore.key());
      }

      oldRDifference=rDifference;
    }

    itBefore=itCurrent;
    itCurrent++;
  } // while
}



// ************************************************************************

FrameData::FrameData()
{
//  qDebug(QString("FrameData(%1)").arg((unsigned long)this));
  m_frameNumber=0;
  m_easeIn=false;
  m_easeOut=false;
}

FrameData::FrameData(int num,Position pos,Rotation rot)
{
//  qDebug(QString("FrameData(%1): frame %2  pos %3,%4,%5 rot %6,%7,%8").arg((unsigned long) this).arg(frame).arg(pos.x).arg(pos.y).arg(pos.z).arg(rot.x).arg(rot.y).arg(rot.z));
  m_frameNumber=num;
  m_rotation=rot;
  m_position=pos;
  m_easeIn=false;
  m_easeOut=false;
}

int FrameData::frameNumber() const
{
  return m_frameNumber;
}

void FrameData::setFrameNumber(int frame)
{
  m_frameNumber=frame;
}

Position FrameData::position() const
{
  return m_position;
}

Rotation FrameData::rotation() const
{
  return m_rotation;
}

void FrameData::setEaseIn(bool state)
{
  m_easeIn=state;
}

void FrameData::setEaseOut(bool state)
{
  m_easeOut=state;
}

bool FrameData::easeIn() const
{
  return m_easeIn;
}

bool FrameData::easeOut() const
{
  return m_easeOut;
}

void FrameData::setPosition(const Position& pos)
{
  m_position=pos;
}

void FrameData::setRotation(const Rotation& rot)
{
//  qDebug(QString("FrameData::setRotation(<%1,%2,%3>)").arg(m_rotation.x).arg(m_rotation.y).arg(m_rotation.z));
//  qDebug(QString("FrameData::setRotation(<%1,%2,%3>)").arg(rot.x).arg(rot.y).arg(rot.z));
  m_rotation.x=rot.x;
  m_rotation.y=rot.y;
  m_rotation.z=rot.z;
//  qDebug(QString("FrameData::setRotation(<%1,%2,%3>)").arg(m_rotation.x).arg(m_rotation.y).arg(m_rotation.z));
//  m_rotation=rot;
}

// debugging
void FrameData::dump() const
{
  qDebug("FrameData::dump()");
  qDebug("Frame Number: %d",m_frameNumber);
  qDebug("Rotation: %lf, %lf, %lf",m_rotation.x,m_rotation.y,m_rotation.z);
  qDebug("Position: %lf, %lf, %lf",m_position.x,m_position.y,m_position.z);
  qDebug("Ease in/out: %d / %d",m_easeIn,m_easeOut);
}

FrameData::~FrameData()
{
//  qDebug(QString("~FrameData(%1)").arg((unsigned long) this));
}
