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

#ifndef BVH_H
#define BVH_H

#include <QtCore>
#include <QTextStream>

#include "rotation.h"
#include "bvhnode.h"
#include "animation.h"
///#include "camera.h"

#include <map>
using namespace std;

class BVHNode;
class Animation;

class BVH
{
  public:

    BVH();
    ~BVH();

	void setCurFrameIndex(int index){curFrameIndex = index;};
	void bvhWrite(const QString& file, int startFrame, int endFrame, bool flag3dMaxBVH);
	BVHNode* readSkeletonFromAvm(const QString& file);
    BVHNode* bvhRead(const QString& file);

    void assignChannels(BVHNode* node, FILE* f, int frame);
    void setChannelLimits(BVHNode* node,BVHChannelType type,double min,double max) const;
    void parseLimFile(BVHNode* root,const QString& limFile) const;

    void setNumFrames(int numFrames);
    int numFrames() const;
    void setAllKeyFrames(Animation* anim) const;
    void bvhIndent(QTextStream& out,int depth);
    void bvhWriteNode(BVHNode* node,QTextStream& out,int depth);
	void bvhWriteNode(BVHNode* node,QTextStream& out,int depth, int writeFlag, int flagNextNode);
    void bvhWriteFrame(BVHNode* node,QTextStream& out,int frame);
	void bvhWriteFrameNew(BVHNode* node,QTextStream& out,int frame);
	void bvhWriteFrame(BVHNode* node,QTextStream& out,int frame,int writeFlag);
	void bvhWriteFrameNew(BVHNode* node,QTextStream& out,int frame,int writeFlag);
    void bvhPrintNode(BVHNode* n, int depth);

    void bvhWrite(Animation* root,const QString& file);
    BVHNode* bvhFindNode(BVHNode* root,const QString& name) const;

    void bvhGetChannelLimits(BVHNode* node, BVHChannelType type, double *min, double *max);


    const QString bvhGetName(BVHNode* node,int index);
    int bvhGetIndex(BVHNode* node,const QString& name);
    void bvhCopyOffsets(BVHNode* dst,BVHNode* src);

    void bvhGetFrameData(BVHNode* node,int frame);
    void bvhSetFrameData(BVHNode* node,int frame);

    // lex neva's stuff:
    BVHNode* animRead(const QString& file, const QString& limFile);
    BVHNode* avmRead(const QString& file);

    void avmWrite(Animation* root,const QString& file);
    void animWrite(Animation* root,const QString& file);
    void bvhDelete(BVHNode* node);

    QStringList bvhTypeName;
    QStringList bvhChannelName;
    int lastLoadedNumberOfFrames;
    int lastLoadedLoopIn;
    int lastLoadedLoopOut;
    float lastLoadedAvatarScale;
    double lastLoadedFrameTime;
    Animation::FigureType lastLoadedFigureType;
    BVHNode* lastLoadedPositionNode;

    int nodeCount;

    QList<Rotation> rotationCopyBuffer;
    Position positionCopyBuffer;

  protected:
    QString inputFile;
    QStringList tokens;
    int tokenPos;

    // remember if the loaded animation is in old or new AVM format
    bool havePositionKeys;

    QStringList validNodes;
    BVHNode* positionNode;

    QString token();
    bool expect_token(const QString& expect);
    BVHNode* bvhReadNode();

    void assignChannels(BVHNode* node,int frame);

    void avmReadKeyFrame(BVHNode* root);
    void avmReadKeyFrameProperties(BVHNode* root);

    void avmWriteKeyFrame(BVHNode* root,QTextStream& out);
    void avmWriteKeyFrameProperties(BVHNode* root,QTextStream& out);

    // removes all unknown nodes from the animation
    void removeNoSLNodes(BVHNode* root);

    // debugging function, dumps the node structure
    void dumpNodes(BVHNode* node,QString indent);

    void setAllKeyFramesHelper(BVHNode* node,int numberOfFrames) const;
    const QString bvhGetNameHelper(BVHNode* node,int index);
    int bvhGetIndexHelper(BVHNode* node,const QString& name);

    void bvhGetFrameDataHelper(BVHNode* node,int frame);
    void bvhSetFrameDataHelper(BVHNode* node,int frame);

    int pasteIndex;
	int curFrameIndex;
};

#endif
