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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <QTimer>

#include "bvhnode.h"
#include "playstate.h"
#include "rotation.h"

#define DEFAULT_POSE "data/TPose.avm"
// #define DEFAULT_POSE "data/Relaxed.bvh"
#define LIMITS_FILE "data/SL.lim"

// playback resolution in milliseconds
// this is the speed of the internal sync timer, not of the animation itself
#define PLAYBACK_RESOLUTION     20.0

class BVH;

class Animation : public QObject
{
  //Q_OBJECT

  public:
    typedef enum
    {
      IK_LHAND=0,
      IK_RHAND,
      IK_LFOOT,
      IK_RFOOT,
      NUM_IK
    } IKPartType;

    typedef enum
    {
      FIGURE_MALE=0,
      FIGURE_FEMALE,
      NUM_FIGURES
    } FigureType;

    Animation(BVH* bvh,const QString& bvhFile=QString::null);
    ~Animation();
    BVHNode* getPositionNode();
    void loadBVH(const QString& bvhFile);
    void saveBVH(const QString& bvhFile);
    int getNumberOfFrames();
    void setNumberOfFrames(int num);
    int getFrame();
    void setFrame(int frameNumber);
    int stepForward();
    void setLoopInPoint(int inFrame);
    int getLoopInPoint();
    void setLoopOutPoint(int outFrame);
    int getLoopOutPoint();
    void setIK(BVHNode* node, bool flag);
    bool getIK(BVHNode* node);
    const QString getPartName(int index) const;
    int getPartIndex(BVHNode* node);
    void setMirrored(bool mirror);
    bool getMirrored();

    // get and set frames per second
    void setFPS(int fps);
    int fps() const;

    // convenience functions
    void setFrameTime(double frameTime);
    double frameTime() const;

    float getAvatarScale();
    void setAvatarScale(float newScale);

    FigureType getFigureType();
    void setFigureType(FigureType type);

    BVHNode* getMotion();
    BVHNode* getEndSite(const QString& siteParentName);
    BVHNode* getNode(int jointNumber);

    void cutFrame();
    void copyFrame();
    void pasteFrame();

    bool dirty() const;
    void setDirty(bool state);
    void setLoop(bool loop);

    void nextPlaystate();
    void setPlaystate(PlayState state);

    const FrameData keyframeDataByIndex(int jointNumber,int index);

    void setRotation(BVHNode* node,double w,double x,double y,double z);
    Rotation getRotation(BVHNode* node);
    void useRotationLimits(bool flag);
    bool isLimit(){ return limits;}
    RotationLimits getRotationLimits(BVHNode* node);
    void setPosition(double x,double y,double z);
    Position getPosition();
    int getRotationOrder(const QString& jointName);
    void addKeyFrameAllJoints();
    void addKeyFrame(BVHNode* joint);
    bool isKeyFrame();
    bool isKeyFrame(const QString& jointName);
    bool isKeyFrame(int jointNumber,int frame);
    void deleteKeyFrameAllJoints();
    void deleteKeyFrame(BVHNode* joint,int frame,bool silent=false); // silent = only send signal to timeline
    bool toggleKeyFrameAllJoints();
    bool toggleKeyFrame(BVHNode* node);

    void setEaseIn(BVHNode* node,int frameNum,bool state);
    void setEaseOut(BVHNode* node,int frameNum,bool state);
    bool easeIn(BVHNode* node,int frameNum);
    bool easeOut(BVHNode* node,int frameNum);

    int numKeyFrames(int jointNumber);
    void copyKeyFrame(int jointNumber,int from,int to);
    void moveKeyFrame(int jointNumber,int from,int to,bool copy=false);

    bool compareFrames(const BVHNode* joint,int key1,int key2) const;


    void optimize();

    enum { MAX_PARTS=64 };
    int frame;
  public slots:
    void deleteKeyFrame(int jointNumber,int frame);
    void insertFrame(int jointNumber,int frame);
    void deleteFrame(int jointNumber,int frame);

    // advances curentPlayTime and sends appropriate currentFrame signals
    void playbackTimeout();

  signals:
    /*void numberOfFrames(int num);
    void currentFrame(int frame);
    void frameChanged();
    void redrawTrack(int track);
    void animationDirty(bool state);*/

  protected:
    BVH* bvh;
    BVHNode* frames;
    BVHNode* positionNode;

    FigureType figureType;

    // this flag shows if the animation has been worked on and not yet saved
    bool isDirty;

    // display the avatar at another scale (1.0 is default)
    float avatarScale;


    int totalFrames;
    int framesPerSecond;

    // for playback
    double currentPlayTime;
    int playstate;

    bool loop;            // should we loop when using stepForward()?
    int loopInPoint;
    int loopOutPoint;

    bool mirrored;
    bool limits;



    void recursiveAddKeyFrame(BVHNode* joint);
    bool isKeyFrameHelper(BVHNode* joint);
    void recursiveDeleteKeyFrame(BVHNode* joint);
    void insertFrameHelper(BVHNode* joint,int frame);
    void deleteFrameHelper(BVHNode* joint,int frame);
    void optimizeHelper(BVHNode* joint);


    QString dataPath;
    QTimer timer;
};

#endif
