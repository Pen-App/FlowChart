#include "pch.h"
#include "TempSaver.h"
#include "App.xaml.h"

TempSaver::TempSaver()
{
	//1. 얘들이 0이란건 아직 데이터가 안들어왔다는 뜻.
	curIdx = 0;
	startIdx = 0;
	endIdx = 0;
}

void TempSaver::tempSave(PTR_COPIED_APP_VECTOR targetVector)
{
	//1. copiedAppVector 만들기 : App::Vector의 카피버젼 만들기. 
	Vector<SymbolInfo^>^ copiedAppVector = ref new Vector<SymbolInfo^>();

	for (SIZE_TYPE i = 0; i < targetVector->Size; i++)  
	{
		SymbolInfo^ symbolInfo = targetVector->GetAt(i);
		copiedAppVector->Append(ref new SymbolInfo(symbolInfo)); 
	}
	copiedAppVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<SymbolInfo ^>(this, &TempSaver::OnVectorChanged);

	//2. 처음 데이터를 넣는지 아닌지 판단하고 새로운 인덱스로 모두 맞춰주기, curIdx에 복사본을 넣는다. 
	if (curIdx == 0) //처음 데이터를 넣는 경우라면 모두 1로 맞춰준다. 
	{
		curIdx = 1;
		startIdx = 1;
		endIdx = 1;
	}
	else
	{
		if (next(curIdx) == startIdx) //더 이상 저장공간이 없다는 뜻. 갈아치운다.
		{
			startIdx = next(startIdx);
		}
		
		curIdx = next(curIdx);
		endIdx = curIdx;
	}
	
	symbolInfoVectorContainer[curIdx] = copiedAppVector;
	
}

SIZE_TYPE TempSaver::next(SIZE_TYPE targetIdx)
{
	if ((targetIdx / MAX_HISTORY) == 1) //끝에 왔다는 뜻
	{
		return 1; //여기서는 0이 처음이 아니다. 1이 처음임.
	}
	else
	{
		return targetIdx + 1;
	}
}

SIZE_TYPE TempSaver::previous(SIZE_TYPE targetIdx)
{
	if (targetIdx == 1) //끝에 왔다는 뜻
	{
		return MAX_HISTORY; // 인덱스 1이면 끝 인덱스가 이전 인덱스
	}
	else
	{
		return targetIdx - 1;
	}
}

void TempSaver::unDo()
{
	if (curIdx != startIdx)
		curIdx = previous(curIdx);
}

void TempSaver::reDo()
{
	if (curIdx != endIdx)
		curIdx = next(curIdx);
}

PTR_COPIED_APP_VECTOR TempSaver::getCurCopiedVector()
{
	return symbolInfoVectorContainer[curIdx];
}

void TempSaver::OnVectorChanged(Windows::Foundation::Collections::IObservableVector<SymbolInfo^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^ event)
{
	App::tempSaver->tempSave(App::symbolVector); //벡터에 변화가 감지되면 일단 App::symbolVector 를 복제저장해놓는다.
}



