#include "stdafx.h"
#include "commonData.h"



class COsgviewerView;
list<vector<sDataBufferEngine>> framedatas;

COsgviewerView* viewSet[4];
int realWindowNum;
int windowNum_;
map<QString,int> dataNameMap;

CCriticalSection g_cs;
CCriticalSection g_csForView;

int iWheel = 0;

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