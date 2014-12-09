#ifndef C3D_H
#define C3D_H


#include <QtCore>
#include <map>
using namespace std;

class C3D
{
public:

	C3D();
	~C3D();

	void saveC3DFile(const char* sBVHFilePath,int,int);
	void setCurFrameIndex(int index){curFrameIndex = index;};
private:
	map<QString,int> dataNameMap;
	int curFrameIndex;

};

#endif