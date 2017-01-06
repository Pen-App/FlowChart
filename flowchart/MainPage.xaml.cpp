//
// MainPage.xaml.cpp
// MainPage 클래스의 구현입니다.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "SymbolInfo.h"
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

// 빈 페이지 항목 템플릿은 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 에 문서화되어 있습니다.

MainPage::MainPage()
{
	InitializeComponent();
	this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid), "s0");
}

//이미지 이벤트 처리 메소드 : 이미지가 클릭되면 어떤 도형이 선택되었는지 selectedSymbolNumber에 저장된다. 
void flowchart::MainPage::Image_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{
	auto sendersName = ((Image^)sender)->Name;
	App::selectedSymbolNumber = std::stoi(sendersName->Data() + 1);
		 

}
//이미지 드롭 후 이벤트 처리 메소드 : 드롭이 되면 selectedSymbolNumber가 다시 -1이 된다. => 아무것도 선택된게 없다는 뜻
void flowchart::MainPage::Image_DropCompleted(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args)
{
	App::selectedSymbolNumber = -1;
}

void flowchart::MainPage::ListBox_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	OutputDebugString(L"ListBox_Drop\n");

	wchar_t asdf[234];
	swprintf_s(asdf, L"draggingSymbolNo : %d\n", App::draggingSymbolNo);
	OutputDebugString(asdf);

	if (App::draggingSymbolNo != -1)
	{
		Button^ tempButton1 = nullptr;
		Button^ tempButton2 = nullptr;
		Button^ tempButton3 = nullptr;
		Image^ tempImage = nullptr;

		Page^ GridPage = (Page^)(GridContentFrame->Content);
		Grid^ PageGrid = (Grid^)(GridPage->FindName("PageGrid"));

		tempButton1 = safe_cast<Button^>(PageGrid->FindName("b1 " + App::draggingSymbolNo));
		tempButton2 = safe_cast<Button^>(PageGrid->FindName("b2 " + App::draggingSymbolNo));
		tempButton3 = safe_cast<Button^>(PageGrid->FindName("b3 " + App::draggingSymbolNo));
		tempImage = safe_cast<Image^>(PageGrid->FindName("i" + App::draggingSymbolType + " " + App::draggingSymbolNo));

		UIElement^ UIButton1 = safe_cast<UIElement^>(PageGrid->FindName("b1 " + App::draggingSymbolNo));
		UIElement^ UIButton2 = safe_cast<UIElement^>(PageGrid->FindName("b2 " + App::draggingSymbolNo));
		UIElement^ UIButton3 = safe_cast<UIElement^>(PageGrid->FindName("b3 " + App::draggingSymbolNo));
		UIElement^ UIImage = safe_cast<UIElement^>(PageGrid->FindName("i" + App::draggingSymbolType + " " + App::draggingSymbolNo));

		for (int i = 0; i < PageGrid->Children->Size; i++)
		{			
			if (PageGrid->Children->GetAt(i) == UIImage)
			{
				for (int j = 0; j < 5; j++) {
					PageGrid->Children->RemoveAt(i);
				}
				break;
			}
		}
		PageGrid->UpdateLayout();
		
		//for (int i = 0; i < App::symbolVector->Size; i++)
		//{
		//	if (App::symbolVector->GetAt(i)->SymbolNo == App::draggingSymbolNo)
		//	{
		//		App::symbolVector->RemoveAt(i);
		//	}
		//}
		App::symbolVector->RemoveAt(App::focusedSymbolIndex);
	}
}


void flowchart::MainPage::ListBox_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
	e->DragUIOverride->IsGlyphVisible = false; //복사, 이동, 금지 아이콘 띄우지 말기
	e->DragUIOverride->Caption = L"Delete";
}
