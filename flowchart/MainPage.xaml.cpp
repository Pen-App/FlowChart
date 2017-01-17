﻿//
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

		deleteConnectLine(App::draggingSymbolNo);

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
		if (file != nullptr)
		{
			// symbolVector 초기화
			App::symbolVector->Clear();

			// file->Name에 선택한 파일의 이름이 저장되어 있습니다.
			create_task(XmlDocument::LoadFromFileAsync(file)).then([this](XmlDocument^ xmlDocument)
			{
				// document에서 해당 노드를 검색하여 노드가 존재하는 경우
				XmlNodeList ^items = xmlDocument->SelectNodes("file");
				if (items != nullptr)
				{
					// Root 노드를 얻는다
					IXmlNode^ rootNode = items->GetAt(0);
					XmlNodeList^ symbolList = rootNode->ChildNodes;
					for (int i = 0; i < symbolList->Size; i++)
					{
						SymbolInfo^ symbolInfo = ref new SymbolInfo();
						IXmlNode^ symbolNode = symbolList->GetAt(i);

						// 'title'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ titleNode = symbolNode->Attributes->GetNamedItem("title");
						symbolInfo->Title = (String^)titleNode->NodeValue;

						// 'detail'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ detailNode = symbolNode->Attributes->GetNamedItem("detail");
						symbolInfo->Detail = (String^)detailNode->NodeValue;

						// 'content'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ contentNode = symbolNode->Attributes->GetNamedItem("content");
						symbolInfo->Content = (String^)contentNode->NodeValue;

						// 'symbolType'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ symbolTypeNode = symbolNode->Attributes->GetNamedItem("symbolType");
						symbolInfo->SymbolType = (int)symbolTypeNode->NodeValue;

						// 'columnIndex'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ columnIndexNode = symbolNode->Attributes->GetNamedItem("columnIndex");
						symbolInfo->ColumnIndex = (int)columnIndexNode->NodeValue;

						// 'rowIndex'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ rowIndexNode = symbolNode->Attributes->GetNamedItem("rowIndex");
						symbolInfo->RowIndex = (int)rowIndexNode->NodeValue;

						// 'no'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						IXmlNode^ noNode = symbolNode->Attributes->GetNamedItem("no");
						symbolInfo->SymbolNo = (int)noNode->NodeValue;

						// 'path'의 이름을 가진 아이템 노드를 반환 & vector에 저장
						XmlNodeList^ pathList = symbolNode->ChildNodes;
						for (int j = 0; j < pathList->Size; j++)
						{
							IXmlNode^ pathNode = pathList->GetAt(j);
							SymbolInfo^ pathSymbolInfo = ref new SymbolInfo();
							pathSymbolInfo->SymbolNo = (int)pathNode->Attributes
														->GetNamedItem("symbolNo")->NodeValue;
							symbolInfo->Path->Append(pathSymbolInfo);
						}

						App::symbolVector->Append(symbolInfo);
						MessageDialog ^msg = ref new MessageDialog(
							"title = " + symbolInfo->Title
							+ "\ndetail = " + symbolInfo->Detail
							+ "\ncontent = " + symbolInfo->Content
							+ "\ntype = " + symbolInfo->SymbolType
							+ "\ncolumn = " + symbolInfo->ColumnIndex
							+ "\nrow = " + symbolInfo->RowIndex
							+ "\nno = " + symbolInfo->SymbolNo
						);
						msg->ShowAsync();
					}
				}
			});
		}
		else 
		{
			// "취소 버튼을 누른 경우"
		}
	});
}

// 파일 저장 클릭시
void flowchart::MainPage::SaveFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (App::symbolVector->Size <= 0)	// 추가한 도형이 없을 경우
	{
		return;
	}

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
		symbolInfo->RowIndex =  safe_cast<int>(tmpImg->GetValue(pageGrid->RowProperty));
		symbolInfo->ColumnIndex = safe_cast<int>(tmpImg->GetValue(pageGrid->ColumnProperty));
	}

	FileSavePicker^ savePicker = SaveFilePath();
	SaveFileContent(savePicker);
}

// 저장할 경로 및 파일형식 설정
FileSavePicker^ flowchart::MainPage::SaveFilePath()
{
	FileSavePicker^ savePicker = ref new FileSavePicker();
	savePicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

	auto fileType = ref new Vector<String^>();
	fileType->Append(".xml");
	fileType->Append(".pap");
	savePicker->FileTypeChoices->Insert("PenApp Files", fileType);

	savePicker->DefaultFileExtension = ".xaml";
	savePicker->SuggestedFileName = FileName->Text;
	FileName->Text = savePicker->SuggestedFileName;

	return savePicker;
}

// xml 파싱해서 파일 저장
void flowchart::MainPage::SaveFileContent(FileSavePicker^ savePicker)
{
	task<StorageFile ^>(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
	{
		if (file != nullptr)
		{
			XmlDocument^ xmlDocument = ref new XmlDocument;	// xml 문서형식 사용
			XmlElement^ rootElement = xmlDocument->CreateElement("file");	// root 항목 설정
			rootElement->SetAttribute("fileName", FileName->Text);
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
					for (int j = 0; j < symbolInfo->Path->Size; j++)
					{
						pathElement->SetAttribute("noIndex"+j, symbolInfo->Path->GetAt(j)->SymbolNo + "");
					}
				}
			xmlElement->AppendChild(pathElement);
		}
		task<void>(xmlDocument->SaveToFileAsync(file));
		}
	});
}

void flowchart::MainPage::deleteConnectLine(UINT16 deleteSymbolNo)
{
	Page^ varGridPage = (Page^)(GridContentFrame->Content);
	Canvas^ varPageGridCanvas = safe_cast<Canvas^>(varGridPage->FindName("PageGridCanvas"));

	//삭제된 심볼의 정보를 찾는다
	SymbolInfo^ deleteSymbolInfo = nullptr;
	for (int i = 0; i < App::symbolVector->Size; i++)
	{
		if (App::symbolVector->GetAt(i)->SymbolNo == deleteSymbolNo)
		{
			deleteSymbolInfo = App::symbolVector->GetAt(i);
			break;
		}
	}

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

				//선이름으로 선을 찾아서 삭제
				UIElement^ childPageGridCanvas = nullptr;
				for (int k = 0; k < varPageGridCanvas->Children->Size; k++)
				{
					childPageGridCanvas = varPageGridCanvas->Children->GetAt(k);
					if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Line") == 0)
					{
						Line^ connectLine = safe_cast<Line^>(childPageGridCanvas);
						if (wcscmp(connectLine->Name->Data(), connectLineNameStr->Data()) == 0)
						{
							varPageGridCanvas->Children->RemoveAt(k);
							break;
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

					//선이름으로 선을 찾아서 삭제함
					UIElement^ childPageGridCanvas = nullptr;
					for (int k = 0; k < varPageGridCanvas->Children->Size; k++)
					{
						childPageGridCanvas = varPageGridCanvas->Children->GetAt(k);
						if (wcscmp(childPageGridCanvas->ToString()->Data(), L"Windows.UI.Xaml.Shapes.Line") == 0)
						{
							Line^ connectLine = safe_cast<Line^>(childPageGridCanvas);
							if (wcscmp(connectLine->Name->Data(), connectLineNameStr->Data()) == 0)
							{
								varPageGridCanvas->Children->RemoveAt(k);
								break;
							}
						}
					}
					break;
				}
			}
		}
	}
}
