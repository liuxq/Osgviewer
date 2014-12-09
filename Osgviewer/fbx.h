#ifndef FBX_H
#define FBX_H


#include <QtCore>
#include <map>
using namespace std;

class FBX
{
public:

	FBX();
	~FBX();
	void fbxWrite(const QString& file, int startFrame, int endFrame);
	void setCurFrameIndex(int index){curFrameIndex = index;};
private:
	map<QString,int> dataNameMap;
	int curFrameIndex;
};

#endif