//
// App.xaml.h
// App 클래스의 선언입니다.
//

#pragma once

#include "App.g.h"
#include "SymbolInfo.h"
#include <collection.h>



//typedef UINT64 LONGINT;

namespace flowchart
{
	
	/// <summary>
	/// 기본 응용 프로그램 클래스를 보완하는 응용 프로그램별 동작을 제공합니다.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();
		static property UINT64 symbolIdCount;
		static property int selectedSymbolNumber;
		static property Platform::Collections::Vector<SymbolInfo^>^ symbolVector;
		static property UINT64 draggingSymbolNo;
		static property int draggingSymbolType;
		static property int focusedSymbolIndex;	// 선택된 symbolInfoVector의 index

	public:
		//SymbolNo로 App::SymbolVector에서 SymbolInfo를 반환하는 함수
		static SymbolInfo^ getSymbolInfoByNo(UINT64 symbolNo);
		//symbolInfo에서 4방위중 한 방향에 연결되있는 선 갯수 셈(fourDirection 1이 왼쪽부터 시계방향으로)
		static int getCountInOutLine(SymbolInfo^ symbolInfo, int fourDirection);
		//fromInfo에서 toInfo에 대한 방향을 알려주는 함수
		static int getDirectionTargetSymbol(SymbolInfo^ fromInfo, SymbolInfo^ toInfo);
		//심볼 사이에 다른 심볼이 가로막고 있는지 판별
		//2,4,5,7번 방향만 탐색, 나머지는 무조건 장애물이 있다고(true) 리턴
		static bool isBlockedBetweenSymbols(SymbolInfo^ startSymbol, SymbolInfo^ endSymbol);

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	};
}
