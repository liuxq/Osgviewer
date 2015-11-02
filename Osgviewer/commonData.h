#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <QtCore>

#include "DeviceBuffer.h"

using namespace std;

extern int iWheel;

extern int realWindowNum;
extern int windowNum_;

extern list<vector<sDataBufferEngine>> framedatas;
extern vector<CString> mapIndexToIpPort;
extern CCriticalSection g_cs;
extern CCriticalSection g_csForView;
//extern COutputWnd* gOutputWnd;
extern map<QString,int> dataNameMap;

//common function
vector<sDataBufferEngine>* getCurFrame(int i);


#ifdef _DEBUG
extern ofstream g_debug;

#endif // _DEBUG
