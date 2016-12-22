//
// GridPage.xaml.cpp
// Implementation of the GridPage class
//

#include "pch.h"
#include "GridPage.xaml.h"
#include <string>
using namespace flowchart;

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
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

//GridPage 생성자 : 생성과 동시에 makeGridArray로 행열을 그려준다. 
GridPage::GridPage()
{
	InitializeComponent();
	isSymbolIn = false;

	nowColumnNum = 10;
	nowRowNum = 10;
	columnWidth = 100;
	rowHeight = 70;
	makeGridArray(PageGrid, nowRowNum, nowColumnNum, rowHeight, columnWidth);
}

//행,열 하나하나 마다 rectangle을 채워넣는 함수 : 마우스 커서가 어떤 행,열 인덱스위에 올라가있는지 이벤트를 받기 위해서
void GridPage::makeRectangle(Grid^ parentGrid, int rowIndex, int columnIndex)
{
	Rectangle^ tempRect = ref new Rectangle();
	tempRect->Style = RECTANGLE_STYLE;
	auto dasharray = ref new DoubleCollection;
	dasharray->Append(5);
	tempRect->StrokeDashArray = dasharray;
	tempRect->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempRect->SetValue(parentGrid->RowProperty, rowIndex);
	tempRect->AllowDrop = true;

	tempRect->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerEntered);
	tempRect->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerPressed);
	tempRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragEnter);
	parentGrid->Children->Append(tempRect);
}

//행열을 그려주는 함수: parentGrid에 rowNum, columnNum만큼 행열을 만들어주고, makeRectangle로 한개씩 사각형을 채워넣는다.
void GridPage::makeGridArray(Grid^ parentGrid, int rowNum, int columnNum, int rowHeight, int columnWidth)
{
	//1. 행을 그린다. 
	for (int i = 0; i < rowNum; i++) {
		RowDefinition^ tempRow = ref new RowDefinition();
		tempRow->Height = rowHeight;
		parentGrid->RowDefinitions->Append(tempRow);
	}

	//2. 열을 그린다. 
	for (int i = 0; i < columnNum; i++) {
		ColumnDefinition^ tempColumn = ref new ColumnDefinition();
		tempColumn->Width = columnWidth;
		parentGrid->ColumnDefinitions->Append(tempColumn);
	}

	//3. rectangle을 채워넣어본다. 
	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < columnNum; j++) {
			//1. rectangle 채우기 
			makeRectangle(parentGrid, i, j);
		}
	}
}

//이미지 그려주기 : symbolType(1~6)에 따라서 해당 행,열에 symbol모양을 그려준다. 
void GridPage::makeImage(Grid^ parentGrid, UINT64 symbolNo, int symbolType, int rowIndex, int columnIndex)
{
	if (symbolType == -1)
		return;

	Image^ tempImage = ref new Image();
	tempImage->SetValue(parentGrid->RowProperty, curRowIndex);
	tempImage->SetValue(parentGrid->ColumnProperty, curColumnIndex);
	tempImage->CanDrag = true;
	tempImage->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerEntered);
	tempImage->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerExited);
	tempImage->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerPressed);
	String^ tempName = "i";

	//type이 1이고 symbolNo가 77인 이미지의 경우 "i1 77" 이라는 name이 부여된다. 
	switch (symbolType) {
	case 1:
		tempImage->Style = IMAGE_PROCESS;
		tempName += "1 ";
		break;
	case 2:
		tempImage->Style = IMAGE_DECISION;
		tempName += "2 ";
		break;
	case 3:
		tempImage->Style = IMAGE_PREPARATION;
		tempName += "3 ";
		break;
	case 4:
		tempImage->Style = IMAGE_TERMINATOR;
		tempName += "4 ";
		break;
	case 5:
		tempImage->Style = IMAGE_DATA;
		tempName += "5 ";
		break;
	case 6:
		tempImage->Style = IMAGE_DOCUMENT;
		tempName += "6 ";
		break;
	}
	tempName += symbolNo;
	tempImage->Name = tempName;
	parentGrid->Children->Append(tempImage);
	//parentGrid->UpdateLayout();
}

//버튼 1개 만들기 : buttonType 파라미터에 따라서 세가지 종류의 버튼을 만들수 있다. 
void GridPage::makeButton(Grid^ parentGrid, UINT64 symbolNo, int buttonType, int rowIndex, int columnIndex)
{
	int horizontal, vertical;

	Button^ tempButton = ref new Button();
	tempButton->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempButton->SetValue(parentGrid->RowProperty, rowIndex);
	tempButton->SetValue(this->VisibilityProperty, 1); //0: visible, 1:collapsed : 그냥 놓았을 때는 버튼이 숨겨진다. 

	switch (buttonType) {
	case 1:
		tempButton->Name = "b1 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_CONNECTOR;
		horizontal = 2;
		vertical = 2;
		break;
	case 2:
		tempButton->Name = "b2 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_CONTENTS;
		horizontal = 0;
		vertical = 2;
		break;

	case 3:
		tempButton->Name = "b3 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_DETAIL;
		horizontal = 2;
		vertical = 0;
		break;
	}
	tempButton->SetValue(HorizontalAlignmentProperty, horizontal);
	tempButton->SetValue(VerticalAlignmentProperty, vertical);
	parentGrid->Children->Append(tempButton);
}
//버튼 3종류 한꺼번에 만들기
void GridPage::makeButtons(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex)
{
	makeButton(parentGrid, symbolNo, 1, rowIndex, columnIndex);
	makeButton(parentGrid, symbolNo, 2, rowIndex, columnIndex);
	makeButton(parentGrid, symbolNo, 3, rowIndex, columnIndex);

}


//grid를 전체 새로 그려주는 함수 : 작동 이상으로 쓰이지 않음.-----------------
void GridPage::refreshGridPage(Grid^ parentGrid)
{
	//parentGrid->Children->Clear();
	for (int i = 0; i < parentGrid->Children->Size; i++) {
		if (parentGrid->Children->GetAt(i)->GetType() == Image::typeid) {
			parentGrid->Children->RemoveAt(i);
			parentGrid->UpdateLayout();
		}
	}

	//parentGrid->UpdateLayout();
	//makeGridArray(PageGrid, 10, 10, 70, 100);

	//1. map 순회
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		//makeImage(parentGrid, App::symbolVector->GetAt(i)->SymbolType, App::symbolVector->GetAt(i)->RowIndex, App::symbolVector->GetAt(i)->ColumnIndex);
		//makeButtons(parentGrid, App::symbolVector->GetAt(i)->RowIndex, App::symbolVector->GetAt(i)->ColumnIndex);
	}

}

void GridPage::appendRow()
{
	//행을 한줄 그려줌
	RowDefinition^ tempRowDef = ref new RowDefinition;
	tempRowDef->Height = rowHeight;
	PageGrid->RowDefinitions->Append(tempRowDef);
	nowRowNum++;

	//그려준 행에 Rectangle(경계선)을 그림
	for (int i = 0; i < nowColumnNum; i++)
	{
		makeRectangle(PageGrid, nowRowNum - 1, i);
	}
}

void GridPage::appendColumn()
{
	//열을 한줄 그려줌
	ColumnDefinition^ tempColDef = ref new ColumnDefinition;
	tempColDef->Width = columnWidth;
	PageGrid->ColumnDefinitions->Append(tempColDef);
	nowColumnNum++;

	//그려준 열에 Rectangle(경계선)을 그림
	for (int i = 0; i < nowRowNum; i++)
	{
		makeRectangle(PageGrid, i, nowColumnNum - 1);
	}
}

void GridPage::appendTopRow()
{
	//행 한줄 추가
	appendRow();

	//모든 심볼 한 행씩 아래로 옴김(위쪽에 행이 한줄 추가되는 효과를 얻기 위해)
	wchar_t elementType[256];
	for (int i = 0; i < PageGrid->Children->Size; i++)
	{
		UIElement^ symbolInPageGrid = PageGrid->Children->GetAt(i);

		swprintf_s(elementType, L"%s", symbolInPageGrid->ToString()->Data());

		if (wcscmp(elementType, L"Windows.UI.Xaml.Shapes.Rectangle")) //심볼일 때
		{
			Object^ rowPropertyValueObject =
				symbolInPageGrid->GetValue(Grid::RowProperty);
			int rowPropertyValueInt = safe_cast<int>(rowPropertyValueObject);
			symbolInPageGrid->SetValue(Grid::RowProperty, rowPropertyValueInt + 1);
		}
	}
}

void GridPage::appendLeftColumn()
{
	//열 한줄 추가
	appendColumn();

	//모든 심볼 한 열씩 오른쪽으로 옴김(왼쪽에 열이 한줄 추가되는 효과를 얻기 위해)
	wchar_t elementType[256];
	for (int i = 0; i < PageGrid->Children->Size; i++)
	{
		UIElement^ symbolInPageGrid = PageGrid->Children->GetAt(i);

		swprintf_s(elementType, L"%s", symbolInPageGrid->ToString()->Data());

		if (wcscmp(elementType, L"Windows.UI.Xaml.Shapes.Rectangle")) //심볼일 때
		{
			Object^ columnPropertyValueObject =
				symbolInPageGrid->GetValue(Grid::ColumnProperty);
			int columnPropertyValueInt = safe_cast<int>(columnPropertyValueObject);
			symbolInPageGrid->SetValue(Grid::ColumnProperty, columnPropertyValueInt + 1);
		}
	}
}


//이벤트 함수들 

//이미지가 드래그된 상태로 pageGrid위로 올라가면 커서에 무엇을 보여줄 것인가?
void flowchart::GridPage::PageGrid_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	e->AcceptedOperation = DataPackageOperation::Move;
	e->DragUIOverride->Caption = "추가";
	e->DragUIOverride->IsGlyphVisible = true;
}

//드래그된 상태에서 마우스버튼을 놓았을때, 즉 drop했을 때 어떤 일이 일어나는가?
void flowchart::GridPage::PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	//1. map에 새로운 symbolInfo 객체를 추가시킨다. 
	UINT64 tempSymbolNo = App::symbolIdCount;
	App::symbolIdCount++;
	SymbolInfo^ tempSymbolInfo = ref new SymbolInfo();
	tempSymbolInfo->SymbolType = App::selectedSymbolNumber;
	tempSymbolInfo->RowIndex = curRowIndex;
	tempSymbolInfo->ColumnIndex = curColumnIndex;
	tempSymbolInfo->SymbolNo = tempSymbolNo;
	App::symbolVector->Append(tempSymbolInfo);

	//심볼 놓는 위치에 따라 PageGrid를 늘려줌
	if (curColumnIndex == 0)
	{
		appendLeftColumn();
	}
	if (curRowIndex == 0)
	{
		appendTopRow();
	}
	if (curColumnIndex + 1 == nowColumnNum)
	{
		appendColumn();
	}
	if (curRowIndex + 1 == nowRowNum)
	{
		appendRow();
	}


	//testTextBox->Text = "" + App::symbolVector->Size;
	makeImage(PageGrid, tempSymbolNo, App::selectedSymbolNumber, curRowIndex, curColumnIndex);
	makeButtons(PageGrid, tempSymbolNo, curRowIndex, curColumnIndex);
	PageGrid->UpdateLayout();
	//refreshGridPage(PageGrid);
}

//마우스가 rectangle위로 올라가있으면 rectangle이 자신이 속한 행,열 인덱스를 curRowIndex와 curColumnIndex에 기록한다. 
void flowchart::GridPage::Rectangle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);


}

//마우스가 드래그된 상태에서 rectangle위로 올라가도 행,열 인덱스가 자동으로 curRowIndex와 curColumnIndex에 기록된다. 
void flowchart::GridPage::Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);
}

//symbol이 없는 곳에서 마우스 클릭하면 모든 버튼이 숨겨진다. 
void flowchart::GridPage::Rectangle_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (!isSymbolIn) {
		hideAllButtons();
	}
}

//그리드 사이즈 변환 함수
void flowchart::GridPage::PageGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	//커진 크기 측정
	int resizedPageGridHeight = (int)(PageGrid->ActualHeight);
	int resizedPageGridWidth = (int)(PageGrid->ActualWidth);

	//커진 크기에 따른 행,열의 갯수 측정
	int resizedRowNum = (resizedPageGridHeight / rowHeight)
		+
		((resizedPageGridHeight%rowHeight) ? 1 : 0);
	int resizedColumnNum = (resizedPageGridWidth / columnWidth)
		+
		((resizedPageGridWidth%columnWidth) ? 1 : 0);

	//커진 크기만큼 행,열 늘림
	while (resizedRowNum > nowRowNum)
	{
		appendRow();
	}
	while (resizedColumnNum > nowColumnNum)
	{
		appendColumn();
	}
}



//이미지 안에 커서가 들어오면 isSymbolIn이 true가 되고 나가면 false가 된다. 
void flowchart::GridPage::Image_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = true;
	testTextBox->Text = "" + isSymbolIn;
}

void flowchart::GridPage::Image_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = false;
	testTextBox->Text = "" + isSymbolIn;
}


//이미지 안에서 클릭을 했을 경우 -> focus 시키기, 버튼보이게 하기
void flowchart::GridPage::Image_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isSymbolIn)
	{
		//1. 이미지의 symbolNo를 알아낸다. 
		auto tempImageName = ((Image^)sender)->Name;
		focusedSymbolNo = std::stoi(tempImageName->Data() + 3);

		//2. 포커스된 symbol의 버튼만 보이게 만든다. 
		showFocusedSymbolButtons(focusedSymbolNo);

	}
	
}

//파라미터로 넘겨진 id의 symbol 버튼만 보이게 하는 함수
void flowchart::GridPage::showFocusedSymbolButtons(UINT64 focusedSymbolNo)
{
	UINT64 tempSymbolNo;
	Button^ tempButton1;
	Button^ tempButton2;
	Button^ tempButton3;

	for (UINT64 i = 0; i < App::symbolVector->Size; i++) {
		tempSymbolNo = App::symbolVector->GetAt(i)->SymbolNo;
		tempButton1 = nullptr;
		tempButton2 = nullptr;
		tempButton3 = nullptr;

		tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + tempSymbolNo));
		tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + tempSymbolNo));
		tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + tempSymbolNo));

		if (tempSymbolNo == focusedSymbolNo) {
			tempButton1->SetValue(VisibilityProperty, 0);
			tempButton2->SetValue(VisibilityProperty, 0);
			tempButton3->SetValue(VisibilityProperty, 0);
		}
		else {
			tempButton1->SetValue(VisibilityProperty, 1);
			tempButton2->SetValue(VisibilityProperty, 1);
			tempButton3->SetValue(VisibilityProperty, 1);
		}
	}
}

void flowchart::GridPage::hideAllButtons()
{
	UINT64 tempSymbolNo;
	Button^ tempButton1;
	Button^ tempButton2;
	Button^ tempButton3;
	for (UINT64 i = 0; i < App::symbolVector->Size; i++) {
		tempSymbolNo = App::symbolVector->GetAt(i)->SymbolNo;
		tempButton1 = nullptr;
		tempButton2 = nullptr;
		tempButton3 = nullptr;

		tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + tempSymbolNo));
		tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + tempSymbolNo));
		tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + tempSymbolNo));


		tempButton1->SetValue(VisibilityProperty, 1);
		tempButton2->SetValue(VisibilityProperty, 1);
		tempButton3->SetValue(VisibilityProperty, 1);

	}
	PageGrid->UpdateLayout();
}



