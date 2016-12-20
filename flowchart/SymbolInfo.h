#pragma once

using namespace Platform;

ref class SymbolInfo sealed
{
private:
	int symbolNo = 0;	// symbol의 고유번호
	int rowIndex, columnIndex;	// Grid 내에서 index
	SymbolType symbolType;	// symbol의 종류
	String^ title = nullptr;	// 제목
	String^ content = nullptr;	// 내용
	String^ detail = nullptr;	// 세부내용
	SymbolInfo^ path = nullptr;	// 이 symbol이 가리키는 symbol들
public:
	SymbolInfo();
};

enum class SymbolType
{
	process,	// 처리
	decision,	// 판당
	preparation,	// 준비
	terminator,	// 단말
	data,	// 입출력
	document	// 인쇄
};

