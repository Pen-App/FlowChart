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
using namespace Windows::UI::Popups;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

//GridPage 생성자 : 생성과 동시에 makeGridArray로 행열을 그려준다. 
GridPage::GridPage()
{
	InitializeComponent();
	isSymbolIn = false;
	isSymbolRectIn = false;

	nowColumnNum = 10;
	nowRowNum = 10;
	columnWidth = safe_cast<int>(Resources->Lookup("gridWidth"));
	rowHeight = safe_cast<int>(Resources->Lookup("gridHeight"));
	mouseXPos = 0;
	mouseYPos = 0;
	tappedDeletorName = nullptr;
	makeGridArray(PageGrid, nowRowNum, nowColumnNum, rowHeight, columnWidth);

	isLineDrawing = false;

	if(App::symbolVector->Size != 0)
		LoadOpenedFile();
}

void GridPage::LoadOpenedFile()
{
	//MessageDialog^ msg = ref new MessageDialog("hello size = " + App::symbolVector->GetAt(0)->SymbolNo);
	//msg->ShowAsync();

	LoadingPageGridSize();

	// symbol 다시 그려주기
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		SymbolInfo^ symbolInfo = App::symbolVector->GetAt(i);
		makeSymbol(symbolInfo);
		LoadingConnectLine(symbolInfo);
	}
	// symbol 추가시 사용할 번호 갱신
	App::symbolIdCount = App::symbolVector->GetAt(App::symbolVector->Size - 1)->SymbolNo + 1;
	isSelectingYesOrNo = false;

	PageGrid->UpdateLayout();
	PageGridCanvas->UpdateLayout();
	PageGridScrollViewer->UpdateLayout();
}

void GridPage::makeSymbol(SymbolInfo^ symbolInfo)
{
	makeImage(PageGrid, symbolInfo->SymbolNo, symbolInfo->SymbolType, symbolInfo->RowIndex, symbolInfo->ColumnIndex);
	makeTextBlocks(PageGrid, symbolInfo->SymbolNo, symbolInfo->RowIndex, symbolInfo->ColumnIndex);
	makeSymbolRectangle(PageGrid, symbolInfo->SymbolNo, symbolInfo->SymbolType, symbolInfo->RowIndex, symbolInfo->ColumnIndex);
	makeButtons(PageGrid, symbolInfo->SymbolNo, symbolInfo->RowIndex, symbolInfo->ColumnIndex);
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
	tempRect->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerExited);
	tempRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragEnter);
	tempRect->DragLeave += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragLeave);
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
	tempImage->SetValue(parentGrid->RowProperty, rowIndex);
	tempImage->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempImage->CanDrag = true;
	tempImage->AllowDrop = true;
	tempImage->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerEntered);
	tempImage->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerExited);
	tempImage->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Image_PointerPressed);
	tempImage->DragStarting += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::UIElement ^, Windows::UI::Xaml::DragStartingEventArgs ^>(this, &flowchart::GridPage::Image_DragStarting);
	tempImage->DropCompleted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::UIElement^, Windows::UI::Xaml::DropCompletedEventArgs^>(this, &flowchart::GridPage::Image_DropComplete);
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
	tempButton->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Button_PointerEntered);
	tempButton->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Button_PointerExited);
	tempButton->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Button_DragEnter);
	tempButton->DragLeave += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Button_DragLeave);
	tempButton->AllowDrop = true;

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

		// TitleButton을 클릭했을 때 flyout 이벤트
		tempButton->Click += ref new RoutedEventHandler(this, &flowchart::GridPage::TitleButtonClick);
		break;

	case 3:
		tempButton->Name = "b3 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_DETAIL;
		horizontal = 2;
		vertical = 0;

		// DetailButton을 클릭했을 때 flyout 이벤트
		tempButton->Click += ref new RoutedEventHandler(this, &flowchart::GridPage::DetailButtonClick);
		break;

	case 4:
		tempButton->Name = "b4 " + symbolNo;
		tempButton->Style = BUTTON_STYLE_TITLE;
		horizontal = 0;
		vertical = 0;
		// ContentButton을 클릭했을 때 flyout 이벤트
		tempButton->Click += ref new RoutedEventHandler(this, &flowchart::GridPage::ContentButtonClick);
		break;
	}

	tempButton->SetValue(HorizontalAlignmentProperty, horizontal);
	tempButton->SetValue(VerticalAlignmentProperty, vertical);
	parentGrid->Children->Append(tempButton);

}

// Title버튼 클릭했을때 flyout이 나오도록
void GridPage::TitleButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^ button = (Button ^)sender;
	Flyout^ flyout = ref new Flyout();	// flyout

	
	if (App::focusedSymbolIndex == -1)	
	{	// 선택된 도형이 없을시 -> 도형이 처음 생성되었을 때
		TitleText->Text = nullptr;
	} else
	{	// 선택된 도형에 저장되어 있는 내용이 있을 때
		TitleText->Text = App::symbolVector->GetAt(App::focusedSymbolIndex)->Title;
	}
	flyout = FLYOUT_TITLE;
	flyout->ShowAt(button);
}
// Detail버튼 클릭했을때 flyout이 나오도록
void GridPage::DetailButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^ button = (Button ^)sender;
	Flyout^ flyout = ref new Flyout();	// flyout
	
	if (App::focusedSymbolIndex == -1)
	{	// 선택된 도형이 없을시 -> 도형이 처음 생성되었을 때
		DetailText->Text = nullptr;
	}
	else
	{	// 선택된 도형에 저장되어 있는 내용이 있을 때
		DetailText->Text = App::symbolVector->GetAt(App::focusedSymbolIndex)->Detail;
	}
	flyout = FLYOUT_DETAIL;
	flyout->ShowAt(button);
}
// Content버튼 클릭했을때 flyout 나오도록
void GridPage::ContentButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^ button = (Button ^)sender;
	Flyout^ flyout = ref new Flyout();	// flyout

	if (App::focusedSymbolIndex == -1)
	{	// 선택된 도형이 없을시 -> 도형이 처음 생성되었을 때
		ContentText->Text = nullptr;
	}
	else
	{	// 선택된 도형에 저장되어 있는 내용이 있을 때
		ContentText->Text = App::symbolVector->GetAt(App::focusedSymbolIndex)->Content;
	}
	flyout = FLYOUT_CONTENT;
	flyout->ShowAt(button);
}

//버튼 3종류 한꺼번에 만들기
void GridPage::makeButtons(Grid^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex)
{
	makeButton(parentGrid, symbolNo, 1, rowIndex, columnIndex);
	makeButton(parentGrid, symbolNo, 2, rowIndex, columnIndex);
	makeButton(parentGrid, symbolNo, 3, rowIndex, columnIndex);
	makeButton(parentGrid, symbolNo, 4, rowIndex, columnIndex);

}

void flowchart::GridPage::makeTitleTextBlock(Grid ^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex)
{
	TextBlock^ tempTextBlock = ref new TextBlock();
	tempTextBlock->Style = TITLE_TEXTBLOCK;
	tempTextBlock->Name = "title " + symbolNo;
	tempTextBlock->Text = "title " + symbolNo;
	tempTextBlock->SetValue(Canvas::ZIndexProperty, 2);
	tempTextBlock->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempTextBlock->SetValue(parentGrid->RowProperty, rowIndex);
	parentGrid->Children->Append(tempTextBlock);
}

void flowchart::GridPage::makeContentTextBlock(Grid ^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex)
{
	TextBlock^ tempTextBlock = ref new TextBlock();
	tempTextBlock->Style = CONTENT_TEXTBLOCK;
	tempTextBlock->Name = "content " + symbolNo;
	tempTextBlock->Text = "content " + symbolNo;
	tempTextBlock->SetValue(Canvas::ZIndexProperty, 2);
	tempTextBlock->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempTextBlock->SetValue(parentGrid->RowProperty, rowIndex);
	parentGrid->Children->Append(tempTextBlock);
}

void flowchart::GridPage::makeTextBlocks(Grid ^ parentGrid, UINT64 symbolNo, int rowIndex, int columnIndex)
{
	makeTitleTextBlock(parentGrid, symbolNo, rowIndex, columnIndex);
	makeContentTextBlock(parentGrid, symbolNo, rowIndex, columnIndex);
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
		else
		{
			Rectangle^ rt = safe_cast<Rectangle^>(symbolInPageGrid);
			if (rt->Name->Length() != 0)
			{
				Object^ rowPropertyValueObject =
					symbolInPageGrid->GetValue(Grid::RowProperty);
				int rowPropertyValueInt = safe_cast<int>(rowPropertyValueObject);
				rt->SetValue(Grid::RowProperty, rowPropertyValueInt + 1);
			}
		}
	}

	//벡터 내용 수정
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		auto tempSymbolInfo = App::symbolVector->GetAt(i);
		tempSymbolInfo->RowIndex++;
		//연결선 움직임
		moveConnectLine(tempSymbolInfo->SymbolNo);
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
		else
		{
			Rectangle^ rt = safe_cast<Rectangle^>(symbolInPageGrid);
			if (rt->Name->Length() != 0)
			{
				Object^ columnPropertyValueObject =
					symbolInPageGrid->GetValue(Grid::ColumnProperty);
				int columnPropertyValueInt = safe_cast<int>(columnPropertyValueObject);
				rt->SetValue(Grid::ColumnProperty, columnPropertyValueInt + 1);
			}
		}
	}

	//벡터 내용 수정
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		auto tempSymbolInfo = App::symbolVector->GetAt(i);
		tempSymbolInfo->ColumnIndex++;
		//연결선 움직임
		moveConnectLine(tempSymbolInfo->SymbolNo);
	}
}


//이벤트 함수들 

//이미지가 드래그된 상태로 pageGrid위로 올라가면 커서에 무엇을 보여줄 것인가?
void flowchart::GridPage::PageGrid_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	e->DragUIOverride->IsCaptionVisible = true;

	if (App::selectedSymbolNumber != -1 && !isSymbolIn && !isSymbolRectIn) 
	{
		e->AcceptedOperation = DataPackageOperation::Copy;
		e->DragUIOverride->Caption = "추가";
		e->DragUIOverride->IsGlyphVisible = true;
	}
	else if(App::selectedSymbolNumber == -1 && !isSymbolIn && !isSymbolRectIn)
	{
		e->AcceptedOperation = DataPackageOperation::Move;
		e->DragUIOverride->Caption = "이동";
		e->DragUIOverride->IsGlyphVisible = true;
	}
	else if (isSymbolIn || isSymbolRectIn)
	{
		e->AcceptedOperation = DataPackageOperation::None;
		e->DragUIOverride->IsCaptionVisible = false;
		e->DragUIOverride->IsGlyphVisible = true;
	}
	
}

//드래그된 상태에서 마우스버튼을 놓았을때, 즉 drop했을 때 어떤 일이 일어나는가?
void flowchart::GridPage::PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	if (App::selectedSymbolNumber != -1 && !isSymbolIn && !isSymbolRectIn) { //symbol 생성 로직
		//1. map에 새로운 symbolInfo 객체를 추가시킨다. 
		UINT64 tempSymbolNo = App::symbolIdCount;	
		App::symbolIdCount++;
		SymbolInfo^ tempSymbolInfo = ref new SymbolInfo();
		tempSymbolInfo->SymbolType = App::selectedSymbolNumber;
		tempSymbolInfo->RowIndex = curRowIndex;
		tempSymbolInfo->ColumnIndex = curColumnIndex;
		tempSymbolInfo->SymbolNo = tempSymbolNo;
		App::symbolVector->Append(tempSymbolInfo);
		App::focusedSymbolIndex = App::symbolVector->Size - 1;

		
		makeImage(PageGrid, tempSymbolNo, App::selectedSymbolNumber, curRowIndex, curColumnIndex);
		makeTextBlocks(PageGrid, tempSymbolNo, curRowIndex, curColumnIndex);
		makeButtons(PageGrid, tempSymbolNo, curRowIndex, curColumnIndex);
		makeSymbolRectangle(PageGrid, tempSymbolNo, App::selectedSymbolNumber, curRowIndex, curColumnIndex);
		

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
	else if(!isSymbolIn && !isSymbolRectIn){ //symbol 이동 로직
		moveSymbolInfoInSymbolVector(focusedSymbolNo, curRowIndex, curColumnIndex);
		moveSymbolRectangle(PageGrid, focusedSymbolNo, curRowIndex, curColumnIndex);
		moveFocusedSymbol(PageGrid, focusedSymbolNo, curRowIndex, curColumnIndex);
		moveTextBlocks(PageGrid, focusedSymbolNo, curRowIndex, curColumnIndex);
		moveYesOrNoTextBlock(PageGrid, focusedSymbolNo, curRowIndex, curColumnIndex);
		moveConnectLine(focusedSymbolNo);

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
	auto tempRectangleName = ((Rectangle^)sender)->Name;
	if (tempRectangleName->Length() != 0)
	{
		isSymbolRectIn = true;
	}

	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);


}

//마우스가 드래그된 상태에서 rectangle위로 올라가도 행,열 인덱스가 자동으로 curRowIndex와 curColumnIndex에 기록된다. 
void flowchart::GridPage::Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	auto tempRectangleName = ((Rectangle^)sender)->Name;
	if (tempRectangleName->Length() != 0)
	{
		isSymbolRectIn = true;
	}

	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);
}

//symbol이 없는 곳에서 마우스 클릭하면 모든 버튼이 숨겨진다. 
void flowchart::GridPage::Rectangle_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto tempRectangleName = ((Rectangle^)sender)->Name;
	if (tempRectangleName->Length() != 0)
	{
		isSymbolRectIn = true;
	}


	if (!isSymbolIn || !isSymbolRectIn) {
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
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		if (focusedSymbolNo == App::symbolVector->GetAt(i)->SymbolNo) {
			App::focusedSymbolIndex = i;
			break;
		}
	}

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
		for (int i = 0; i < App::symbolVector->Size; i++)
		{
			if (focusedSymbolNo == App::symbolVector->GetAt(i)->SymbolNo) {
				App::focusedSymbolIndex = i;
				break;
			}
		}

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

		//App.xaml.h.DraggingSymbolNo 설정
		App::draggingSymbolNo = focusedSymbolNo;
		App::draggingSymbolType = focusedSymbolType;
	}
}

//이미지의 드래그가 끝났을 때
void flowchart::GridPage::Image_DropComplete(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::DropCompletedEventArgs ^ args)
{
	App::draggingSymbolNo = -1;
	App::draggingSymbolType = -1;
}


//파라미터로 넘겨진 id의 symbol 버튼만 보이게 하는 함수
void flowchart::GridPage::showFocusedSymbolButtons(UINT64 focusedSymbolNo)
{
	UINT64 tempSymbolNo;
	Button^ tempButton1;
	Button^ tempButton2;
	Button^ tempButton3;
	Button^ tempButton4;
	Ellipse^ lineDeletor1;
	Ellipse^ lineDeletor2;

	for (UINT64 i = 0; i < App::symbolVector->Size; i++) {
		SymbolInfo^ tempSymbolInfo = App::symbolVector->GetAt(i);
		tempSymbolNo = tempSymbolInfo->SymbolNo;
		tempButton1 = nullptr;
		tempButton2 = nullptr;
		tempButton3 = nullptr;
		tempButton4 = nullptr;
		lineDeletor1 = nullptr;
		lineDeletor2 = nullptr;

		tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + tempSymbolNo));
		tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + tempSymbolNo));
		tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + tempSymbolNo));
		tempButton4 = safe_cast<Button^>(PageGrid->FindName("b4 " + tempSymbolNo));

		if (tempSymbolNo == focusedSymbolNo) {
			tempButton1->SetValue(VisibilityProperty, 0);
			tempButton2->SetValue(VisibilityProperty, 0);
			tempButton3->SetValue(VisibilityProperty, 0);
			tempButton4->SetValue(VisibilityProperty, 0);

			for (UINT64 j = 0; j < tempSymbolInfo->Path->Size; j++)
			{
				SymbolInfo^ targetSymbolInfo = tempSymbolInfo->Path->GetAt(j);
				lineDeletor1 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor1 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));
				lineDeletor2 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor2 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));
				
				lineDeletor1->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Visible);
				lineDeletor2->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Visible);
			}
		}
		else {
			tempButton1->SetValue(VisibilityProperty, 1);
			tempButton2->SetValue(VisibilityProperty, 1);
			tempButton3->SetValue(VisibilityProperty, 1);
			tempButton4->SetValue(VisibilityProperty, 1);

			for (UINT64 j = 0; j < tempSymbolInfo->Path->Size; j++)
			{
				SymbolInfo^ targetSymbolInfo = tempSymbolInfo->Path->GetAt(j);
				lineDeletor1 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor1 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));
				lineDeletor2 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor2 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));

				lineDeletor1->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Collapsed);
				lineDeletor2->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Collapsed);
			}
		}
	}
}

void flowchart::GridPage::hideAllButtons()
{
	UINT64 tempSymbolNo;
	Button^ tempButton1;
	Button^ tempButton2;
	Button^ tempButton3;
	Button^ tempButton4;
	Ellipse^ lineDeletor1;
	Ellipse^ lineDeletor2;

	for (UINT64 i = 0; i < App::symbolVector->Size; i++) {
		SymbolInfo^ tempSymbolInfo = App::symbolVector->GetAt(i);
		tempSymbolNo = tempSymbolInfo->SymbolNo;
		tempButton1 = nullptr;
		tempButton2 = nullptr;
		tempButton3 = nullptr;
		tempButton4 = nullptr;
		lineDeletor1 = nullptr;
		lineDeletor2 = nullptr;

		tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + tempSymbolNo));
		tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + tempSymbolNo));
		tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + tempSymbolNo));
		tempButton4 = safe_cast<Button^>(PageGrid->FindName("b4 " + tempSymbolNo));

		tempButton1->SetValue(VisibilityProperty, 1);
		tempButton2->SetValue(VisibilityProperty, 1);
		tempButton3->SetValue(VisibilityProperty, 1);
		tempButton4->SetValue(VisibilityProperty, 1);

		for (UINT64 j = 0; j < tempSymbolInfo->Path->Size; j++)
		{
			SymbolInfo^ targetSymbolInfo = tempSymbolInfo->Path->GetAt(j);
			lineDeletor1 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor1 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));
			lineDeletor2 = safe_cast<Ellipse^>(PageGridCanvas->FindName("lineDeletor2 " + tempSymbolInfo->SymbolNo + " to " + targetSymbolInfo->SymbolNo));

			lineDeletor1->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Collapsed);
			lineDeletor2->SetValue(VisibilityProperty, Windows::UI::Xaml::Visibility::Collapsed);
		}
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
	Button^ tempButton4 = nullptr;
	Image^ tempImage = nullptr;
	

	tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + focusedSymbolNo));
	tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + focusedSymbolNo));
	tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + focusedSymbolNo));
	tempButton4 = safe_cast<Button^>(PageGrid->FindName("b4 " + focusedSymbolNo));

	tempImage = safe_cast<Image^>(PageGrid->FindName("i" + focusedSymbolType + " " + focusedSymbolNo));
	

	tempButton1->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton1->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempButton2->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton2->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempButton3->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton3->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempButton4->SetValue(parentGrid->RowProperty, newRowIndex);
	tempButton4->SetValue(parentGrid->ColumnProperty, newColumnIndex);

	tempImage->SetValue(parentGrid->RowProperty, newRowIndex);
	tempImage->SetValue(parentGrid->ColumnProperty, newColumnIndex);

	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		if (App::symbolVector->GetAt(i)->SymbolNo == focusedSymbolNo)
		{
			App::symbolVector->GetAt(i)->ColumnIndex = newColumnIndex;
			App::symbolVector->GetAt(i)->RowIndex = newRowIndex;
			break;
		}
	}
}

void flowchart::GridPage::moveSymbolInfoInSymbolVector(UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex)
{
	SymbolInfo^ movedSymbolInfo = App::getSymbolInfoByNo(focusedSymbolNo);
	if (movedSymbolInfo == nullptr) return;
	movedSymbolInfo->RowIndex = newRowIndex;
	movedSymbolInfo->ColumnIndex = newColumnIndex;
}

void flowchart::GridPage::moveSymbolRectangle(Grid ^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex)
{
	Rectangle^ tempRectangle = nullptr;
	tempRectangle = safe_cast<Rectangle^>(PageGrid->FindName("r" + focusedSymbolType + " " + focusedSymbolNo));
	tempRectangle->SetValue(parentGrid->RowProperty, newRowIndex);
	tempRectangle->SetValue(parentGrid->ColumnProperty, newColumnIndex);

	parentGrid->UpdateLayout();
}

void flowchart::GridPage::moveTextBlocks(Grid ^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex)
{
	TextBlock^ tempTitleTextBlock = nullptr;
	TextBlock^ tempContentTextBlock = nullptr;

	tempTitleTextBlock = safe_cast<TextBlock^>(PageGrid->FindName("title " + focusedSymbolNo));
	tempContentTextBlock = safe_cast<TextBlock^>(PageGrid->FindName("content " + focusedSymbolNo));

	tempTitleTextBlock->SetValue(parentGrid->RowProperty, newRowIndex);
	tempTitleTextBlock->SetValue(parentGrid->ColumnProperty, newColumnIndex);
	tempContentTextBlock->SetValue(parentGrid->RowProperty, newRowIndex);
	tempContentTextBlock->SetValue(parentGrid->ColumnProperty, newColumnIndex);
}

void flowchart::GridPage::moveYesOrNoTextBlock(Grid^ parentGrid, UINT64 focusedSymbolNo, int newRowIndex, int newColumnIndex)
{
	//1. 심볼넘버로 move된 심볼을 찾는다.
	SymbolInfo^ movedSymbolInfo = App::getSymbolInfoByNo(focusedSymbolNo);
	if (movedSymbolInfo == nullptr) return;

	//decision 심볼일 때
	if (movedSymbolInfo->SymbolType == 2)
	{
		//2. YesOrNo 텍스트블록을 감싸고 있는 보더를 움직인다.
		Border^ tempYesOrNoBorder = nullptr;

		for (int i = 0; i < movedSymbolInfo->Path->Size; i++)
		{
			//2-1. YesOrNo의 Grid위치를 바꾼다.
			auto targetSymbolInfo = movedSymbolInfo->Path->GetAt(i);
			tempYesOrNoBorder = safe_cast<Border^>(PageGrid->FindName("decisionText " + focusedSymbolNo + " to " + targetSymbolInfo->SymbolNo));
			tempYesOrNoBorder->SetValue(Grid::RowProperty, newRowIndex);
			tempYesOrNoBorder->SetValue(Grid::ColumnProperty, newColumnIndex);

			//2-2. YesOrNo의 Grid안에서 위치를 바꾼다.
			int direction = getDirectionTartgetSymbol(movedSymbolInfo, targetSymbolInfo);
			switch (direction)
			{
			case 1:
			case 2:
			{
				tempYesOrNoBorder->Margin = Thickness(0, 0, (columnWidth / 2.0), 0);
				break;
			}
			case 3:
			case 4:
			{
				tempYesOrNoBorder->Margin = Thickness(0, 0, 0, (rowHeight / 2.0));
				break;
			}
			case 5:
			case 6:
			{
				tempYesOrNoBorder->Margin = Thickness(0, (rowHeight / 2.0), 0, 0);
				break;
			}
			case 7:
			case 8:
			{
				tempYesOrNoBorder->Margin = Thickness((columnWidth / 2.0), 0, 0, 0);
				break;
			}
			default:
				tempYesOrNoBorder->HorizontalAlignment =
					Windows::UI::Xaml::HorizontalAlignment::Center;
				tempYesOrNoBorder->VerticalAlignment =
					Windows::UI::Xaml::VerticalAlignment::Center;
				break;
			}
		}
	}
	//아닐 때도 decision과 연결되있으면 방향을 바꿔야 할 수도 있음
	else
	{
		//2. YesOrNo 텍스트블록을 감싸고 있는 보더를 움직인다.
		Border^ tempYesOrNoBorder = nullptr;
		
		//2-1. 모든 심볼을 조사하면서
		for (int i = 0; i < App::symbolVector->Size; i++)
		{
			auto tempSymbolInfo = App::symbolVector->GetAt(i);

			//2-2. 심볼이 decision이면서
			if (tempSymbolInfo->SymbolType != 2)
			{
				continue;
			}

			for (int j = 0; j < tempSymbolInfo->Path->Size; j++)
			{
				auto targetSymbolInfo = tempSymbolInfo->Path->GetAt(j);

				//2-3. 연결된 심볼이 움직인 심볼이라면
				if (targetSymbolInfo->SymbolNo == focusedSymbolNo)
				{
					//2-4. YesOrNo의 Grid안에서 위치를 바꾼다.
					tempYesOrNoBorder = safe_cast<Border^>(PageGrid->FindName("decisionText " + tempSymbolInfo->SymbolNo + " to " + focusedSymbolNo));

					int direction = getDirectionTartgetSymbol(tempSymbolInfo, targetSymbolInfo);
					switch (direction)
					{
					case 1:
					case 2:
					{
						tempYesOrNoBorder->Margin = Thickness(0, 0, (columnWidth / 2.0), 0);
						break;
					}
					case 3:
					case 4:
					{
						tempYesOrNoBorder->Margin = Thickness(0, 0, 0, (rowHeight / 2.0));
						break;
					}
					case 5:
					case 6:
					{
						tempYesOrNoBorder->Margin = Thickness(0, (rowHeight / 2.0), 0, 0);
						break;
					}
					case 7:
					case 8:
					{
						tempYesOrNoBorder->Margin = Thickness((columnWidth / 2.0), 0, 0, 0);
						break;
					}
					default:
						tempYesOrNoBorder->HorizontalAlignment =
							Windows::UI::Xaml::HorizontalAlignment::Center;
						tempYesOrNoBorder->VerticalAlignment =
							Windows::UI::Xaml::VerticalAlignment::Center;
						break;
					}
				}
			}
		}
	}

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
		if (startSymbolInfo->SymbolType != 2)
		{
			startSymbolInfo->Path->Append(connectSymbolInfo);
		}

		if (startSymbolInfo->SymbolType == 2) 
		{
			Flyout^ yesOrNoFlyout = YES_OR_NO_FLYOUT;

			String^ connectSymbolName = ref new String(L"i");
			connectSymbolName += connectSymbolInfo->SymbolType;
			connectSymbolName += L" ";
			connectSymbolName += connectSymbolInfo->SymbolNo;
			Image^ connectSymbolImage =
				safe_cast<Image^>(PageGrid->FindName(connectSymbolName));

			yesOrNoFlyout->ShowAt(connectSymbolImage);
			isSelectingYesOrNo = true;
		}
		else
		{
			//4. 간이 연결선 삭제
			deleteTempConnectLine();

			//5. 실제 연결선 생성
			makeConnectLine(connectorStartSymbolNo, connectSymbolInfo->SymbolNo);
			isLineDrawing = false;
		}

		//debugging
		String^ tempStr = "startSymbol:" + connectorStartSymbolNo + " ";
		for (UINT64 i = 0; i < startSymbolInfo->Path->Size; i++)
		{
			tempStr = tempStr + ((startSymbolInfo->Path->GetAt(i)->SymbolNo)) + L", " ;
		}
		pathBox->Text = tempStr;
		
	}
	OutputDebugString(L"canvas_press!!!\n");
	wchar_t asdf[234];
	swprintf_s(asdf, L"%d %d\n", nowColumnNum, nowRowNum);
	OutputDebugString(asdf);
	if (isLineDrawing && !isSelectingYesOrNo)
	{
		isLineDrawing = false;
		isSelectingYesOrNo = false;
		deleteTempConnectLine();
	}

	isSelectingYesOrNo = false;
}

void flowchart::GridPage::PageGridCanvas_PointerMove(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (isLineDrawing)
	{
		//OutputDebugString(L"move\n");
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

void flowchart::GridPage::makeSymbolRectangle(Grid ^ parentGrid, UINT64 symbolNo, int symbolType, int rowIndex, int columnIndex)
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
	tempRect->SetValue(NameProperty, "r" + symbolType + " " + symbolNo);

	tempRect->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerEntered);
	tempRect->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerPressed);
	tempRect->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &flowchart::GridPage::Rectangle_PointerExited);
	tempRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragEnter);
	tempRect->DragLeave += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragLeave);
	parentGrid->Children->Append(tempRect);
}


void flowchart::GridPage::Rectangle_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolRectIn = false;
}


void flowchart::GridPage::Rectangle_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	isSymbolRectIn = false;
}


void flowchart::GridPage::Button_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = true;
	//1. 버튼의 symbolNo를 알아낸다. 
	auto tempButtonName = ((Button^)sender)->Name;
	focusedSymbolNo = std::stoi(tempButtonName->Data() + 3);
}


void flowchart::GridPage::Button_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	isSymbolIn = false;
}


void flowchart::GridPage::Button_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	isSymbolIn = true;
}


void flowchart::GridPage::Button_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	isSymbolIn = false;
}


void flowchart::GridPage::PageGridScrollViewer_ViewChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ e)
{
	wchar_t debugStr[256];
	swprintf_s(debugStr, L"zoomLevel : %f\n", PageGridScrollViewer->ZoomFactor);
	OutputDebugString(debugStr);

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
}


void flowchart::GridPage::makeConnectLine(UINT16 from, UINT16 to)
{
	SymbolInfo^ fromInfo = App::getSymbolInfoByNo(from);
	SymbolInfo^ toInfo = App::getSymbolInfoByNo(to);
	if (fromInfo == nullptr || toInfo == nullptr) return;

	int direction;
	double fromXPos, fromYPos;
	double turn1X, turn1Y;
	double turn2X, turn2Y;
	double turn3X, turn3Y;
	double toXPos, toYPos;

	direction = getDirectionTartgetSymbol(fromInfo, toInfo);

	switch (direction)
	{
	case 1:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn2X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn2Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn3Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0);
		break;
	}
	case 2:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn2X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn2Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn3X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn3Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		break;
	}
	case 3:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		turn2X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth+70) / 2.0) + 10;
		turn2Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn3Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		break;
	}
	case 4:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		turn2X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn2Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn3Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0) - 10;
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + ((rowHeight - 40) / 2.0);
		break;
	}
	case 5:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		turn2X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn2Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn3Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0);
		break;
	}
	case 6:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		turn2X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn2Y = ((fromInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0) - 10;
		turn3Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth - 70) / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		break;
	}
	case 7:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn2X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn2Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn3Y = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		break;
	}
	case 8:
	{
		fromXPos = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0);
		fromYPos = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn1X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn1Y = ((fromInfo->RowIndex)*rowHeight) + (rowHeight / 2.0);
		turn2X = ((fromInfo->ColumnIndex)*columnWidth) + ((columnWidth + 70) / 2.0) + 10;
		turn2Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		turn3X = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		turn3Y = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0) + 10;
		toXPos = ((toInfo->ColumnIndex)*columnWidth) + (columnWidth / 2.0);
		toYPos = ((toInfo->RowIndex)*rowHeight) + ((rowHeight + 40) / 2.0);
		break;
	}
	default:
	{
		fromXPos = 0;
		fromYPos = 0;
		turn1X = 0;
		turn1Y = 0;
		turn2X = 0;
		turn2Y = 0;
		turn3X = 0;
		turn3Y = 0;
		toXPos = 0;
		toYPos = 0;
	}
	}
	
	Polyline^ connectLine = ref new Polyline;

	connectLine->Name = "connectLine " + from + " to " + to;
	connectLine->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
	connectLine->StrokeThickness = 1;
	PointCollection^ connectLinePoints = ref new PointCollection;
	connectLinePoints->Append(*(ref new Point(fromXPos, fromYPos)));
	connectLinePoints->Append(*(ref new Point(turn1X, turn1Y)));
	connectLinePoints->Append(*(ref new Point(turn2X, turn2Y)));
	connectLinePoints->Append(*(ref new Point(turn3X, turn3Y)));
	connectLinePoints->Append(*(ref new Point(toXPos, toYPos)));
	connectLine->Points = connectLinePoints;
	PageGridCanvas->Children->Append(connectLine);

	Ellipse^ lineDeletor1 = ref new Ellipse;
	lineDeletor1->Name = "lineDeletor1 " + from + " to " + to;
	Color deletorColor;
	deletorColor.R = 0;
	deletorColor.G = 150;
	deletorColor.B = 50;
	deletorColor.A = 150;
	lineDeletor1->Fill = ref new SolidColorBrush(deletorColor);
	lineDeletor1->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &flowchart::GridPage::LineDeletor_Tapped);
	lineDeletor1->Width = 8;
	lineDeletor1->Height = 8;
	lineDeletor1->SetValue(Canvas::TopProperty, (fromYPos-4));
	lineDeletor1->SetValue(Canvas::LeftProperty, (fromXPos-4));
	lineDeletor1->Visibility = Windows::UI::Xaml::Visibility::Visible;
	PageGridCanvas->Children->Append(lineDeletor1);

	Ellipse^ lineDeletor2 = ref new Ellipse;
	lineDeletor2->Name = "lineDeletor2 " + from + " to " + to;
	lineDeletor2->Fill = ref new SolidColorBrush(deletorColor);
	lineDeletor2->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &flowchart::GridPage::LineDeletor_Tapped);
	lineDeletor2->Width = 8;
	lineDeletor2->Height = 8;
	lineDeletor2->SetValue(Canvas::TopProperty, (toYPos-4));
	lineDeletor2->SetValue(Canvas::LeftProperty, (toXPos-4));
	lineDeletor2->Visibility = Windows::UI::Xaml::Visibility::Visible;
	PageGridCanvas->Children->Append(lineDeletor2);

	PageGridCanvas->UpdateLayout();
}


void flowchart::GridPage::moveConnectLine(UINT16 movedSymbolNo)
{
	//move된 심볼의 정보를 찾는다
	SymbolInfo^ movedSymbolInfo = App::getSymbolInfoByNo(movedSymbolNo);
	if (movedSymbolInfo == nullptr) return;

	//모든 심볼정보를 순회하며, move된 심볼과 연결된 심볼의 연결선을 새로 생성해준다
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		SymbolInfo^ tempSymbolInfo = App::symbolVector->GetAt(i);

		//움직인 심볼에서 나온 선을 움직인다
		if (tempSymbolInfo == movedSymbolInfo)
		{
			//연결된 심볼의 정보를 순회
			for (int j = 0; j < movedSymbolInfo->Path->Size; j++)
			{
				SymbolInfo^ connectSymbolInfo = movedSymbolInfo->Path->GetAt(j);
				
				//선이름 생성
				String^ connectLineNameStr = L"connectLine ";
				connectLineNameStr += movedSymbolInfo->SymbolNo;
				connectLineNameStr += L" to ";
				connectLineNameStr += connectSymbolInfo->SymbolNo;

				//deletor 이름 생성
				String^ lineDeletor1NameStr = "lineDeletor1 " + 
											  movedSymbolInfo->SymbolNo + 
											  " to " + 
											  connectSymbolInfo->SymbolNo;
				String^ lineDeletor2NameStr = "lineDeletor2 " +
											  movedSymbolInfo->SymbolNo +
											  " to " +
											  connectSymbolInfo->SymbolNo;

				//선이름으로 선을 찾아서 삭제
				UIElement^ childPageGridCanvas = nullptr;
				for (int k = 0; k < PageGridCanvas->Children->Size; k++)
				{
					childPageGridCanvas = PageGridCanvas->Children->GetAt(k);

					//선 삭제
					if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Polyline") == 0)
					{
						Polyline^ connectLine = safe_cast<Polyline^>(childPageGridCanvas);
						if (wcscmp(connectLine->Name->Data(), connectLineNameStr->Data()) == 0)
						{
							PageGridCanvas->Children->RemoveAt(k);
							k--;
						}
					}
					//lineDeletor 삭제
					else if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Ellipse") == 0)
					{
						Ellipse^ lineDeletor = safe_cast<Ellipse^>(childPageGridCanvas);
						if (lineDeletor->Name == lineDeletor1NameStr || lineDeletor->Name == lineDeletor2NameStr)
						{
							PageGridCanvas->Children->RemoveAt(k);
							k--;
						}
					}
				}

				//선 새로 다시 그려줌
				makeConnectLine(movedSymbolInfo->SymbolNo, connectSymbolInfo->SymbolNo);
			}
		}
		//다른 심볼에서 나온 움직인 심볼과 연결된 선을 움직인다
		else
		{
			//연결된 심볼의 정보를 순회
			for (int j = 0; j < tempSymbolInfo->Path->Size; j++)
			{
				//움직인 심볼과 연결됨
				if (tempSymbolInfo->Path->GetAt(j) == movedSymbolInfo)
				{
					//선이름 생성
					String^ connectLineNameStr = L"connectLine ";
					connectLineNameStr += tempSymbolInfo->SymbolNo;
					connectLineNameStr += L" to ";
					connectLineNameStr += movedSymbolNo;

					//deletor 이름 생성
					String^ lineDeletor1NameStr = "lineDeletor1 " +
												  tempSymbolInfo->SymbolNo +
												  " to " +
												  movedSymbolNo;
					String^ lineDeletor2NameStr = "lineDeletor2 " +
												  tempSymbolInfo->SymbolNo +
												  " to " +
												  movedSymbolNo;

					//선이름으로 선을 찾아서 움직여줌
					UIElement^ childPageGridCanvas = nullptr;
					for (int k = 0; k < PageGridCanvas->Children->Size; k++)
					{
						childPageGridCanvas = PageGridCanvas->Children->GetAt(k);

						//선 삭제
						if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Polyline") == 0)
						{
							Polyline^ connectLine = safe_cast<Polyline^>(childPageGridCanvas);
							if (wcscmp(connectLine->Name->Data(), connectLineNameStr->Data()) == 0)
							{
								PageGridCanvas->Children->RemoveAt(k);
								k--;
							}
						}
						//lineDeletor 삭제
						else if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Ellipse") == 0)
						{
							Ellipse^ lineDeletor = safe_cast<Ellipse^>(childPageGridCanvas);
							if (lineDeletor->Name == lineDeletor1NameStr || lineDeletor->Name == lineDeletor2NameStr)
							{
								PageGridCanvas->Children->RemoveAt(k);
								k--;
							}
						}
					}

					//선 새로 다시 그려줌
					makeConnectLine(tempSymbolInfo->SymbolNo, movedSymbolInfo->SymbolNo);
					break;
				}
			}
		}
	}
}

//파일오픈용 연결선 함수
void flowchart::GridPage::LoadingConnectLine(SymbolInfo^ fromInfo)
{
	if (fromInfo->SymbolType == 2)
	{
		for (int i = 0; i < fromInfo->Path->Size; i++)
		{
			auto toInfo = fromInfo->Path->GetAt(i);
			makeConnectLine(fromInfo->SymbolNo, toInfo->SymbolNo);
			makeYesOrNoTextBlock(fromInfo->SymbolNo, toInfo->SymbolNo, fromInfo->Decision->GetAt(i));
		}
	}
	else
	{
		for (int i = 0; i < fromInfo->Path->Size; i++)
		{
			auto toInfo = fromInfo->Path->GetAt(i);
			makeConnectLine(fromInfo->SymbolNo, toInfo->SymbolNo);
		}
	}
}

//연결 대상의 방향을 알려주는 함수
int flowchart::GridPage::getDirectionTartgetSymbol(SymbolInfo^ fromInfo, SymbolInfo^ toInfo)
{
	int direction;

	if (fromInfo->ColumnIndex > toInfo->ColumnIndex)
	{
		if (fromInfo->RowIndex > toInfo->RowIndex)
		{
			direction = 1;
		}
		else if (fromInfo->RowIndex == toInfo->RowIndex)
		{
			direction = 4;
		}
		else
		{
			direction = 6;
		}
	}
	else if (fromInfo->ColumnIndex == toInfo->ColumnIndex)
	{
		if (fromInfo->RowIndex > toInfo->RowIndex)
		{
			direction = 2;
		}
		else
		{
			direction = 7;
		}
	}
	else
	{
		if (fromInfo->RowIndex > toInfo->RowIndex)
		{
			direction = 3;
		}
		else if (fromInfo->RowIndex == toInfo->RowIndex)
		{
			direction = 5;
		}
		else
		{
			direction = 8;
		}
	}

	return direction;
}

//간이 연결선 삭제해주는 함수
void flowchart::GridPage::deleteTempConnectLine()
{
	for (int i = 0; i < PageGridCanvas->Children->Size; i++)
	{
		auto childOfPageGridCanvas = PageGridCanvas->Children->GetAt(i);
		String^ childOfType = childOfPageGridCanvas->ToString();
		if (childOfType == ref new String(L"Windows.UI.Xaml.Shapes.Line"))
		{
			Line^ lineOnCanvas = safe_cast<Line^>(childOfPageGridCanvas);
			if (lineOnCanvas->Name == ref new String(L"tempLine"))
			{
				PageGridCanvas->Children->RemoveAt(i);
				break;
			}
		}
	}
	PageGridCanvas->UpdateLayout();
}

// Detail Flyout에서 텍스트를 입력받는 중일 때
void flowchart::GridPage::DetailText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args)
{
	// vector에 저장
	App::symbolVector->GetAt(App::focusedSymbolIndex)->Detail = sender->Text;
}

// Title Flyout에서 텍스트를 입력받는 중일 때
void flowchart::GridPage::TitleText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args)
{
	App::symbolVector->GetAt(App::focusedSymbolIndex)->Title = sender->Text;
	TextBlock^ title = safe_cast<TextBlock^>(PageGrid->FindName("title " + focusedSymbolNo));
	title->Text = App::symbolVector->GetAt(App::focusedSymbolIndex)->Title;
}

// Content Flyout에서 텍스트를 입력받는 중일 때
void flowchart::GridPage::ContentText_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args)
{
	App::symbolVector->GetAt(App::focusedSymbolIndex)->Content = sender->Text;
	TextBlock^ content = safe_cast<TextBlock^>(PageGrid->FindName("content " + focusedSymbolNo));
	content->Text = App::symbolVector->GetAt(App::focusedSymbolIndex)->Content;
}

//YES_OR_NO_FLYOUT의 버튼을 클릭했을 때
void flowchart::GridPage::YesOrNoFlyoutButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	OutputDebugString(L"YesOrNoFlyoutButton_Click\n");
	Button^ yesOrNoButton = safe_cast<Button^>(sender);

	SymbolInfo^ startSymbolInfo = App::getSymbolInfoByNo(connectorStartSymbolNo);
	SymbolInfo^ connectSymbolInfo = App::getSymbolInfoByNo(focusedSymbolNo);
	if (startSymbolInfo == nullptr || connectSymbolInfo == nullptr)
	{
		return;
	}

	//3. startSymbolInfo의 path에 connectSymbolInfo넣기
	startSymbolInfo->Path->Append(connectSymbolInfo);

	//4. 간이 연결선 삭제
	deleteTempConnectLine();

	//5. 실제 연결선 생성
	makeConnectLine(connectorStartSymbolNo, connectSymbolInfo->SymbolNo);
	isLineDrawing = false;

	/*//6. 누른 버튼에 따라서 yes텍스트나 no텍스트 생성
	//6-1. yes or no를 표시할 테두리 설정
	Border^ borderOfYesOrNoTextBlock = ref new Border;
	String^ nameOfTextBlockBorder = ref new String(L"decisionText ");
	nameOfTextBlockBorder += connectorStartSymbolNo;
	nameOfTextBlockBorder += L" to ";
	nameOfTextBlockBorder += connectSymbolInfo->SymbolNo;
	borderOfYesOrNoTextBlock->Name = nameOfTextBlockBorder;
	borderOfYesOrNoTextBlock->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Gray);
	borderOfYesOrNoTextBlock->BorderThickness = 1;
	borderOfYesOrNoTextBlock->Width = 30;
	borderOfYesOrNoTextBlock->Height = 20;
	borderOfYesOrNoTextBlock->IsTapEnabled = false;
	borderOfYesOrNoTextBlock->SetValue(Canvas::ZIndexProperty, 3);
	borderOfYesOrNoTextBlock->SetValue(Grid::RowProperty, startSymbolInfo->RowIndex);
	borderOfYesOrNoTextBlock->SetValue(Grid::ColumnProperty, startSymbolInfo->ColumnIndex);
	int direction = getDirectionTartgetSymbol(startSymbolInfo, connectSymbolInfo);

	//6-2. yes or no 위치 조정
	switch (direction)
	{
	case 1:
	case 2:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, 0, (columnWidth/2.0), 0);
		break;
	}
	case 3:
	case 4:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, 0, 0, (rowHeight/2.0));
		break;
	}
	case 5:
	case 6:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, (rowHeight/2.0), 0, 0);
		break;
	}
	case 7:
	case 8:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness((columnWidth/2.0), 0, 0, 0);
		break;
	}
	default:
		borderOfYesOrNoTextBlock->HorizontalAlignment =
			Windows::UI::Xaml::HorizontalAlignment::Center;
		borderOfYesOrNoTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		break;
	}

	//6-3. yes or no 텍스트 생성
	if (yesOrNoButton->Name == L"YesFlyoutButton")
	{
		TextBlock^ yesTextBlock = ref new TextBlock;
		yesTextBlock->Text = L"Yes";
		yesTextBlock->HorizontalAlignment = 
			Windows::UI::Xaml::HorizontalAlignment::Center;
		yesTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		borderOfYesOrNoTextBlock->Child = yesTextBlock;

		//6-4. decision 벡터에 넣음
		startSymbolInfo->Decision->Append(true);
	}
	else if (yesOrNoButton->Name == L"NoFlyoutButton")
	{
		TextBlock^ noTextBlock = ref new TextBlock;
		noTextBlock->Text = L"no";
		noTextBlock->HorizontalAlignment =
			Windows::UI::Xaml::HorizontalAlignment::Center;
		noTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		borderOfYesOrNoTextBlock->Child = noTextBlock;

		//6-4. decision 벡터에 넣음
		startSymbolInfo->Decision->Append(false);
	}

	//6-5. PageGrid에 넣음
	PageGrid->Children->Append(borderOfYesOrNoTextBlock);
	PageGrid->UpdateLayout();*/

	//6. 누른 버튼에 따라서 yes텍스트나 no텍스트 생성
	if (yesOrNoButton->Name == L"YesFlyoutButton")
	{
		makeYesOrNoTextBlock(connectorStartSymbolNo, focusedSymbolNo, true);

		//6-2. decision 벡터에 넣음
		startSymbolInfo->Decision->Append(true);
	}
	else if (yesOrNoButton->Name == L"NoFlyoutButton")
	{
		makeYesOrNoTextBlock(connectorStartSymbolNo, focusedSymbolNo, false);

		//6-2. decision 벡터에 넣음
		startSymbolInfo->Decision->Append(false);
	}

	//7. flyout 닫기
	YES_OR_NO_FLYOUT->Hide();

	//8. boolean false;
	isLineDrawing = false;
	isSelectingYesOrNo = false;
}

//Yes Or No TextBlock을 만드는 함수
void flowchart::GridPage::makeYesOrNoTextBlock(UINT16 from, UINT16 to, bool decision)
{
	SymbolInfo^ fromInfo = App::getSymbolInfoByNo(from);
	SymbolInfo^ toInfo = App::getSymbolInfoByNo(to);
	if (fromInfo == nullptr || toInfo == nullptr)
	{
		return;
	}

	//6-1. yes or no를 표시할 테두리 설정
	Border^ borderOfYesOrNoTextBlock = ref new Border;
	String^ nameOfTextBlockBorder = ref new String(L"decisionText ");
	nameOfTextBlockBorder += from;
	nameOfTextBlockBorder += L" to ";
	nameOfTextBlockBorder += to;
	borderOfYesOrNoTextBlock->Name = nameOfTextBlockBorder;
	borderOfYesOrNoTextBlock->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Gray);
	borderOfYesOrNoTextBlock->BorderThickness = 1;
	borderOfYesOrNoTextBlock->Width = 30;
	borderOfYesOrNoTextBlock->Height = 20;
	borderOfYesOrNoTextBlock->IsTapEnabled = false;
	borderOfYesOrNoTextBlock->SetValue(Canvas::ZIndexProperty, 3);
	borderOfYesOrNoTextBlock->SetValue(Grid::RowProperty, fromInfo->RowIndex);
	borderOfYesOrNoTextBlock->SetValue(Grid::ColumnProperty, fromInfo->ColumnIndex);

	//6-2. yes or no 위치 조정
	int direction = getDirectionTartgetSymbol(fromInfo, toInfo);
	switch (direction)
	{
	case 1:
	case 2:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, 0, (columnWidth / 2.0), 0);
		break;
	}
	case 3:
	case 4:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, 0, 0, (rowHeight / 2.0));
		break;
	}
	case 5:
	case 6:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness(0, (rowHeight / 2.0), 0, 0);
		break;
	}
	case 7:
	case 8:
	{
		borderOfYesOrNoTextBlock->Margin = Thickness((columnWidth / 2.0), 0, 0, 0);
		break;
	}
	default:
		borderOfYesOrNoTextBlock->HorizontalAlignment =
			Windows::UI::Xaml::HorizontalAlignment::Center;
		borderOfYesOrNoTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		break;
	}

	//6-3. yes or no 텍스트 생성
	if (decision)
	{
		TextBlock^ yesTextBlock = ref new TextBlock;
		yesTextBlock->Text = L"Yes";
		yesTextBlock->HorizontalAlignment =
			Windows::UI::Xaml::HorizontalAlignment::Center;
		yesTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		borderOfYesOrNoTextBlock->Child = yesTextBlock;
	}
	else
	{
		TextBlock^ noTextBlock = ref new TextBlock;
		noTextBlock->Text = L"no";
		noTextBlock->HorizontalAlignment =
			Windows::UI::Xaml::HorizontalAlignment::Center;
		noTextBlock->VerticalAlignment =
			Windows::UI::Xaml::VerticalAlignment::Center;
		borderOfYesOrNoTextBlock->Child = noTextBlock;
	}

	//6-4. PageGrid에 넣음
	PageGrid->Children->Append(borderOfYesOrNoTextBlock);
	PageGrid->UpdateLayout();
}

//연결선 삭제 확인 버튼을 눌렀을 때
void flowchart::GridPage::LineDeleteConfirmButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//델레터 네임 파싱
	UINT16 from, to;
	SymbolInfo^ fromInfo = nullptr;
	wchar_t *tappedDeletorNameWc = new wchar_t[tappedDeletorName->Length() + 1];
	wcscpy_s(tappedDeletorNameWc, tappedDeletorName->Length() + 1, tappedDeletorName->Data());
	wchar_t *tempBuf; //토큰 후 남은 글자 저장
	wchar_t *tokenedBuf; //토큰된 글자 저장

	tokenedBuf = wcstok_s(tappedDeletorNameWc, L" ", &tempBuf); //lineDeletor1 or 2

	tokenedBuf = wcstok_s(tempBuf, L" ", &tempBuf); // fromInfo->SymbolNo
	from = wcstol(tokenedBuf, NULL, 10);

	tokenedBuf = wcstok_s(tempBuf, L" ", &tempBuf); // to

	tokenedBuf = wcstok_s(tempBuf, L" ", &tempBuf); // toInfo->SymbolNo
	to = wcstol(tokenedBuf, NULL, 10);

	//선 삭제될 인포를 찾음
	fromInfo = App::getSymbolInfoByNo(from);
	//삭제될 선과 델레터 이름 생성
	String^ connectLineName = "connectLine " + from + " to " + to;
	String^ lineDeletor1Name = "lineDeletor1 " + from + " to " + to;
	String^ lineDeletor2Name = "lineDeletor2 " + from + " to " + to;
	//이름으로 찾아서 삭제
	for (int i = 0; i < PageGridCanvas->Children->Size; i++)
	{
		UIElement^ pageGridCanvasChild = PageGridCanvas->Children->GetAt(i);
		if (pageGridCanvasChild->ToString() == L"Windows.UI.Xaml.Shapes.Polyline")
		{
			Polyline^ connectLine = safe_cast<Polyline^>(pageGridCanvasChild);
			if (connectLine->Name == connectLineName)
			{
				PageGridCanvas->Children->RemoveAt(i);
				i--;
			}
		}
		else if (pageGridCanvasChild->ToString() == L"Windows.UI.Xaml.Shapes.Ellipse")
		{
			Ellipse^ lineDeletor = safe_cast<Ellipse^>(pageGridCanvasChild);
			if (lineDeletor->Name == lineDeletor1Name || lineDeletor->Name == lineDeletor2Name)
			{
				PageGridCanvas->Children->RemoveAt(i);
				i--;
			}
		}
	}
	//decision symbol이면 decisionText도 삭제
	if (fromInfo->SymbolType == 2)
	{
		String^ decisionTextblockName = "decisionText " + from + " to " + to;
		for (int i = 0; i < PageGrid->Children->Size; i++)
		{
			UIElement^ childPageGrid = PageGrid->Children->GetAt(i);
			if (childPageGrid->ToString() == L"Windows.UI.Xaml.Controls.Border")
			{
				Border^ decisionText = safe_cast<Border^>(childPageGrid);
				if (wcscmp(decisionText->Name->Data(), decisionTextblockName->Data()) == 0)
				{
					PageGrid->Children->RemoveAt(i);
					break;
				}
			}
		}
	}

	PageGridCanvas->UpdateLayout();

	//info->path에서 삭제
	if (fromInfo->SymbolType == 2)
	{
		for (int i = 0; i < fromInfo->Path->Size; i++)
		{
			if (fromInfo->Path->GetAt(i)->SymbolNo == to)
			{
				fromInfo->Path->RemoveAt(i);
				fromInfo->Decision->RemoveAt(i);
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < fromInfo->Path->Size; i++)
		{
			if (fromInfo->Path->GetAt(i)->SymbolNo == to)
			{
				fromInfo->Path->RemoveAt(i);
				break;
			}
		}
	}

	//LINEDELETOR_FLYOUT 닫음
	LINEDELETOR_FLYOUT->Hide();

	//메모리 회수
	delete[]tappedDeletorNameWc;
}

//파일오픈용 PageGrid 늘려주는 함수
void flowchart::GridPage::LoadingPageGridSize()
{
	int maxRow = nowRowNum;
	int maxColumn = nowColumnNum;
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		auto tempSymbolInfo = App::symbolVector->GetAt(i);
		maxRow = (tempSymbolInfo->RowIndex+2 > maxRow) ? tempSymbolInfo->RowIndex+2 : maxRow;
		maxColumn = (tempSymbolInfo->ColumnIndex+2 > maxColumn) ? tempSymbolInfo->ColumnIndex+2 : maxColumn;
	}

	while (maxRow > nowRowNum)
	{
		appendRow();
	}
	while (maxColumn > nowColumnNum)
	{
		appendColumn();
	}
}

void flowchart::GridPage::LineDeletor_Tapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	auto deletor = safe_cast<Ellipse^>(sender);
	//델레터 지정
	tappedDeletorName = deletor->Name;
	String^ deletorNameDebug = deletor->Name;
	deletorNameDebug += "\n";
	OutputDebugString(deletorNameDebug->Data());

	//open LINEDELETOR_FLYOUT
	LINEDELETOR_FLYOUT->ShowAt(deletor);
}