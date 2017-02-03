#include "pch.h"
#include "SymbolInfo.h"


SymbolInfo::SymbolInfo()
{
}

SymbolInfo::SymbolInfo(SymbolInfo ^ symbolInfo)
{
	this->symbolNo = symbolInfo->symbolNo;
	this->rowIndex = symbolInfo->rowIndex;
	this->columnIndex = symbolInfo->columnIndex;
	this->symbolType = symbolInfo->symbolType;
	this->decisionChecker = symbolInfo->decisionChecker;
	this->title = symbolInfo->title;
	this->content = symbolInfo->content;
	this->detail = symbolInfo->detail;
	this->path = symbolInfo->path;
	this->decision = symbolInfo->decision;
}

SymbolInfo ^ SymbolInfo::cloneSymbolInfo()
{
	SymbolInfo^ tempSymbolInfo = ref new SymbolInfo();


	return tempSymbolInfo;
}
