#include "pch.h"
#include "HistoryObject.h"

HistoryObject::HistoryObject() {}

//히스토리를 초기화함
void HistoryObject::initHistory()
{
	history[0] = ref new Vector<SymbolInfo^>();

	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		SymbolInfo^ tempSymbolInfo = ref new SymbolInfo(App::symbolVector->GetAt(i));
		history[0]->Append(tempSymbolInfo);
	}

	for (int i = 1; i < 10; i++)
	{
		history[i] = nullptr;
	}
}

//히스토리 안에 바뀐 심볼벡터를 새로 넣어줌
void HistoryObject::putHistory()
{
	historyIndex++;
	if (historyIndex >= 10)
	{
		moveFullHistory();
	}

	Vector<SymbolInfo^>^ tempHistoryElement = ref new Vector<SymbolInfo^>();
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		tempHistoryElement->Append(ref new SymbolInfo(App::symbolVector->GetAt(i)));
	}
	history[historyIndex] = tempHistoryElement;

	//undo하고 새로 넣고 redo하는거 방지용
	for (int i = historyIndex + 1; i < 10; i++)
	{
		history[i] = nullptr;
	}
}

//언두
bool HistoryObject::unDo()
{
	//언두 할 수 없음을 리턴
	if (historyIndex <= 0)
	{
		App::undoOrRedoButtonClicked = false;
		return false;
	}

	historyIndex--;
	Vector<SymbolInfo^>^ unDoSymbolVector = ref new Vector<SymbolInfo^>();
	for (int i = 0; i < history[historyIndex]->Size; i++)
	{
		unDoSymbolVector->Append(ref new SymbolInfo(history[historyIndex]->GetAt(i)));
	}
	App::symbolVector = unDoSymbolVector;

	return true;
}

//리두
bool HistoryObject::reDo()
{
	//리두 할 수 없음을 리턴
	if (historyIndex + 1 >= 10 || history[historyIndex + 1] == nullptr)
	{
		App::undoOrRedoButtonClicked = false;
		return false;
	}

	historyIndex++;
	Vector<SymbolInfo^>^ reDoSymbolVector = ref new Vector <SymbolInfo^>();
	for (int i = 0; i < history[historyIndex]->Size; i++)
	{
		reDoSymbolVector->Append(ref new SymbolInfo(history[historyIndex]->GetAt(i)));
	}
	App::symbolVector = reDoSymbolVector;

	return true;
}

//꽉찬 히스토리에 자리 만들기
void HistoryObject::moveFullHistory()
{
	for (int i = 1; i < 10; i++)
	{
		history[i - 1] = history[i];
	}
	history[9] = nullptr;

	historyIndex = 9;
}