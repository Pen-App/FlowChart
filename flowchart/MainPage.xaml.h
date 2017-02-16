//
// MainPage.xaml.h
// MainPage 클래스의 선언입니다.
//

#pragma once

#include "MainPage.g.h"
#include <string>
namespace flowchart
{
	/// <summary>
	/// 자체적으로 사용하거나 프레임 내에서 탐색할 수 있는 빈 페이지입니다.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
		Windows::Foundation::IAsyncAction^ RenderImageToFileAsync(Windows::UI::Xaml::UIElement^ uiElement, Platform::String^ outputImageFilename);
	private:
		unsigned int pixelWidth;
		unsigned int pixelHeight;
		Platform::Array<unsigned char>^ pixelData;
		Platform::Array<unsigned char>^ GetArrayFromBuffer(Windows::Storage::Streams::IBuffer^ buffer);
		void StorePixelsFromBuffer(Windows::Storage::Streams::IBuffer^ buffer);
		concurrency::task<void> WriteBufferToFile(Platform::String^ outputImageFilename);
		concurrency::task<void> RenderAndSaveToFileAsync(Windows::UI::Xaml::UIElement^ uiElement, Platform::String^ outputImageFilename,
			uint32 width = 0, uint32 height = 0);

		void Image_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
		void Image_DropCompleted(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args);
		void ListBox_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void ListBox_DragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void ZoomInButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ZoomOutButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		// 파일 열기부분
		void OpenFileInit();
		void OpenFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OpenFileContent(Windows::Storage::StorageFile^ file);
		void OpenFileContentXmlParser(Windows::Data::Xml::Dom::XmlDocument^ xmlDocument);
		void SetPathList(Vector<SymbolInfo^>^ tmpPath);
		
		// 파일 저장부분
		void SaveFileInit();
		void SaveFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SaveFileContent(Windows::Storage::StorageFile^ file);

		void deleteConnectLine(UINT16 deleteSymbolNo);
		void NewFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void UndoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RedoButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SaveImage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
