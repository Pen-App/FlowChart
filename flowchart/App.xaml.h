//
// App.xaml.h
// App 클래스의 선언입니다.
//

#pragma once

#include "App.g.h"
#include "SymbolInfo.h"
#include <collection.h>



typedef UINT64 ID_TYPE;

namespace flowchart
{
	
	/// <summary>
	/// 기본 응용 프로그램 클래스를 보완하는 응용 프로그램별 동작을 제공합니다.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();
		static property ID_TYPE symbolIdCount;
		static property int selectedSymbolNumber;
		static property Platform::Collections::Vector<SymbolInfo^>^ symbolVector;

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	};
}
