#pragma once
#include <collection.h>
#include <algorithm>
#include "SymbolInfo.h"
typedef unsigned int SIZE_TYPE;


using namespace flowchart;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

ref class TempSaver sealed
{
public:
	TempSaver();
	SIZE_TYPE getSize();
	void tempSave();
	void unDo(); //뒤로
	void reDo(); //앞으로
	IVector<SymbolInfo^>^ getCurSymbolInfoVector();

	int getNowRowNum();
	int getNowColumnNum();
	void setNowRowNum(int value);
	void setNowColumnNum(int value);

private:
	Vector< IVector< SymbolInfo^ >^ >^ symbolInfoVectorContainer = ref new Vector< IVector< SymbolInfo^ >^ >();
	int curVectorIndex;
	int nowRowNum, nowColumnNum;
	

};