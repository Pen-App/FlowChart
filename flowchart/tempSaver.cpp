#include "pch.h"
#include "TempSaver.h"
#include "App.xaml.h"

TempSaver::TempSaver()
{
	//0으로 시작
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

	//3. clonedVector도 넣어줌.
	symbolInfoVectorContainer->Append(clonedVector);
	
	//4. curVectorIndex 늘려주기
	
	curVectorIndex++;

	
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
	return symbolInfoVectorContainer->GetAt(curVectorIndex);
}
