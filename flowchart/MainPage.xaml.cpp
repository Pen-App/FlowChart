//
// MainPage.xaml.cpp
// MainPage 클래스의 구현입니다.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "SymbolInfo.h"
#include "GridPage.xaml.h"
#include "UndoHistoryDialog.xaml.h"
#include "RedoHistoryDialog.xaml.h"

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
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;	// 선택기 사용 파일저장
using namespace Concurrency;

using namespace Windows::UI::Popups;	// 테스트용

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

//심볼 리스트박스에 그리드에 들어있던 심볼을 드래그&드롭해서 삭제
void flowchart::MainPage::ListBox_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	wchar_t asdf[234];
	swprintf_s(asdf, L"draggingSymbolNo : %d\n", App::draggingSymbolNo);
	OutputDebugString(asdf);

	if (App::draggingSymbolNo != -1)
	{
		/*Button^ tempButton1 = nullptr;
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

		deleteConnectLine(App::draggingSymbolNo);

		varPageGrid->UpdateLayout();*/

		for (int i = 0; i < App::symbolVector->Size; i++)
		{
			auto tempSymbolInfo = App::symbolVector->GetAt(i);
			if (tempSymbolInfo->SymbolNo == App::draggingSymbolNo)
			{
				continue;
			}

			if (tempSymbolInfo->SymbolType == 2)
			{
				for (int j = 0; j < tempSymbolInfo->Path->Size; j++)
				{
					auto tempConnectSymbolInfo = tempSymbolInfo->Path->GetAt(j);
					if (tempConnectSymbolInfo->SymbolNo == App::draggingSymbolNo)
					{
						tempSymbolInfo->Path->RemoveAt(j);
						tempSymbolInfo->Decision->RemoveAt(j); 
						break;
					}
				}
			}
			else
			{
				for (int j = 0; j < tempSymbolInfo->Path->Size; j++)
				{
					auto tempConnectSymbolInfo = tempSymbolInfo->Path->GetAt(j);
					if (tempConnectSymbolInfo->SymbolNo == App::draggingSymbolNo)
					{
						tempSymbolInfo->Path->RemoveAt(j);
						break;
					}
				}
			}
		}
		
		for (int i = 0; i < App::symbolVector->Size; i++)
		{
			if (App::symbolVector->GetAt(i)->SymbolNo == App::draggingSymbolNo)
			{
				App::symbolVector->RemoveAt(i);
				break;
			}
		}

		App::historyObject->putHistory();

		this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid), "s0");
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
	//rootCommandBar->IsOpen = true;
	//rootCommandBar->IsSticky = true;
}

void flowchart::MainPage::ZoomOutButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Page^ varGridPage = (Page^)(GridContentFrame->Content);
	ScrollViewer^ varScrollViewer = (ScrollViewer^)(varGridPage->FindName("PageGridScrollViewer"));
	varScrollViewer->ZoomToFactor((varScrollViewer->ZoomFactor - 0.1));
	varScrollViewer->UpdateLayout();
	//rootCommandBar->IsOpen = true;
}

// 파일 오픈
void flowchart::MainPage::OpenFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// FileOpenPicker 객체 생성
	FileOpenPicker^ openPicker = ref new FileOpenPicker();

	// 탐색 시장 경로 지정
	openPicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

	// PickerViewMode::List - 파일을 리스트 형태로 보여줌
	// PickerViewMode::Thumnail - 파일을 섬네일 이미지 형태로 보여줌
	// 명시해주지 않으면 기본 ViewMode인 PickerViewMode::List로 보여줌
	//openPicker->ViewMode = PickerViewMode::Thumbnail;

	// 보여줄 파일 종류 설정 (모든 경로일 경우 Append("*"))
	openPicker->FileTypeFilter->Append(".xml");

	// FileOpenPicker 실행 - PickSingleFileAsync 비동기 함수를 사용 => 한꺼번에 한개의 파일만 선택
	task<StorageFile ^>(openPicker->PickSingleFileAsync()).then([this](StorageFile ^file) 
	{
		OpenFileContent(file);
	});
}

// 파일 열기
void flowchart::MainPage::OpenFileContent(Windows::Storage::StorageFile^ file)
{
	if (file != nullptr)
	{
		OpenFileInit();
		// file->Name에 선택한 파일의 이름이 저장되어 있습니다.
		create_task(XmlDocument::LoadFromFileAsync(file)).then([this](XmlDocument^ xmlDocument)
		{
			OpenFileContentXmlParser(xmlDocument);
			this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid), "s0");
		});
	}
	else
	{
		// "취소 버튼을 누른 경우"
	}
}

// 파일을 열기전 프로그램에 남아있던 정보 초기화
void flowchart::MainPage::OpenFileInit()
{
	// symbolVector 초기화
	App::symbolVector->Clear();
	App::symbolIdCount = 0;
	App::selectedSymbolNumber = -1;
	App::draggingSymbolNo = -1;
	App::draggingSymbolType = -1;
	App::focusedSymbolIndex = -1;	// 선택된 symbolInfoVector의 index

	FileName->Text = "";
}

// 열은 파일을 xml 파싱해 저장
void flowchart::MainPage::OpenFileContentXmlParser(Windows::Data::Xml::Dom::XmlDocument^ xmlDocument)
{
	XmlNodeList ^items = xmlDocument->SelectNodes("file");
	// document에서 해당 노드를 검색하여 노드가 존재하는 경우
	if (items != nullptr)
	{
		// Root 노드를 얻는다
		IXmlNode^ rootNode = items->GetAt(0);

		// 파일명을 얻는다
		FileName->Text = (String^)rootNode->Attributes->GetNamedItem("fileName")->NodeValue;

		XmlNodeList^ symbolList = rootNode->ChildNodes;
		//Vector<int>^ tmpPathNo = nullptr;
		//Vector<bool>^ tmpDecision = nullptr;
		Vector<SymbolInfo^>^ tmpPath = ref new Vector<SymbolInfo^>();
		for (int i = 0; i < symbolList->Size; i++)
		{
			SymbolInfo^ symbolInfo = ref new SymbolInfo();
			IXmlNode^ symbolNode = symbolList->GetAt(i);

			// 'title'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ titleNode = symbolNode->Attributes->GetNamedItem("title");
			if (titleNode != nullptr)
				symbolInfo->Title = (String^)titleNode->NodeValue;

			// 'detail'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ detailNode = symbolNode->Attributes->GetNamedItem("detail");
			if (detailNode != nullptr)
				symbolInfo->Detail = (String^)detailNode->NodeValue;

			// 'content'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ contentNode = symbolNode->Attributes->GetNamedItem("content");
			if (contentNode != nullptr)
				symbolInfo->Content = (String^)contentNode->NodeValue;

			// 'symbolType'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ symbolTypeNode = symbolNode->Attributes->GetNamedItem("symbolType");
			if (symbolTypeNode != nullptr)
				symbolInfo->SymbolType = _wtof(((String^)symbolTypeNode->NodeValue)->Data());

			// 'columnIndex'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ columnIndexNode = symbolNode->Attributes->GetNamedItem("columnIndex");
			if (columnIndexNode != nullptr)
				symbolInfo->ColumnIndex = _wtof(((String^)columnIndexNode->NodeValue)->Data());

			// 'rowIndex'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ rowIndexNode = symbolNode->Attributes->GetNamedItem("rowIndex");
			if (rowIndexNode != nullptr)
				symbolInfo->RowIndex = _wtof(((String^)rowIndexNode->NodeValue)->Data());

			// 'no'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ noNode = symbolNode->Attributes->GetNamedItem("no");
			if (noNode != nullptr)
				symbolInfo->SymbolNo = _wtof(((String^)noNode->NodeValue)->Data());

			// 'path'의 이름을 가진 아이템 노드를 반환 & vector에 저장
			IXmlNode^ pathNode = symbolNode->ChildNodes->GetAt(0);
			SymbolInfo^ pathVector = ref new SymbolInfo();
			switch (symbolInfo->SymbolType)
			{
			case 2:	// symbolType이 decision일때
				for (int j = 0; j < pathNode->Attributes->Size; j++)
				{
					SymbolInfo^ pathNo = ref new SymbolInfo();	// path의 no만 들어갈 공간
					IXmlNode^ pathSymbolNode = pathNode->Attributes->GetNamedItem("no" + j);
					IXmlNode^ decisionNode = pathNode->Attributes->GetNamedItem("decision" + j);
					if (pathSymbolNode == nullptr || decisionNode == nullptr)	// path에 정보가 없다면 for문을 빠져나감
						break;
					pathNo->SymbolNo = _wtof(((String^)pathSymbolNode->NodeValue)->Data());
					pathVector->Path->Append(pathNo);
					// path의 no들 저장
					//pathNo->SymbolNo = _wtof(((String^)pathSymbolNode->NodeValue)->Data());
					//pathSymbolInfo->Path->Append(pathNo);
					// path의 decision들 저장
					symbolInfo->Decision->Append(_wtof(((String^)decisionNode->NodeValue)->Data()));
					//pathSymbolInfo->Decision->Append(_wtof(((String^)decisionNode->NodeValue)->Data());
				}
				break;
			default:
				for (int j = 0; j < pathNode->Attributes->Size; j++)
				{
					SymbolInfo^ pathNo = ref new SymbolInfo();	// path의 no만 들어갈 공간
					IXmlNode^ pathSymbolNode = pathNode->Attributes->GetNamedItem("no" + j);
					if (pathSymbolNode == nullptr)	// path에 정보가 없다면 for문을 빠져나감
						break;
					// path의 no들 저장
					pathNo->SymbolNo = _wtof(((String^)pathSymbolNode->NodeValue)->Data());
					pathVector->Path->Append(pathNo);
					//tmpPathNo->Append(_wtof(((String^)pathSymbolNode->NodeValue)->Data()));
					//pathSymbolInfo->SymbolNo = _wtof(((String^)pathSymbolNode->NodeValue)->Data());
					//symbolInfo->Path->Append(pathSymbolInfo);
				}
				break;
			}
			tmpPath->Append(pathVector);
			//tmpSymbol->Append(pathSymbolInfo);	// path정보
			App::symbolVector->Append(symbolInfo);
		}
		// path 정리
		SetPathList(tmpPath);
		App::historyObject->initHistory();
		MessageDialog^ msg = ref new MessageDialog("file open success!" + "\n vector size = " + App::symbolVector->Size);
		msg->ShowAsync();
	}
}
void flowchart::MainPage::SetPathList(Vector<SymbolInfo^>^ tmpPath)
{
	//MessageDialog^ msg = ref new MessageDialog(App::symbolVector->Size + ", " + tmpPath->Size);
	//msg->ShowAsync();
	OutputDebugStringA("My output string.\n");
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		for (int j = 0; j < tmpPath->GetAt(i)->Path->Size; j++)
		{
			int cnt = 0;
			int pathNo = tmpPath->GetAt(i)->Path->GetAt(j)->SymbolNo;
			while (1)
			{
				SymbolInfo^ tmpSymbol = App::symbolVector->GetAt(cnt);
				if (pathNo == tmpSymbol->SymbolNo)
				{
					App::symbolVector->GetAt(i)->Path->Append(tmpSymbol);
					break;
				}
				cnt++;
			}
		}
	}
}

// 파일 저장 클릭시
void flowchart::MainPage::SaveFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (App::symbolVector->Size <= 0)	// 추가한 도형이 없을 경우
	{
		return;
	}

	SaveFileInit();

	// 저장할 파일 경로 및 파일형식 설정
	FileSavePicker^ savePicker = ref new FileSavePicker();
	savePicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

	auto fileType = ref new Vector<String^>();
	fileType->Append(".xml");
	fileType->Append(".pap");
	savePicker->FileTypeChoices->Insert("PenApp Files", fileType);

	savePicker->DefaultFileExtension = ".xaml";
	savePicker->SuggestedFileName = FileName->Text;
	FileName->Text = savePicker->SuggestedFileName;

	// 선택한 경로로 파일 저장
	task<StorageFile ^>(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
	{
		SaveFileContent(file);
	});
	
}

// 파일 저장 전 초기화
void flowchart::MainPage::SaveFileInit()
{
	Image^ tmpImg = nullptr;
	SymbolInfo^ symbolInfo = nullptr;

	// row, column을 찾을 pageGrid 찾기
	Page^ gridContent = (Page^)(GridContentFrame->Content);
	Grid^ pageGrid = (Grid^)(gridContent->FindName("PageGrid"));

	// 저장하기전 바뀌었던 column, row Index들을 다시 vector에 갱신해줌
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		symbolInfo = App::symbolVector->GetAt(i);
		tmpImg = safe_cast<Image^>(pageGrid->FindName("i" + symbolInfo->SymbolType
			+ " " + symbolInfo->SymbolNo));
		symbolInfo->RowIndex = safe_cast<int>(tmpImg->GetValue(pageGrid->RowProperty));
		symbolInfo->ColumnIndex = safe_cast<int>(tmpImg->GetValue(pageGrid->ColumnProperty));
	}
}

// new File
void flowchart::MainPage::NewFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	OpenFileInit();
	App::historyObject->initHistory();
	this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid), "s0");
}

// xml 파싱해서 파일 저장
void flowchart::MainPage::SaveFileContent(StorageFile^ file)
{
	if (file != nullptr)
	{
		XmlDocument^ xmlDocument = ref new XmlDocument;	// xml 문서형식 사용
		XmlElement^ rootElement = xmlDocument->CreateElement("file");	// root 항목 설정
		std::wstring s(file->Name->Data());
		std::wstring replacement(L"");
		s = s.replace(s.size()-4, 4, replacement);
		String^ tmpStr = ref new String(s.c_str());
		rootElement->SetAttribute("fileName", tmpStr);
		xmlDocument->AppendChild(rootElement);	// 이항목이 xmlDocument의 root항목이 될수 있게 추가

		for (int i = 0; i < App::symbolVector->Size; i++)
		{
			XmlElement^ xmlElement = xmlDocument->CreateElement("symbol");
			SymbolInfo^ symbolInfo = App::symbolVector->GetAt(i);
			xmlElement->SetAttribute("no", symbolInfo->SymbolNo + "");
			xmlElement->SetAttribute("rowIndex", symbolInfo->RowIndex + "");
			xmlElement->SetAttribute("columnIndex", symbolInfo->ColumnIndex + "");
			xmlElement->SetAttribute("symbolType", symbolInfo->SymbolType + "");
			xmlElement->SetAttribute("title", symbolInfo->Title);
			xmlElement->SetAttribute("content", symbolInfo->Content);
			xmlElement->SetAttribute("detail", symbolInfo->Detail);
			rootElement->AppendChild(xmlElement);

			XmlElement^ pathElement = xmlDocument->CreateElement("path");
			if (symbolInfo->Path->Size != 0) {
				switch (symbolInfo->SymbolType)
				{
				case 2:
					for (int j = 0; j < symbolInfo->Path->Size; j++)
					{
						pathElement->SetAttribute("no" + j, symbolInfo->Path->GetAt(j)->SymbolNo + "");
						pathElement->SetAttribute("decision" + j, symbolInfo->Decision->GetAt(j) + "");
					}
					break;
				default:
					for (int j = 0; j < symbolInfo->Path->Size; j++)
					{
						pathElement->SetAttribute("no" + j, symbolInfo->Path->GetAt(j)->SymbolNo + "");
					}
					break;
				}
			}
		xmlElement->AppendChild(pathElement);
	}
	task<void>(xmlDocument->SaveToFileAsync(file));
	}
}

void flowchart::MainPage::deleteConnectLine(UINT16 deleteSymbolNo)
{
	Page^ varGridPage = (Page^)(GridContentFrame->Content);
	Grid^ varPageGrid = safe_cast<Grid^>(varGridPage->FindName("PageGrid"));
	Canvas^ varPageGridCanvas = safe_cast<Canvas^>(varGridPage->FindName("PageGridCanvas"));

	//삭제된 심볼의 정보를 찾는다
	SymbolInfo^ deleteSymbolInfo = App::getSymbolInfoByNo(deleteSymbolNo);
	if (deleteSymbolInfo == nullptr) return;

	//모든 심볼정보를 순회하며, 삭제된 심볼과 연결된 심볼의 연결선을 삭제한다.
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		SymbolInfo^ tempSymbolInfo = App::symbolVector->GetAt(i);

		//삭제된 심볼에서 나온 선을 삭제한다.
		if (tempSymbolInfo == deleteSymbolInfo)
		{
			//연결된 심볼의 정보를 순회
			for (int j = 0; j < deleteSymbolInfo->Path->Size; j++)
			{
				//선이름 생성
				SymbolInfo^ connectSymbolInfo = deleteSymbolInfo->Path->GetAt(j);
				String^ connectLineNameStr = L"connectLine ";
				connectLineNameStr += deleteSymbolInfo->SymbolNo;
				connectLineNameStr += L" to ";
				connectLineNameStr += connectSymbolInfo->SymbolNo;

				//deletor 이름 생성
				String^ lineDeletor1NameStr = "lineDeletor1 " +
											  deleteSymbolInfo->SymbolNo +
											  " to " +
											  connectSymbolInfo->SymbolNo;
				String^ lineDeletor2NameStr = "lineDeletor2 " +
											  deleteSymbolInfo->SymbolNo +
											  " to " +
											  connectSymbolInfo->SymbolNo;

				//방향표시 이름 생성
				String^ indicatorNameStr = "indicator " + 
										   deleteSymbolInfo->SymbolNo + 
										   " to " + 
										   connectSymbolInfo->SymbolNo;

				//선이름으로 선을 찾아서 삭제
				UIElement^ childPageGridCanvas = nullptr;
				for (int k = 0; k < varPageGridCanvas->Children->Size; k++)
				{
					childPageGridCanvas = varPageGridCanvas->Children->GetAt(k);
					if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Polyline") == 0)
					{
						Polyline^ connectLine = safe_cast<Polyline^>(childPageGridCanvas);
						if (connectLine->Name == connectLineNameStr || connectLine->Name == indicatorNameStr)
						{
							varPageGridCanvas->Children->RemoveAt(k);
							k--;
						}
					}
					else if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Ellipse") == 0)
					{
						Ellipse^ lineDeletor = safe_cast<Ellipse^>(childPageGridCanvas);
						if (lineDeletor->Name == lineDeletor1NameStr || lineDeletor->Name == lineDeletor2NameStr)
						{
							varPageGridCanvas->Children->RemoveAt(k);
							k--;
						}
					}
				}

				//심볼타입이 2(decision)이라면 decisionText도 삭제해줘야 함
				if (deleteSymbolInfo->SymbolType == 2)
				{
					String^ decisionTextNameStr = "decisionText " + deleteSymbolNo + " to " + connectSymbolInfo->SymbolNo;
					UIElement^ childPageGrid = nullptr;
					for (int k = 0; k < varPageGrid->Children->Size; k++)
					{
						childPageGrid = varPageGrid->Children->GetAt(k);
						if (wcscmp(childPageGrid->ToString()->Data(), L"Windows.UI.Xaml.Controls.Border") == 0)
						{
							Border^ decisionText = safe_cast<Border^>(childPageGrid);
							if (wcscmp(decisionText->Name->Data(), decisionTextNameStr->Data()) == 0)
							{
								varPageGrid->Children->RemoveAt(k);
								break;
							}
						}
					}
				}
			}
		}
		//다른 심볼에서 나온 삭제된 심볼과 연결된 선을 삭제한다
		else
		{
			//연결된 심볼의 정보를 순회
			for (int j = 0; j < tempSymbolInfo->Path->Size; j++)
			{
				//삭제된 심볼과 연결됨
				if (tempSymbolInfo->Path->GetAt(j) == deleteSymbolInfo)
				{
					//선이름 생성
					String^ connectLineNameStr = L"connectLine ";
					connectLineNameStr += tempSymbolInfo->SymbolNo;
					connectLineNameStr += L" to ";
					connectLineNameStr += deleteSymbolNo;

					//deletor 이름 생성
					String^ lineDeletor1NameStr = "lineDeletor1 " +
												  tempSymbolInfo->SymbolNo +
												  " to " +
												  deleteSymbolNo;
					String^ lineDeletor2NameStr = "lineDeletor2 " +
												  tempSymbolInfo->SymbolNo +
												  " to " +
												  deleteSymbolNo;

					//방향표시 이름 생성
					String^ indicatorNameStr = "indicator " +
											   tempSymbolInfo->SymbolNo + 
											   " to " + 
											   deleteSymbolNo;

					//선이름으로 선을 찾아서 삭제함
					UIElement^ childPageGridCanvas = nullptr;
					for (int k = 0; k < varPageGridCanvas->Children->Size; k++)
					{
						childPageGridCanvas = varPageGridCanvas->Children->GetAt(k);
						if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Polyline") == 0)
						{
							Polyline^ connectLine = safe_cast<Polyline^>(childPageGridCanvas);
							if (connectLine->Name == connectLineNameStr || connectLine->Name == indicatorNameStr)
							{
								varPageGridCanvas->Children->RemoveAt(k);
								k--;
							}
						}
						else if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Ellipse") == 0)
						{
							Ellipse^ lineDeletor = safe_cast<Ellipse^>(childPageGridCanvas);
							if (lineDeletor->Name == lineDeletor1NameStr || lineDeletor->Name == lineDeletor2NameStr)
							{
								varPageGridCanvas->Children->RemoveAt(k);
								k--;
							}
						}
					}

					//심볼타입이 2(decision)이라면 decisionText도 삭제해줘야 함
					if (tempSymbolInfo->SymbolType == 2)
					{
						String^ decisionTextNameStr = "decisionText " + tempSymbolInfo->SymbolNo + " to " + deleteSymbolNo;
						UIElement^ childPageGrid = nullptr;
						for (int k = 0; k < varPageGrid->Children->Size; k++)
						{
							childPageGrid = varPageGrid->Children->GetAt(k);
							if (wcscmp(childPageGrid->ToString()->Data(), L"Windows.UI.Xaml.Controls.Border") == 0)
							{
								Border^ decisionText = safe_cast<Border^>(childPageGrid);
								if (wcscmp(decisionText->Name->Data(), decisionTextNameStr->Data()) == 0)
								{
									varPageGrid->Children->RemoveAt(k);
									break;
								}
							}
						}
					}

					break;
				}
			}
		}
	}

	varPageGrid->UpdateLayout();
	varPageGridCanvas->UpdateLayout();
}

void flowchart::MainPage::UndoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	App::undoOrRedoButtonClicked = true;

	/*if (App::historyIndex <= 0)
	{
		ContentDialog^ noHistoryDialog = ref new ContentDialog();
		noHistoryDialog->Title = "히스토리 없음";
		noHistoryDialog->Content = "더이상 Undo를 할 수 없습니다.";
		noHistoryDialog->PrimaryButtonText = "OK";
		noHistoryDialog->ShowAsync();
	}
	else
	{

	}*/
	if (!App::historyObject->unDo())
	{
		UndoHistoryDialog^ dialog = ref new UndoHistoryDialog();
		dialog->ShowAsync();
	}
	else
	{
		this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid));
	}
}

void flowchart::MainPage::RedoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	App::undoOrRedoButtonClicked = true;

	if (!App::historyObject->reDo())
	{
		RedoHistoryDialog^ dialog = ref new RedoHistoryDialog();
		dialog->ShowAsync();
	}
	else
	{
		this->GridContentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(GridPage::typeid));
	}
}
