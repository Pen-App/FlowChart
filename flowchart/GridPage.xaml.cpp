//
// GridPage.xaml.cpp
// Implementation of the GridPage class
//

#include "pch.h"
#include "GridPage.xaml.h"

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

GridPage::GridPage()
{
	InitializeComponent();

	nowColumnNum = 10;
	nowRowNum = 10;
	columnWidth = 100;
	rowHeight = 70;
	makeGridArray(PageGrid, nowRowNum, nowColumnNum, rowHeight, columnWidth);
}
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
	tempRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridPage::Rectangle_DragEnter);
	parentGrid->Children->Append(tempRect);
}
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
void GridPage::makeImage(Grid^ parentGrid, int symbolType, int rowIndex, int columnIndex)
{
	if (symbolType == -1)
		return;

	Image^ tempImage = ref new Image();
	tempImage->SetValue(parentGrid->RowProperty, curRowIndex);
	tempImage->SetValue(parentGrid->ColumnProperty, curColumnIndex);
	switch (symbolType) {
	case 1:
		tempImage->Style = IMAGE_PROCESS;
		break;
	case 2:
		tempImage->Style = IMAGE_DECISION;
		break;
	case 3:
		tempImage->Style = IMAGE_PREPARATION;
		break;
	case 4:
		tempImage->Style = IMAGE_TERMINATOR;
		break;
	case 5:
		tempImage->Style = IMAGE_DATA;
		break;
	case 6:
		tempImage->Style = IMAGE_DOCUMENT;
		break;
	}
	parentGrid->Children->Append(tempImage);
	//parentGrid->UpdateLayout();
}
void GridPage::makeButton(Grid^ parentGrid, int buttonType, int rowIndex, int columnIndex)
{
	int horizontal, vertical;

	Button^ tempButton = ref new Button();
	tempButton->SetValue(parentGrid->ColumnProperty, columnIndex);
	tempButton->SetValue(parentGrid->RowProperty, rowIndex);

	switch (buttonType) {
	case 1:
		tempButton->Style = BUTTON_STYLE_CONNECTOR;
		horizontal = 2;
		vertical = 2;
		break;
	case 2:
		tempButton->Style = BUTTON_STYLE_CONTENTS;
		horizontal = 0;
		vertical = 2;
		break;

	case 3:
		tempButton->Style = BUTTON_STYLE_DETAIL;
		horizontal = 2;
		vertical = 0;
		break;
	}
	tempButton->SetValue(HorizontalAlignmentProperty, horizontal);
	tempButton->SetValue(VerticalAlignmentProperty, vertical);
	parentGrid->Children->Append(tempButton);
}
void GridPage::makeButtons(Grid^ parentGrid, int rowIndex, int columnIndex)
{
	makeButton(parentGrid, 1, rowIndex, columnIndex);
	makeButton(parentGrid, 2, rowIndex, columnIndex);
	makeButton(parentGrid, 3, rowIndex, columnIndex);

}

void GridPage::refreshGridPage(Grid^ parentGrid) 
{
	parentGrid->Children->Clear();
	makeGridArray(PageGrid, 10, 10, 70, 100);

	//1. map 순회
	for (int i = 0; i < App::symbolVector->Size; i++) 
	{
		makeImage(parentGrid, App::symbolVector->GetAt(i)->SymbolType, App::symbolVector->GetAt(i)->RowIndex, App::symbolVector->GetAt(i)->ColumnIndex);
		makeButtons(PageGrid, App::symbolVector->GetAt(i)->RowIndex, App::symbolVector->GetAt(i)->ColumnIndex);
	}
	parentGrid->UpdateLayout();
}

void GridPage::appendRow()
{
	RowDefinition^ tempRowDef = ref new RowDefinition;
	tempRowDef->Height = rowHeight;
	PageGrid->RowDefinitions->Append(tempRowDef);
	nowRowNum++;

	for (int i = 0; i < nowColumnNum; i++)
	{
		makeRectangle(PageGrid, nowRowNum - 1, i);
	}
}

void GridPage::appendColumn()
{
	ColumnDefinition^ tempColDef = ref new ColumnDefinition;
	tempColDef->Width = columnWidth;
	PageGrid->ColumnDefinitions->Append(tempColDef);
	nowColumnNum++;

	for (int i = 0; i < nowRowNum; i++)
	{
		makeRectangle(PageGrid, i, nowColumnNum - 1);
	}
}

void GridPage::appendTopRow()
{
	appendRow();

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
	appendColumn();

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

void flowchart::GridPage::PageGrid_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	e->AcceptedOperation = DataPackageOperation::Move;
	e->DragUIOverride->Caption = "추가";
	e->DragUIOverride->IsGlyphVisible = true;
}


void flowchart::GridPage::PageGrid_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	//1. map에 새로운 symbolInfo 객체를 추가시킨다. 
	SymbolInfo^ tempSymbolInfo = ref new SymbolInfo();
	tempSymbolInfo->SymbolType = App::selectedSymbolNumber;
	tempSymbolInfo->RowIndex = curRowIndex;
	tempSymbolInfo->ColumnIndex = curColumnIndex;
	tempSymbolInfo->SymbolNo = App::symbolIdCount++;
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

	//refreshGridPage(PageGrid);
	testTextBox->Text = "" + App::symbolVector->Size;
	makeImage(PageGrid, App::selectedSymbolNumber, curRowIndex, curColumnIndex);
	makeButtons(PageGrid, curRowIndex, curColumnIndex);
	
}


void flowchart::GridPage::Rectangle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);
}


void flowchart::GridPage::Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curRowIndex = safe_cast<int>(rowIdx);
	curColumnIndex = safe_cast<int>(columnIdx);
}


void flowchart::GridPage::PageGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	int resizedPageGridHeight = (int)(PageGrid->ActualHeight);
	int resizedPageGridWidth = (int)(PageGrid->ActualWidth);

	int resizedRowNum = (resizedPageGridHeight / rowHeight)
		+
		((resizedPageGridHeight%rowHeight) ? 1 : 0);
	int resizedColumnNum = (resizedPageGridWidth / columnWidth)
		+
		((resizedPageGridWidth%columnWidth) ? 1 : 0);
	while (resizedRowNum > nowRowNum)
	{
		appendRow();
	}
	while (resizedColumnNum > nowColumnNum)
	{
		appendColumn();
	}
}
