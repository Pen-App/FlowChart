#pragma once
#include <iostream>
#include <list>

using namespace std;
using namespace Platform;

ref class SymbolInfo sealed
{
	//enum class SymbolType
	//{
	//	process,	// 처리
	//	decision,	// 판단
	//	preparation,	// 준비
	//	terminator,	// 단말
	//	data,	// 입출력
	//	document	// 인쇄
	//};
private:
	int symbolNo = 0;	// symbol의 고유번호
	int rowIndex, columnIndex;	// Grid 내에서 index
	//SymbolType symbolType;	// symbol의 종류
	// 1.process 2.decision 3.preparation 4.terminator 5.data 6. document
	int SymbolType = 0;	// symbol의 종류
	String^ title = nullptr;	// 제목
	String^ content = nullptr;	// 내용
	String^ detail = nullptr;	// 세부내용
	list<SymbolInfo^> path;
	//SymbolInfo^ path = nullptr;	// 이 symbol이 가리키는 symbol들
public:
	SymbolInfo();

	// GET & SET
	int GetSymbolNo();
	void SetSymbolNo(int symbolNo);
	int GetRowIndex();
	void SetRowIndex(int rowIndex);
	int GetColumnIndex();
	void SetColumnIndex(int columnIndex);
	int GetSymbolType();
	void SetSymbolType(int SymbolType);
	String^ GetTitle();
	void SetTitle(String^ title);
	String^ GetContent();
	void SetContent(String^ content);
	String^ GetDetail();
	void SetDetail(String^ detail);
	list<SymbolInfo^> GetPath();
	void SetPath(list<SymbolInfo^> path);

	//SymbolInfo^ GetPath();
	//void SetPath(SymbolInfo^ path);
};
