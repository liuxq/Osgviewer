#include "stdafx.h"
#include "commonData.h"



class COsgviewerView;
list<vector<sDataBufferEngine>> framedatas;
vector<CString> mapIndexToIpPort;

COsgviewerView* viewSet[4];
int realWindowNum;
int windowNum_;
map<QString,int> dataNameMap;

CCriticalSection g_cs;
CCriticalSection g_csForView;

int iWheel = 0;

#ifdef _DEBUG
ofstream g_debug("debug.txt");

#endif // _DEBUG


vector<sDataBufferEngine>* getCurFrame(int i)
{
	if (framedatas.empty() || i >= framedatas.size())
	{
		return NULL;
	}
	int k = 0;
	for (list<vector<sDataBufferEngine>>::iterator j = framedatas.begin(); j != framedatas.end(); j++, k++)
	{
		if (k == i)
		{
			return &(*j);
		}
	}
	return NULL;
}