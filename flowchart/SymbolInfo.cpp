#include "pch.h"
#include "SymbolInfo.h"


SymbolInfo::SymbolInfo()
{
}

SymbolInfo::SymbolInfo(SymbolInfo^ symbolInfo)
{
	this->SymbolNo = symbolInfo->SymbolNo;
	this->RowIndex = symbolInfo->RowIndex;
	this->ColumnIndex = symbolInfo->ColumnIndex;
	this->SymbolType = symbolInfo->SymbolType;
	this->DecisionChecker = symbolInfo->DecisionChecker;
	this->Title = symbolInfo->Title;
	this->Content = symbolInfo->Content;
	this->Detail = symbolInfo->Detail;
	this->Path = symbolInfo->Path;
	this->Decision = symbolInfo->Decision;
}