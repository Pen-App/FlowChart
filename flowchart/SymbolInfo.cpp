#include "pch.h"
#include "SymbolInfo.h"


SymbolInfo::SymbolInfo()
{
}

// GET & SET
int SymbolInfo::GetSymbolNo()
{
	return this->symbolNo;
}
void SymbolInfo::SetSymbolNo(int symbolNo)
{
	this->symbolNo = symbolNo;
}
int SymbolInfo::GetRowIndex()
{
	return this->rowIndex;
}
void SymbolInfo::SetRowIndex(int rowIndex)
{
	this->rowIndex = rowIndex;
}
int SymbolInfo::GetColumnIndex()
{
	return this->columnIndex;
}
void SymbolInfo::SetColumnIndex(int columnIndex)
{
	this->columnIndex = columnIndex;
}
int SymbolInfo::GetSymbolType()
{
	return this->SymbolType;
}
void SymbolInfo::SetSymbolType(int SymbolType)
{
	this->SymbolType = SymbolType;
}
String^ SymbolInfo::GetTitle()
{
	return this->title;
}
void SymbolInfo::SetTitle(String^ title)
{
	this->title = title;
}
String^ SymbolInfo::GetContent()
{
	return this->content;
}
void SymbolInfo::SetContent(String^ content)
{
	this->content = content;
}
String^ SymbolInfo::GetDetail()
{
	return this->detail;
}
void SymbolInfo::SetDetail(String^ detail)
{
	this->detail = detail;
}
list<SymbolInfo^> SymbolInfo::GetPath()
{
	return this->path;
}
void SymbolInfo::SetPath(list<SymbolInfo^> path)
{
	this->path = path;
}