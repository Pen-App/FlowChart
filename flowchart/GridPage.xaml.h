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

		int curRowIndex; //그리드의 행,열 인덱스
		int curColumnIndex;

		int rowHeight, columnWidth;    //가로줄의 높이, 세로줄의 너비
		int nowRowNum, nowColumnNum;  //현재 가로줄 수, 현재 세로줄 수

		bool isSymbolIn; //커서가 있는 이미지안에 symbol이 존재하는가?
		bool isSymbolRectIn; //symbol이 존재하는 rectangle안에 커서가 있는가?
		UINT64 focusedSymbolNo; //포커스된 symbolNo
		int focusedSymbolType;

		bool isLineDrawing; //연결선을 그리고 있는지
		UINT64 connectorStartSymbolNo; //연결선
		double mouseXPos, mouseYPos;

		bool isSelectingYesOrNo; //yes or no를 선택하고 있는지

		String^ tappedDeletorName; //탭 된 deletor 이름을 임시로 갖고 있음

		//=============================함수들=====================================
		//0. gridArray 만들기
		void makeGridArray(Grid^ parentGrid, int rowNum, int columnNum, int rowHeight, int columnWidth);

		//1. 배경 rectangle 만들기
		void makeRectangle(Grid^ parentGrid, int rowIndex, int columnIndex);

		//2. symbol이미지 그려주기 
		void makeImage(Grid^ parentGrid, UINT64 symbolNo, int symbolType, int rowIndex, int columnIndex);

		//3. 기능 버튼들 만들어주기
		void makeButton(Grid^ parentGrid, UINT64 symbolNo, int buttonType, int rowIndex, int columnIndex);
		void makeButtons(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex);

		//TITLE_TEXTBLOCK, CONTENT_TEXTBLOCK 만들기
		void makeTitleTextBlock(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex);
		void makeContentTextBlock(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex);
		void makeTextBlocks(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex);

		//5. grid늘리기
		void appendRow();
		void appendColumn();
		void appendTopRow();
		void appendLeftColumn();

		void PageGrid_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

		void Rectangle_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Rectangle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

		//6. 버튼을 클릭했을때 flyout 처리부분
		void TitleButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DetailButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ContentButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		//7. 그리드 안에 Symbol이 존재하는지 체크 
		void Image_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Image_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Image_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Image_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
		void Image_DropComplete(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args);
		void Image_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void Image_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

		//. focused 된 버튼만 보여주게 만드는 함수, 모든 버튼을 숨기는 함수
		void showFocusedSymbolButtons(UINT64 focusedSymbolNo);
		void hideAllButtons();

		//스크롤뷰어 이벤트 처리(줌, PageGrid 늘리기)
		void PageGridScrollViewer_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void PageGridScrollViewer_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

		//. symbol 이동시켜주는 함수
		void moveSymbolInfoInSymbolVector(UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex);
		void moveFocusedSymbol(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex);
		void moveSymbolRectangle(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex);
		void moveTextBlocks(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex);
		void moveYesOrNoTextBlock(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex);

		// 선그리는 버튼 드래그 해서 선 그리는 함수
		void ConnectorButtonPress(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void PageGridCanvas_PointerPress(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void PageGridCanvas_PointerMove(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

		// symbol 이미지 밑에 깔아서 함께 움직이는 rectangle 만들기 
		void makeSymbolRectangle(Grid^ parentGrid, UINT64 symbolNo, int symbolType, int rowIndex, int columnIndex);
		void Rectangle_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Rectangle_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void Button_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Button_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Button_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void Button_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

		//스크롤뷰어가 줌 인,아웃이 됬을 때 발생하는 이벤트
		void PageGridScrollViewer_ViewChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ e);

		//연결선 만들어주는 함수
		void makeConnectLine(UINT16 from, UINT16 to);

		//연결선과 그에 관련된 것(델레터, YesOrNo, 방향표시)을 이동하는 함수
		void moveConnectLine(UINT16 movedSymbolNo);

		//파일오픈용 연결선 함수
		void LoadingConnectLine(SymbolInfo^ fromInfo);

		//연결 대상의 방향을 알려주는 함수
		int getDirectionTartgetSymbol(SymbolInfo^ fromInfo, SymbolInfo^ toInfo);

		//간이 연결선 삭제하는 함수
		void deleteTempConnectLine();

		// Flyout에서 글을 작성할 때 실시간으로 내용을 받아오는 이벤트
		void DetailText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args);
		void TitleText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args);
		void ContentText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args);

		// 파일 열기로 연 파일을 그려주는 메소드
		void LoadOpenedFile();

		//Yes Or No를 선택하는 플라이아웃 버튼을 클릭했을 때 처리
		void YesOrNoFlyoutButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		//Yes Or No TextBlock을 만드는 함수
		void makeYesOrNoTextBlock(UINT16 from, UINT16 to, bool decision);

		//파일 열기시 symbol을 새로 그려줄때 쓰는 메솓
		void makeSymbol(SymbolInfo^ symbolInfo);

		//연결선 삭제 확인 버튼을 눌렀을 때
		void LineDeleteConfirmButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		//파일오픈용 PageGrid 늘려주는 함수
		void LoadingPageGridSize();

		//델레터를 클릭했을 때
		void LineDeletor_Tapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e);

		//연결선, 델레터, YesOrNo, 방향표시 삭제해주는 함수
		void deleteLine(UINT64 from, UINT64 to);

		//SymbolInfo에서 path와 decision 삭제해주는 함수
		void deletePathAndDecision(UINT64 from, UINT64 to);
};
}