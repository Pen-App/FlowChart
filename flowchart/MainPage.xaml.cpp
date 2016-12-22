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
//이미지 드롭 후 이벤트 처리 메소드 : 드롭이 되면 selectedSymbolNumber가 다시 0이 된다. => 아무것도 선택된게 없다는 뜻
void flowchart::MainPage::Image_DropCompleted(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args)
{
	App::selectedSymbolNumber = -1;
}

void flowchart::MainPage::ScrollViewer_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (e->KeyModifiers == Windows::System::VirtualKeyModifiers::Control)
	{
		int delta = e->
					GetCurrentPoint((UIElement^)sender)->
					Properties->
					MouseWheelDelta;
		float curZoomFactor = ((ScrollViewer^)sender)->ZoomFactor;
		if (delta >= 0)
		{
			((ScrollViewer^)sender)->ZoomToFactor(curZoomFactor + 0.1);
		}
		else
		{
			((ScrollViewer^)sender)->ZoomToFactor(curZoomFactor - 0.1);
		}
	}
}
