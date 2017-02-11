#pragma once
#include <collection.h>
#include <algorithm>
#include "SymbolInfo.h"
#include "App.xaml.h"

using namespace flowchart;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

ref class HistoryObject sealed
{
public:
	HistoryObject();

	//히스토리를 초기화함
	void initHistory();
	//히스토리 안에 바뀐 심볼벡터를 새로 넣어줌
	void putHistory();
	//언두
	bool unDo();
	//리두
	bool reDo();
	//꽉찬 히스토리에 자리 만들기
	void moveFullHistory();

private:
	Vector<SymbolInfo^>^ *history = new Vector<SymbolInfo^>^[10];
	unsigned int historyIndex = 0;

public:
	/*property unsigned int HistoryIndex
	{
		unsigned int get() { return historyIndex; }
		void set(unsigned int value) { historyIndex = value; }
	}
	property Vector<SymbolInfo^>^* History
	{
		Vector<SymbolInfo^>^* get() { return history; }
	}*/
};