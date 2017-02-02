#include "pch.h"
#include "TempSaver.h"
#include "App.xaml.h"

TempSaver::TempSaver()
{
	//-1로 시작
	curVectorIndex = -1;

}

SIZE_TYPE TempSaver::getSize()
{
	return symbolInfoVectorContainer->Size;
}

void TempSaver::tempSave()
{
	//1. 저장할 복제 vector만들기
	Vector<SymbolInfo^>^ clonedVector = ref new Vector<SymbolInfo^>();

	//2. symbolInfo 복사 후 넣어주고
	for (SIZE_TYPE i = 0; i < App::symbolVector->Size; i++)
	{
		SymbolInfo^ symbolInfo = App::symbolVector->GetAt(i);
		clonedVector->Append(symbolInfo);
	}

	if (curVectorIndex != symbolInfoVectorContainer->Size - 1)
	{
		symbolInfoVectorContainer->InsertAt(curVectorIndex-1,clonedVector);
		for (SIZE_TYPE i = 0; i < (symbolInfoVectorContainer->Size - curVectorIndex - 1); i++)
		{
			symbolInfoVectorContainer->RemoveAtEnd();
		}
		++curVectorIndex;
	
	}
	else
	{
		symbolInfoVectorContainer->Append(clonedVector);
		//. curVectorIndex 늘려주기
		if (symbolInfoVectorContainer->Size > 0)
			curVectorIndex = symbolInfoVectorContainer->Size - 1;
		
	}

	
	
	

	


	

}

void TempSaver::unDo()
{
	
	if (curVectorIndex > 0)
	{
		curVectorIndex--;
	}
	
}

void TempSaver::reDo()
{
	if (curVectorIndex < (symbolInfoVectorContainer->Size - 1))
	{
		curVectorIndex++;
	}


}

IVector<SymbolInfo^>^ TempSaver::getCurSymbolInfoVector()
{
	wchar_t debugstr[256];
	swprintf_s(debugstr, L"사이즈 : %d, curIndex: %d \n", symbolInfoVectorContainer->Size, curVectorIndex);
	OutputDebugString(debugstr);
	return symbolInfoVectorContainer->GetAt(curVectorIndex);
}