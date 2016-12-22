//
// GridPage.xaml.h
// Declaration of the GridPage class
//

#pragma once

#include "GridPage.g.h"
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::UI;

namespace flowchart
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GridPage sealed
	{
	public:
		GridPage();

		

	private:
		//그리드의 행,열 인덱스
		int curRowIndex; 
		int curColumnIndex;

		int rowHeight, columnWidth;    //가로줄의 높이, 세로줄의 너비
		int nowRowNum, nowColumnNum;  //현재 가로줄 수, 현재 세로줄 수

		//0. gridArray 만들기
		void makeGridArray(Grid^ parentGrid, int rowNum, int columnNum, int rowHeight, int columnWidth);

		//1. 배경 rectangle 만들기
		void makeRectangle(Grid^ parentGrid, int rowIndex, int columnIndex);
		
		//2. symbol이미지 그려주기 
		void makeImage(Grid^ parentGrid, int symbolType, int rowIndex, int columnIndex);

		//3. 기능 버튼들 만들어주기
		void makeButton(Grid^ parentGrid, int buttonType, int rowIndex, int columnIndex);
		void makeButtons(Grid^ parentGrid, int rowIndex, int columnIndex);

		//4. 화면 전체를 다시 그려주는 함수
		void refreshGridPage(Grid^ parentGrid);

		//5. grid늘리기
		void appendRow();
		void appendColumn();
		void appendTopRow();
		void appendLeftColumn();

		void PageGrid_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void Rectangle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void PageGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
	};
}
