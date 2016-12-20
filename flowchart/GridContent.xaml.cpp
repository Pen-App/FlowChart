//
// GridContent.xaml.cpp
// Implementation of the GridContent class
//

#include "pch.h"
#include "GridContent.xaml.h"
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
// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

GridContent::GridContent()
{
	InitializeComponent();

	nowRowNum = 10;
	nowColumnNum = 10;
	columnWidth = 100;
	rowHeight = 70;

	makeGridArray(nowRowNum, nowColumnNum, columnWidth, rowHeight);
}

void flowchart::GridContent::makeButton(Grid^ parentGrid, 
	int horizontal, int vertical, int width, int height, int rowIdx, int columnIdx) 
{
	Button^ tempButton = ref new Button;
	tempButton->Style = BUTTON_STYLE_ARROW;
	tempButton->Width = width;
	tempButton->Height = height;
	tempButton->SetValue(PageGrid->ColumnProperty, columnIdx);
	tempButton->SetValue(PageGrid->RowProperty, rowIdx);
	tempButton->SetValue(HorizontalAlignmentProperty, horizontal);
	tempButton->SetValue(VerticalAlignmentProperty, vertical);
	parentGrid->Children->Append(tempButton);
}

void flowchart::GridContent::makeRectangle(Grid^ parentGrid, 
	int width, int height, int rowIdx, int columnIdx) 
{
	Rectangle^ tempRect = ref new Rectangle();
	tempRect->Height = height;
	tempRect->Width = width;
	tempRect->Style = RECTANGLE_STYLE;
	auto dasharray = ref new DoubleCollection;
	dasharray->Append(5);
	tempRect->StrokeDashArray = dasharray;
	tempRect->SetValue(PageGrid->ColumnProperty, columnIdx);
	tempRect->SetValue(PageGrid->RowProperty, rowIdx);
	tempRect->AllowDrop = true;
	tempRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &flowchart::GridContent::Rectangle_DragEnter);
	parentGrid->Children->Append(tempRect);
}

void flowchart::GridContent::makeGridArray(int rowNum, int columnNum, 
	int rectWidth, int rectHeight) 
{
	for (int i = 0; i < rowNum; i++) 
	{
		RowDefinition^ tempRow = ref new RowDefinition();
		tempRow->Height = rectHeight;
		PageGrid->RowDefinitions->Append(tempRow);
	}

	for (int i = 0; i < columnNum; i++) 
	{
		ColumnDefinition^ tempColumn = ref new ColumnDefinition();
		tempColumn->Width = rectWidth;
		PageGrid->ColumnDefinitions->Append(tempColumn);
	}

	for (int i = 0; i < rowNum; i++) 
	{
		for (int j = 0; j < columnNum; j++) 
		{
			makeRectangle(PageGrid, rectWidth, rectHeight, i, j);
		}
	}
}

void flowchart::GridContent::makeImage(Grid^ parentGrid, int rowIdx, int columnIdx) 
{
	Image^ tempImage = ref new Image();
	tempImage->Style = TEST_IMAGE;
	tempImage->SetValue(Grid::ColumnProperty, columnIdx);
	tempImage->SetValue(Grid::RowProperty, rowIdx);
	parentGrid->Children->Append(tempImage);
}

void flowchart::GridContent::appendRow() 
{
	RowDefinition^ tempRowDef = ref new RowDefinition;
	tempRowDef->Height = rowHeight;
	PageGrid->RowDefinitions->Append(tempRowDef);
	nowRowNum++;

	for (int i = 0; i < nowColumnNum; i++)
	{
		makeRectangle(PageGrid, columnWidth, rowHeight, nowRowNum - 1, i);
	}
}

void flowchart::GridContent::appendColumn()
{
	ColumnDefinition^ tempColDef = ref new ColumnDefinition;
	tempColDef->Width = columnWidth;
	PageGrid->ColumnDefinitions->Append(tempColDef);
	nowColumnNum++;

	for (int i = 0; i < nowRowNum; i++)
	{
		makeRectangle(PageGrid, columnWidth, rowHeight, i, nowColumnNum - 1);
	}
}

void flowchart::GridContent::appendTopRow()
{
	appendRow();

	wchar_t debugBuf[256];
	for (int i = 0; i < PageGrid->Children->Size; i++)
	{
		UIElement^ symbolInGridContent = PageGrid->Children->GetAt(i);

		swprintf_s(debugBuf, L"%s", symbolInGridContent->ToString()->Data());

		if (wcscmp(debugBuf, L"Windows.UI.Xaml.Shapes.Rectangle")) //심볼일 때
		{
			Object^ rowPropertyValueObject = 
				symbolInGridContent->GetValue(Grid::RowProperty);
			int rowPropertyValueInt = safe_cast<int>(rowPropertyValueObject);
			symbolInGridContent->SetValue(Grid::RowProperty, rowPropertyValueInt + 1);
		}
	}
}

void flowchart::GridContent::appendLeftColumn()
{
	appendColumn();

	wchar_t debugBuf[256];
	for (int i = 0; i < PageGrid->Children->Size; i++)
	{
		UIElement^ symbolInGridContent = PageGrid->Children->GetAt(i);

		swprintf_s(debugBuf, L"%s", symbolInGridContent->ToString()->Data());

		if (wcscmp(debugBuf, L"Windows.UI.Xaml.Shapes.Rectangle")) //심볼일 때
		{
			Object^ columnPropertyValueObject =
				symbolInGridContent->GetValue(Grid::ColumnProperty);
			int columnPropertyValueInt = safe_cast<int>(columnPropertyValueObject);
			symbolInGridContent->SetValue(Grid::ColumnProperty, columnPropertyValueInt + 1);
		}
	}
}

void flowchart::GridContent::PageGrid_DragOver(Platform::Object ^ sender, 
	Windows::UI::Xaml::DragEventArgs ^ e)
{
	e->DragUIOverride->Caption = "여기 놓으세요.";
	e->DragUIOverride->IsGlyphVisible = false;
	e->AcceptedOperation = DataPackageOperation::Copy;
}

void flowchart::GridContent::PageGrid_Drop(Platform::Object^ sender, 
	Windows::UI::Xaml::DragEventArgs^ e) 
{
	makeButton(PageGrid, 2, 0, 20, 20, curRowIdx, curColumnIdx);
	makeButton(PageGrid, 2, 2, 20, 20, curRowIdx, curColumnIdx);
	makeImage(PageGrid, curRowIdx, curColumnIdx);

	if (curColumnIdx == 0)
	{
		appendLeftColumn();
	}

	if (curRowIdx == 0)
	{
		appendTopRow();
	}

	if (curColumnIdx + 1 == nowColumnNum) 
	{
		appendColumn();
	}

	if (curRowIdx + 1 == nowRowNum) {
		appendRow();
	}

	PageGrid->UpdateLayout();
}

void flowchart::GridContent::Rectangle_DragEnter(Platform::Object^ sender, 
	Windows::UI::Xaml::DragEventArgs^ e) 
{
	auto columnIdx = ((Rectangle^)sender)->GetValue(PageGrid->ColumnProperty);
	auto rowIdx = ((Rectangle^)sender)->GetValue(PageGrid->RowProperty);

	curColumnIdx = safe_cast<int>(columnIdx);
	curRowIdx = safe_cast<int>(rowIdx);
}

void flowchart::GridContent::PageGrid_SizeChanged(Platform::Object^ sender, 
	Windows::UI::Xaml::SizeChangedEventArgs^ e)
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