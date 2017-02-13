﻿#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------


namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class AppBarButton;
                ref class TextBox;
                ref class Frame;
                ref class Image;
            }
        }
    }
}

namespace flowchart
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class MainPage : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
        private: ::Windows::UI::Xaml::Controls::AppBarButton^ NewFile;
        private: ::Windows::UI::Xaml::Controls::AppBarButton^ OpenFile;
        private: ::Windows::UI::Xaml::Controls::AppBarButton^ SaveFile;
        private: ::Windows::UI::Xaml::Controls::TextBox^ FileName;
        private: ::Windows::UI::Xaml::Controls::Frame^ GridContentFrame;
        private: ::Windows::UI::Xaml::Controls::Image^ s6;
        private: ::Windows::UI::Xaml::Controls::Image^ s5;
        private: ::Windows::UI::Xaml::Controls::Image^ s4;
        private: ::Windows::UI::Xaml::Controls::Image^ s3;
        private: ::Windows::UI::Xaml::Controls::Image^ s2;
        private: ::Windows::UI::Xaml::Controls::Image^ s1;
    };
}

