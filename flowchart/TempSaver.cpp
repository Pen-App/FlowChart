#include "pch.h"
#include "TempSaver.h"
#include "App.xaml.h"

TempSaver::TempSaver()
{
	vectorPeekerIndex = -1; //1. vectorPeekerIndex는 -1로
	symbolInfoVectorContainer = ref new Vector< IVector< SymbolInfo^ >^ >();
}

SIZE_TYPE TempSaver::getSize()
{
	return symbolInfoVectorContainer->Size; //현재 컨테이너에 몇개나 담겨져 있나?
}

void TempSaver::tempSave(PTR_COPIED_APP_VECTOR targetVector)
{
	PTR_COPIED_APP_VECTOR copiedAppVector = ref new Vector<SymbolInfo^>(); //1. 새로운 앱벡터 만들어주기
																		 
	for (SIZE_TYPE i = 0; i < targetVector->Size; i++)  //2. symbolInfo 복사 후 넣어주고
	{
		SymbolInfo^ symbolInfo = targetVector->GetAt(i);
		copiedAppVector->Append(ref new SymbolInfo(symbolInfo)); 
	}

	//이제 App::Vector의 카피버젼을 가지게 되었다. 
	symbolInfoVectorContainer->Append(copiedAppVector); //컨테이너에 추가시킨다.

	//peeker인덱스 증가로직
	if (vectorPeekerIndex == symbolInfoVectorContainer->Size - 2)
	{
		vectorPeekerIndex;
	}
}

void TempSaver::unDo()
{
	if (vectorPeekerIndex > 0)
	{
		//0. container안에서 피커 인덱스 이전 인덱스 가리키는 원소를 tempSaving한다. 
		tempSave(symbolInfoVectorContainer->GetAt(--vectorPeekerIndex));
	}
}

void TempSaver::reDo()
{
	if (vectorPeekerIndex != symbolInfoVectorContainer->Size-1)
	{
		//0. container안에서 피커 인덱스 이후 인덱스 가리키는 원소를 tempSaving한다. 
		tempSave(symbolInfoVectorContainer->GetAt(++vectorPeekerIndex));
	}
}

void TempSaver::refreshPeekerIndex()
{
	vectorPeekerIndex = symbolInfoVectorContainer->Size - 1;
}

PTR_COPIED_APP_VECTOR TempSaver::getLastSymbolInfoVector()
{
	return symbolInfoVectorContainer->GetAt(symbolInfoVectorContainer->Size-1);
}

