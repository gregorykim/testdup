// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "content/shell/browser/notifications/web_push_content_views.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/background.h"


#include "ui/aura/window.h"
#include "ui/aura/window_event_dispatcher.h"
#include "ui/aura/window_tree_host.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/event.h"
#include "ui/display/screen.h"
#include "base/strings/utf_string_conversions.h"



namespace WebPushView {
    
#define DIALOG_TOP_POSITION       false
#define DIALOG_WIDTH              400
#define DIALOG_HEIGHT             250
#define DIALOG_CONTROL_OK     "OK"

/*
class WebPushViewRequestList{
public:
	void Add();
	void remove();
	void finalize();
private:

	
};
*/
WebPushViewRequest::WebPushViewRequest()
{
}
WebPushViewRequest::~WebPushViewRequest()
{
}

WebPushViewRequest::WebPushViewRequest(std::unique_ptr<content::DesktopNotificationDelegate> delegate,base::string16 title,base::string16 message )
:title_(title),
message_(message),
delegate_(std::move(delegate))
{
    
}
void WebPushViewRequest::Add()
{
	
}
//void WebPushViewRequest::Layout(){}

void WebPushViewRequest::finalizeWebPush() const
{
    if(delegate_)
	   delegate_->NotificationClosed();
}
void WebPushViewRequest::DisplayWebPush()
{
    gfx::Size default_size(500,200);
    
    window_widget_ = new views::Widget;
    views::Widget::InitParams params;
    params.remove_standard_frame = true;
    params.type = views::Widget::InitParams::TYPE_POPUP;
    params.opacity = views::Widget::InitParams::OPAQUE_WINDOW;

    content_view_ = new WebPushContentViews(this);

    params.bounds = gfx::Rect(default_size.width(),default_size.height());
    
    params.delegate = content_view_;
    window_widget_->Init(params);
	content_view_->SetWebPushTitle(message_);
    content_view_->SetWebPushMessage(message_);
    gfx::NativeWindow window_;

	window_ = window_widget_->GetNativeWindow();
	window_->GetHost()->Show();
	window_widget_->Show();
    if(delegate_)
	    delegate_->NotificationDisplayed();


}

WebPushContentViews::WebPushContentViews(const WebPushViewRequest* req)
:request_(req)
{    
}
void WebPushContentViews::ButtonPressed(views::Button* sender, const ui::Event& event)
{
    getReq()->finalizeWebPush();
    GetWidget()->Close();
}




void WebPushContentViews::CreateWidget()
{
	set_background(views::Background::CreateStandardPanelBackground());
    SetBorder(
            views::Border::CreateSolidSidedBorder(0, 0, 1, 1, SkColorSetRGB(0xC0, 0xC0, 0xC0)));
//    int label_width = 0;
    views::GridLayout* layout = new views::GridLayout(this);
    SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 5);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 1,
                          views::GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 5);

    layout->AddPaddingRow(0, 2);
    { // Title
      layout->StartRow(0, 0);
      title_label_ = new views::Label();      
      title_label_->SetMultiLine(true);
      layout->AddView(title_label_);
    }
    layout->AddPaddingRow(0, 5);
    { // message
      message_label_ = new views::Label();
      message_label_->SetMultiLine(true);
      layout->StartRow(1, 0);
      layout->AddView(message_label_);
    }
    layout->AddPaddingRow(0, 5);
    layout->StartRow(0, 0);
    button_view_ = new View();
    views::GridLayout* button_layout = new views::GridLayout(button_view_);
    button_view_->SetLayoutManager(button_layout);
    views::ColumnSet* button_column_set =
        button_layout->AddColumnSet(0);
    
    exit_button_ = new views::LabelButton(this, base::ASCIIToUTF16(DIALOG_CONTROL_OK));
    exit_button_->SetStyle(views::Button::STYLE_BUTTON);
    gfx::Size cancel_button_size = exit_button_->GetPreferredSize();
    button_column_set->AddColumn(views::GridLayout::FILL,
                                  views::GridLayout::FILL, 1,
                                  views::GridLayout::FIXED,
                                  cancel_button_size.width(),
                                  cancel_button_size.width() / 2);
                                  

	button_layout->StartRow(0, 0);
	button_layout->AddView(exit_button_);
	layout->AddView(button_view_);

	
}
void WebPushContentViews::ViewHierarchyChanged(
    const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this) {
    CreateWidget();
  }
}

void WebPushContentViews::SetWebPushTitle(const base::string16& title)
{
	
	title_label_->SetText(title_);
}
void WebPushContentViews::SetWebPushMessage(const base::string16& msg)
{
	message_label_->SetText(msg);

}
    

}
