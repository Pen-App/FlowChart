﻿//
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
	mouseXPos = 0;
	mouseYPos = 0;
	makeGridArray(PageGrid, nowRowNum, nowColumnNum, rowHeight, columnWidth);

	isLineDrawing = false;
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
	tempRect->SetValue(Canvas::ZIndexProperty, 1);

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
	tempImage->AllowDrop = true;
	tempImage->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerEntered);
	tempImage->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerExited);
	tempImage->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerPressed);
	tempImage->DragOver += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Image_DragOver);
	tempImage->DragStarting += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::UIElement ^, Windows::UI::Xaml::DragStartingEventArgs ^>(this, &flowchart::GridPage::Image_DragStarting);
	tempImage->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Image_DragEnter);
	tempImage->DragLeave += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Image_DragLeave);
	tempImage->SetValue(Canvas::ZIndexProperty, 2);
	
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
	tempButton->SetValue(Canvas::ZIndexProperty, 2);

	switch (buttonType) {
	case 1:
		tempButton->Name = "b1 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_CONNECTOR;
		tempButton->CanDrag = true;
		tempButton->ClickMode = ClickMode::Press;
		tempButton->AddHandler(
			UIElement::PointerPressedEvent, 
			ref new Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::ConnectorButtonPress), 
			true);
		horizontal = 2;
		vertical = 2;
		break;
	case 2:
		tempButton->Name = "b2 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_CONTENTS;
		horizontal = 0;
		vertical = 2;

		// ContentButton을 클릭했을 때 flyout 이벤트
		tempButton->Click += ref new RoutedEventHandler(this, &flowchart::GridPage::ContentButtonClick);
		break;

	case 3:
		tempButton->Name = "b3 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_DETAIL;
		horizontal = 2;
		vertical = 0;

		// DetailButton을 클릭했을 때 flyout 이벤트
		tempButton->Click += ref new RoutedEventHandler(this, &flowchart::GridPage::DetailButtonClick);
		break;
	}
	tempButton->SetValue(HorizontalAlignmentProperty, horizontal);
	tempButton->SetValue(VerticalAlignmentProperty, vertical);
	parentGrid->Children->Append(tempButton);

}

// Content버튼 클릭했을때 flyout이 나오도록
void GridPage::ContentButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^ button = (Button ^)sender;
	Flyout^ flyout = ref new Flyout();	// flyout
	TextBlock^ textblock = ref new TextBlock();

	flyout = FLYOUT_TITLE;
	flyout->ShowAt(button);
}
// Detail버튼 클릭했을때 flyout이 나오도록
void GridPage::DetailButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^ button = (Button ^)sender;
	Flyout^ flyout = ref new Flyout();	// flyout
	flyout = FLYOUT_DETAIL;
	//TextBox^ textbox = ref new TextBox();

	//textbox->Width = 150;
	//textbox->Height = 100;
	//textbox->TextWrapping = TextWrapping::Wrap;

	//flyout->Placement = FlyoutPlacementMode::Right;	// flyout이 오른쪽에 나오도록
	//flyout->FlyoutPresenterStyle = FLYOUT_STYLE_PRESENTER;	// textbox가 flyout을 다 차지하는 style
	//flyout->Content = textbox;	// flyout 내부에 textbox 삽입
	flyout->ShowAt(button);
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

	PageGrid->Height = nowRowNum*rowHeight;
	PageGridCanvas->Height = PageGrid->Height;
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

	PageGrid->Width = nowColumnNum*columnWidth;
	PageGridCanvas->Width = PageGrid->Width;
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
	e->DragUIOverride->IsCaptionVisible = true;

	if (App::selectedSymbolNumber != -1 && !isSymbolIn) 
	{
		e->AcceptedOperation = DataPackageOperation::Copy;
		e->DragUIOverride->Caption = "추가";
		e->DragUIOverride->IsGlyphVisible = true;
	}
	else if(App::selectedSymbolNumber == -1 && !isSymbolIn)
	{
		e->AcceptedOperation = DataPackageOperation::Move;
		e->DragUIOverride->Caption = "이동";
		e->DragUIOverride->IsGlyphVisible = true;
	}
	else if (isSymbolIn)
	{
		e->AcceptedOperation = DataPackageOperation::None;
		e->DragUIOverride->IsCaptionVisible = false;
		e->DragUIOverride->IsGlyphVisible = true;
	}
	
}

//드래그된 상태에서 마우스버튼을 놓았을때, 즉 drop했을 때 어떤 일이 일어나는가?
void flowchart::GridPage::PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	if (App::selectedSymbolNumber != -1 && !isSymbolIn) { //symbol 생성 로직
		//1. map에 새로운 symbolInfo 객체를 추가시킨다. 
		UINT64 tempSymbolNo = App::symbolIdCount;
		App::symbolIdCount++;
		SymbolInfo^ tempSymbolInfo = ref new SymbolInfo();
		tempSymbolInfo->SymbolType = App::selectedSymbolNumber;
		tempSymbolInfo->RowIndex = curRowIndex;
		tempSymbolInfo->ColumnIndex = curColumnIndex;
		tempSymbolInfo->SymbolNo = tempSymbolNo;
		App::symbolVector->Append(tempSymbolInfo);


		makeImage(PageGrid, tempSymbolNo, App::selectedSymbolNumber, curRowIndex, curColumnIndex);
		makeButtons(PageGrid, tempSymbolNo, curRowIndex, curColumnIndex);

		showFocusedSymbolButtons(tempSymbolNo);
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
	}
	else if(!isSymbolIn){ //symbol 이동 로직
		moveFocusedSymbol(PageGrid, focusedSymbolNo, curRowIndex, curColumnIndex);

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
	}

	PageGrid->UpdateLayout();
	PageGridCanvas->UpdateLayout();
	PageGridScrollViewer->UpdateLayout();
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

//이미지 안에 커서가 들어오면 isSymbolIn이 true가 되고 나가면 false가 된다. 
void flowchart::GridPage::Image_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = true;

	//1. 이미지의 symbolNo를 알아낸다. 
	auto tempImageName = ((Image^)sender)->Name;
	focusedSymbolNo = std::stoi(tempImageName->Data() + 3);

	//2. 이미지의 symbolType을 알아낸다.
	wchar_t tempImageType[3];
	wcsncpy_s(tempImageType, tempImageName->Data(), 2);
	focusedSymbolType = _wtoi(tempImageType + 1);

	testTextBox->Text = "No:" + focusedSymbolNo + "/t: " + focusedSymbolType;

	OutputDebugString(L"Image_PointerEntered!!!\n");
}
void flowchart::GridPage::Image_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	isSymbolIn = true;
	OutputDebugString(L"Image_DragEnter!!!\n");
}

void flowchart::GridPage::Image_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = false;
	
	OutputDebugString(L"Image_PointerExited!!!\n");
}
void flowchart::GridPage::Image_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	isSymbolIn = false;
	OutputDebugString(L"Image_DragLeave!!!\n");
}

//이미지 안에서 클릭을 했을 경우 -> focus 시키기, 버튼보이게 하기
void flowchart::GridPage::Image_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isSymbolIn)
	{
		//1. 이미지의 symbolNo를 알아낸다. 
		auto tempImageName = ((Image^)sender)->Name;
		focusedSymbolNo = std::stoi(tempImageName->Data() + 3);
		
		//2. 이미지의 symbolType을 알아낸다.
		wchar_t tempImageType[3];
		wcsncpy_s(tempImageType, tempImageName->Data(), 2);
		focusedSymbolType = _wtoi(tempImageType + 1);

		//3. 포커스된 symbol의 버튼만 보이게 만든다. 
		showFocusedSymbolButtons(focusedSymbolNo);
		testTextBox->Text = "No:" + focusedSymbolNo + "/t: " + focusedSymbolType;
	}
	
	OutputDebugString(L"Image_clicked!!!\n");
}

//이미지의 드래그가 시작될 때
void flowchart::GridPage::Image_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{
	if (isSymbolIn)
	{
		//1. 이미지의 symbolNo를 알아낸다. 
		auto tempImageName = ((Image^)sender)->Name;
		focusedSymbolNo = std::stoi(tempImageName->Data() + 3);

		//2. 이미지의 symbolType을 알아낸다.
		wchar_t tempImageType[3];
		wcsncpy_s(tempImageType, tempImageName->Data(), 2);
		focusedSymbolType = _wtoi(tempImageType + 1);
		
		//3. 포커스된 symbol의 버튼만 보이게 만든다. 
		showFocusedSymbolButtons(focusedSymbolNo);
		testTextBox->Text = "No:" + focusedSymbolNo + "/t: " + focusedSymbolType;
	}
}

//필요없는 메소드 xxxxx 필요없음.
void flowchart::GridPage::Image_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	
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

void flowchart::GridPage::PageGridScrollViewer_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	//컨트롤이 눌렸는지 확인
	if (e->KeyModifiers == Windows::System::VirtualKeyModifiers::Control)
	{
		//마우스 휠이 어느 방향(위,아래)로 움직였는지 확인
		int delta = e->
			GetCurrentPoint((UIElement^)sender)->
			Properties->
			MouseWheelDelta;
		//현재 광학 줌의 정도
		float curZoomFactor = PageGridScrollViewer->ZoomFactor;

		//xaml에 Min,MaxZoomFactor에 의해 상한값 하한값은 자동으로 조절됩니다.
		if (delta >= 0) //위쪽으로 스크롤 했을 때
		{
			//줌 인
			PageGridScrollViewer->ZoomToFactor(curZoomFactor + 0.1);
		}
		else //아래쪽으로 스크롤 했을 때
		{
			//줌 아웃
			PageGridScrollViewer->ZoomToFactor(curZoomFactor - 0.1);
		}
	}

	while (
		PageGridCanvas->ActualWidth * PageGridScrollViewer->ZoomFactor
		< 
		PageGridScrollViewer->ActualWidth)
	{
		appendColumn();
		PageGrid->UpdateLayout();
		PageGridCanvas->UpdateLayout();
		PageGridScrollViewer->UpdateLayout();
	}
	while (
		PageGridCanvas->ActualHeight * PageGridScrollViewer->ZoomFactor
		< 
		PageGridScrollViewer->ActualHeight)
	{
		appendRow();
		PageGrid->UpdateLayout();
		PageGridCanvas->UpdateLayout();
		PageGridScrollViewer->UpdateLayout();
	}

	wchar_t debugstr[256];
	swprintf_s(debugstr, L"zoom : %lf\n", PageGridScrollViewer->ZoomFactor);
	OutputDebugString(debugstr);

	swprintf_s(debugstr, L"PageGridCanvas : %lf, %lf\n", PageGridCanvas->RenderSize.Width, PageGridCanvas->RenderSize.Height);
	OutputDebugString(debugstr);

	swprintf_s(debugstr, L"PageGridScrollViewer : %lf, %lf\n", PageGridScrollViewer->ActualWidth, PageGridScrollViewer->ActualHeight);
	OutputDebugString(debugstr);
}

void flowchart::GridPage::moveFocusedSymbol(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex)
{
	Button^ tempButton1 = nullptr;
	Button^ tempButton2 = nullptr;
	Button^ tempButton3 = nullptr;
	Image^ tempImage = nullptr;

	tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + focusedSymbolNo));
	tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + focusedSymbolNo));
	tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + focusedSymbolNo));
	tempImage = safe_cast<Image^>(PageGrid->FindName("i" + focusedSymbolType + " " + focusedSymbolNo));

	tempButton1->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton1->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempButton2->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton2->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempButton3->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton3->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempImage->SetValue(parentGrid->RowProperty, newRowIndex);
	tempImage->SetValue(parentGrid->ColumnProperty, newColumnIndex);

	parentGrid->UpdateLayout();
}

void flowchart::GridPage::PageGridScrollViewer_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	double gridRenderWidth = nowColumnNum * columnWidth * PageGridScrollViewer->ZoomFactor;
	double gridRenderHeight = nowRowNum * rowHeight * PageGridScrollViewer->ZoomFactor;

	wchar_t debugstr[256];
	swprintf_s(debugstr,
		L"zoom : %lf\ngrid : %lf %lf\nscroll : %lf %lf\n",
		PageGridScrollViewer->ZoomFactor,
		gridRenderWidth,
		gridRenderHeight,
		PageGridScrollViewer->ActualWidth,
		PageGridScrollViewer->ActualHeight);
	OutputDebugString(debugstr);

	if (PageGridScrollViewer->ZoomFactor >= 1) 
	{
		while (gridRenderWidth < (PageGridScrollViewer->ActualWidth*PageGridScrollViewer->ZoomFactor))
		{
			appendColumn();
			gridRenderWidth = nowColumnNum * columnWidth * PageGridScrollViewer->ZoomFactor;
		}
		while (gridRenderHeight < (PageGridScrollViewer->ActualHeight*PageGridScrollViewer->ZoomFactor))
		{
			appendRow();
			gridRenderHeight = nowRowNum * rowHeight * PageGridScrollViewer->ZoomFactor;
		}
	}
	else
	{
		while (gridRenderWidth < PageGridScrollViewer->ActualWidth)
		{
			appendColumn();
			gridRenderWidth = nowColumnNum * columnWidth * PageGridScrollViewer->ZoomFactor;
		}
		while (gridRenderHeight < PageGridScrollViewer->ActualHeight)
		{
			appendRow();
			gridRenderHeight = nowRowNum * rowHeight * PageGridScrollViewer->ZoomFactor;
		}
	}

	PageGrid->UpdateLayout();
	PageGridCanvas->UpdateLayout();
	PageGridScrollViewer->UpdateLayout();
}

void flowchart::GridPage::ConnectorButtonPress(
	Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isLineDrawing)
	{
		PageGridCanvas->Children->RemoveAtEnd();
	}

	OutputDebugString(L"connectorButtonPressed!!\n");
	isLineDrawing = true;
	connectorStartSymbolNo = focusedSymbolNo;

	//make line
	auto tempLine = ref new Line;
	tempLine->Name = L"tempLine";
	tempLine->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Red);
	tempLine->StrokeThickness = 1;
	tempLine->X1 = (mouseXPos + PageGridScrollViewer->HorizontalOffset) / PageGridScrollViewer->ZoomFactor;
	tempLine->Y1 = (mouseYPos + PageGridScrollViewer->VerticalOffset) / PageGridScrollViewer->ZoomFactor;
	tempLine->X2 = (mouseXPos + PageGridScrollViewer->HorizontalOffset) / PageGridScrollViewer->ZoomFactor;
	tempLine->Y2 = (mouseYPos + PageGridScrollViewer->VerticalOffset) / PageGridScrollViewer->ZoomFactor;
	PageGridCanvas->Children->Append(tempLine);
	PageGridCanvas->UpdateLayout();

	wchar_t asdf[234];
	swprintf_s(asdf, L"ScrollOffset : %lf, %lf\n", PageGridScrollViewer->VerticalOffset, PageGridScrollViewer->HorizontalOffset);
	OutputDebugString(asdf);
}

void flowchart::GridPage::PageGridCanvas_PointerPress(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isSymbolIn && isLineDrawing && connectorStartSymbolNo != focusedSymbolNo) {
		//그래프에 추가시킨다. 
		
		UINT64 tempSymbolNo;
		SymbolInfo^ startSymbolInfo = nullptr;
		SymbolInfo^ connectSymbolInfo = nullptr;
		//1. 기준이 되는 symbolInfo 찾기, app::vector에서
		for (UINT64 i = 0; i < App::symbolVector->Size; i++) 
		{
			if (App::symbolVector->GetAt(i)->SymbolNo == connectorStartSymbolNo)
			{
				startSymbolInfo = App::symbolVector->GetAt(i);
			}
		}

		//1.5 이미 startSymbolInfo에 connectorStartSymbolNo가 들어있으면 return한다.
		for (UINT64 i = 0; i < startSymbolInfo->Path->Size; i++)
		{
			if (startSymbolInfo->Path->GetAt(i)->SymbolNo == focusedSymbolNo)
			{
				return;
			}
		}

		//2. 이어질 connecctSymbolInfo 찾기
		for (UINT64 i = 0; i < App::symbolVector->Size; i++)
		{
			if (App::symbolVector->GetAt(i)->SymbolNo == focusedSymbolNo)
			{
				connectSymbolInfo = App::symbolVector->GetAt(i);
			}
		}

		//3. startSymbolInfo의 path에 connectSymbolInfo넣기
		startSymbolInfo->Path->Append(connectSymbolInfo);
		

		//debugging
		String^ tempStr = L"";
		for (UINT64 i = 0; i < startSymbolInfo->Path->Size; i++)
		{
			tempStr = tempStr + ((startSymbolInfo->Path->GetAt(i)->SymbolNo)) + L", " ;
		}
		pathBox->Text = tempStr;
		
	}
	OutputDebugString(L"canvas_press!!!\n");
}

void flowchart::GridPage::PageGridCanvas_PointerMove(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isLineDrawing)
	{
		OutputDebugString(L"move\n");
		Line^ tempLine = (Line^)(PageGridCanvas->FindName(L"tempLine"));
		auto movedPoint = e->GetCurrentPoint(this)->Position;
		tempLine->X2 = (movedPoint.X + PageGridScrollViewer->HorizontalOffset) / PageGridScrollViewer->ZoomFactor;
		tempLine->Y2 = (movedPoint.Y + PageGridScrollViewer->VerticalOffset) / PageGridScrollViewer->ZoomFactor;
		PageGridCanvas->UpdateLayout();
	}
	else
	{
		mouseXPos = e->GetCurrentPoint(this)->Position.X;
		mouseYPos = e->GetCurrentPoint(this)->Position.Y;
	}
}

void flowchart::GridPage::PageGridCanvas_PointerRelease(Platform::Object ^sender,
	Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	//throw ref new Platform::NotImplementedException();
	OutputDebugString(L"Release\n");

	if (isLineDrawing)
	{
		isLineDrawing = false;
		PageGridCanvas->Children->RemoveAtEnd();
		PageGridCanvas->UpdateLayout();
	}
}