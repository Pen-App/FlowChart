#pragma once
#include <collection.h>
#include <algorithm>
#include "SymbolInfo.h"
#include "App.xaml.h"


using namespace flowchart;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;


typedef unsigned int SIZE_TYPE;
typedef IVector<SymbolInfo^>^ PTR_COPIED_APP_VECTOR;

ref class TempSaver sealed
{
public:
	TempSaver();

	SIZE_TYPE getSize(); // 현재 컨테이너에 벡터 복사본이 몇개 들어있는가?
	void tempSave(PTR_COPIED_APP_VECTOR targetVector); //파라미터로 넘어온 벡터를 복제한다음 컨테이너에 넣는다. 
	void unDo(); //뒤로
	void reDo(); //앞으로
	void refreshPeekerIndex(); //새로운 상태가 입력되면 PeekerIndex를 최신으로 맞춰준다. 

	PTR_COPIED_APP_VECTOR getLastSymbolInfoVector(); //컨테이너에서 벡터 복사본 꺼내기
	void containerRefresh(SIZE_TYPE maxHistoryCount); //벡터는 인덱스가 조정되데, size가 새롭게 만들어지지는 않는다. 따라서 vector를 새로만들고 원소들을 넣어줘야 size가 갱신됨.

private:
	int vectorPeekerIndex; //컨테이너를 왔다갔다 하며 원하는 심볼벡터를 꺼내주는 포인터
	Vector< PTR_COPIED_APP_VECTOR >^ symbolInfoVectorContainer; // = ref new Vector< IVector< SymbolInfo^ >^ >();
	
};

