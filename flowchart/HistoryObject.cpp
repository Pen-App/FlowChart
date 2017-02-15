#include "pch.h"
#include "HistoryObject.h"

HistoryObject::HistoryObject() {}

//히스토리를 초기화함
void HistoryObject::initHistory()
{
	history[0] = safe_cast<Vector<SymbolInfo^>^>(copySymbolVector(App::symbolVector));

	for (int i = 1; i < 10; i++)
	{
		history[i] = nullptr;
	}
	historyIndex = 0;
}

//히스토리 안에 바뀐 심볼벡터를 새로 넣어줌
void HistoryObject::putHistory()
{
	historyIndex++;
	if (historyIndex >= 10)
	{
		moveFullHistory();
	}

	history[historyIndex] = safe_cast<Vector<SymbolInfo^>^>(copySymbolVector(App::symbolVector));

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
	App::symbolVector = safe_cast<Vector<SymbolInfo^>^>(copySymbolVector(history[historyIndex]));

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
	App::symbolVector = safe_cast<Vector<SymbolInfo^>^>(copySymbolVector(history[historyIndex]));

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

//SymbolInfo가 담긴 벡터를 오류없이 복사하기
IVector<SymbolInfo^>^ HistoryObject::copySymbolVector(IVector<SymbolInfo^>^ baseSymbolVector)
{
	Vector<SymbolInfo^>^ result = ref new Vector<SymbolInfo^>();
	//Path를 제외한 모든 baseSymbolVector의 정보를 복사
	for (int i = 0; i < baseSymbolVector->Size; i++)
	{
		SymbolInfo^ baseVectorElement = baseSymbolVector->GetAt(i);
		SymbolInfo^ resultElement = ref new SymbolInfo();

		resultElement->SymbolNo = baseVectorElement->SymbolNo;
		resultElement->RowIndex = baseVectorElement->RowIndex;
		resultElement->ColumnIndex = baseVectorElement->ColumnIndex;
		resultElement->SymbolType = baseVectorElement->SymbolType;
		resultElement->DecisionChecker = baseVectorElement->DecisionChecker;
		resultElement->Title = ref new String(baseVectorElement->Title->Data());
		resultElement->Content = ref new String(baseVectorElement->Content->Data());
		resultElement->Detail = ref new String(baseVectorElement->Detail->Data());
		for (int j = 0; j < baseVectorElement->Decision->Size; j++)
		{
			resultElement->Decision->Append(baseVectorElement->Decision->GetAt(j));
		}

		result->Append(resultElement);
	}
	//Path를 복사
	for (int i = 0; i < baseSymbolVector->Size; i++)
	{
		SymbolInfo^ baseVectorElement = baseSymbolVector->GetAt(i);
		SymbolInfo^ resultElement = result->GetAt(i);

		for (int j = 0; j < baseVectorElement->Path->Size; j++)
		{
			SymbolInfo^ baseVectorElementPathInfo = baseVectorElement->Path->GetAt(j);
			for (int k = 0; k < result->Size; k++)
			{
				SymbolInfo^ tempResultElement = result->GetAt(k);
				if (tempResultElement->SymbolNo == baseVectorElementPathInfo->SymbolNo)
				{
					resultElement->Path->Append(tempResultElement);
					break;
				}
			}
		}
	}

	return result;
}