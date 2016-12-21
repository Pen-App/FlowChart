//
// MainPage.xaml.cpp
// MainPage 클래스의 구현입니다.
//

#include "pch.h"
#include "MainPage.xaml.h"
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

// 빈 페이지 항목 템플릿은 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 에 문서화되어 있습니다.

MainPage::MainPage()
{
	InitializeComponent();
	this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridContent::typeid));
}

void MainPage::Image_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{

}

void flowchart::MainPage::ScrollViewer_PointerWheelChanged(
	Platform::Object^ sender, 
	Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (e->KeyModifiers == Windows::System::VirtualKeyModifiers::Control) 
	{
		int direction = e->
			GetCurrentPoint((UIElement^)sender)->
			Properties->
			MouseWheelDelta;
		auto scrollView = ((ScrollViewer^)sender);
		float zoomFactor = scrollView->ZoomFactor;
		wchar_t debugBuf[256];

		if (direction >= 0) //줌 인
		{
			scrollView->ZoomToFactor(zoomFactor - 0.1);
			swprintf_s(debugBuf, L"%.2f\n", scrollView->ZoomFactor);
			OutputDebugString(debugBuf);
		}
		else //줌 아웃
		{
			scrollView->ZoomToFactor(zoomFactor + 0.1);
			swprintf_s(debugBuf, L"%.2f\n", scrollView->ZoomFactor);
			OutputDebugString(debugBuf);
		}
	}
}
