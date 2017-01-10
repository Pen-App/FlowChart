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
using namespace Windows::UI::ViewManagement;	// ApplicationView 크기 관리
using namespace Windows::Data::Xml::Dom;	// 파일 입출력
using namespace Windows::Storage::Pickers;	// 선택기 사용 파일저장

// 빈 페이지 항목 템플릿은 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 에 문서화되어 있습니다.

MainPage::MainPage()
{
	InitializeComponent();
	ApplicationView::PreferredLaunchViewSize = Size(1000, 700);
	ApplicationView::PreferredLaunchWindowingMode = ApplicationViewWindowingMode::PreferredLaunchViewSize;
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

		Page^ varGridPage = (Page^)(GridContentFrame->Content);
		Grid^ varPageGrid = (Grid^)(varGridPage->FindName("PageGrid"));

		tempButton1 = safe_cast<Button^>(varPageGrid->FindName("b1 " + App::draggingSymbolNo));
		tempButton2 = safe_cast<Button^>(varPageGrid->FindName("b2 " + App::draggingSymbolNo));
		tempButton3 = safe_cast<Button^>(varPageGrid->FindName("b3 " + App::draggingSymbolNo));
		tempImage = safe_cast<Image^>(varPageGrid->FindName("i" + App::draggingSymbolType + " " + App::draggingSymbolNo));

		UIElement^ UIButton1 = safe_cast<UIElement^>(varPageGrid->FindName("b1 " + App::draggingSymbolNo));
		UIElement^ UIButton2 = safe_cast<UIElement^>(varPageGrid->FindName("b2 " + App::draggingSymbolNo));
		UIElement^ UIButton3 = safe_cast<UIElement^>(varPageGrid->FindName("b3 " + App::draggingSymbolNo));
		UIElement^ UIImage = safe_cast<UIElement^>(varPageGrid->FindName("i" + App::draggingSymbolType + " " + App::draggingSymbolNo));

		for (int i = 0; i < varPageGrid->Children->Size; i++)
		{			
			if (varPageGrid->Children->GetAt(i) == UIImage)
			{
				for (int j = 0; j < 8; j++) {
					varPageGrid->Children->RemoveAt(i);
				}
				break;
			}
		}
		varPageGrid->UpdateLayout();

		App::symbolVector->RemoveAt(App::focusedSymbolIndex);
	}
}


void flowchart::MainPage::ListBox_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
	e->DragUIOverride->IsGlyphVisible = false; //복사, 이동, 금지 아이콘 띄우지 말기
	e->DragUIOverride->Caption = L"Delete";
}


void flowchart::MainPage::ZoomInButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Page^ varGridPage = (Page^)(GridContentFrame->Content);
	ScrollViewer^ varScrollViewer = (ScrollViewer^)(varGridPage->FindName("PageGridScrollViewer"));
	varScrollViewer->ZoomToFactor((varScrollViewer->ZoomFactor + 0.1));
	varScrollViewer->UpdateLayout();
}

void flowchart::MainPage::ZoomOutButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Page^ varGridPage = (Page^)(GridContentFrame->Content);
	ScrollViewer^ varScrollViewer = (ScrollViewer^)(varGridPage->FindName("PageGridScrollViewer"));
	varScrollViewer->ZoomToFactor((varScrollViewer->ZoomFactor - 0.1));
	varScrollViewer->UpdateLayout();
}

// 파일 오픈
void flowchart::MainPage::OpenFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

// 파일 저장
void flowchart::MainPage::SaveFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	XmlDocument^ xmlDocument = ref new XmlDocument;	// xml 문서형식 사용
	XmlElement^ rootElement = xmlDocument->CreateElement(FileName->Text);	// root 항목 설정
	xmlDocument->AppendChild(rootElement);	// 이항목이 xmlDocument의 root항목이 될수 있게 추가
}
