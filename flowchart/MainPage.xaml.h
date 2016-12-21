//
// MainPage.xaml.h
// MainPage 클래스의 선언입니다.
//

#pragma once

#include "MainPage.g.h"

namespace flowchart
{
	/// <summary>
	/// 자체적으로 사용하거나 프레임 내에서 탐색할 수 있는 빈 페이지입니다.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Image_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void ScrollViewer_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
	};
}
