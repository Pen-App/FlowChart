#pragma once
#include <collection.h>
#include <algorithm>
#include "SymbolInfo.h"
#include "App.xaml.h"


using namespace flowchart;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

#define MAX_HISTORY 5 //히스토리 저장 능력

typedef unsigned int SIZE_TYPE;
typedef IVector<SymbolInfo^>^ PTR_COPIED_APP_VECTOR;

ref class TempSaver sealed
{

public:
	TempSaver();

	void tempSave(PTR_COPIED_APP_VECTOR targetVector); //파라미터로 넘어온 벡터를 복제한다음 컨테이너에 넣는다. 
	
	SIZE_TYPE next(SIZE_TYPE targetIdx);
	SIZE_TYPE previous(SIZE_TYPE targetIdx);
	
	void unDo(); //뒤로
	void reDo(); //앞으로

	PTR_COPIED_APP_VECTOR getCurCopiedVector();
		
private:
	PTR_COPIED_APP_VECTOR symbolInfoVectorContainer[MAX_HISTORY + 1];
	SIZE_TYPE curIdx, startIdx, endIdx;
	void OnVectorChanged(Windows::Foundation::Collections::IObservableVector<SymbolInfo ^> ^sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^event);
};

