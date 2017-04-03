// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEB_PUSH_CONTENT_VIEWS_H_
#define CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEB_PUSH_CONTENT_VIEWS_H_

#include <string>
#include <vector>



#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "ui/gfx/animation/animation_delegate.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/message_center/views/message_center_controller.h"
#include "ui/views/widget/widget_delegate.h"

#include "base/observer_list.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/shell/browser/notifications/notification.h"

#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/button/menu_button_listener.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/test/desktop_test_views_delegate.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"


#include "build/build_config.h"

#include "ui/gfx/native_widget_types.h"

#include "content/public/browser/desktop_notification_delegate.h"


namespace WebPushView {
class WebPushContentViews;
class WebPushViewRequest {
public:
    WebPushViewRequest();


    WebPushViewRequest(std::unique_ptr<content::DesktopNotificationDelegate> delegate,
                                   base::string16 title, 
                                   base::string16 message );
    ~WebPushViewRequest();
    void Add();
    void DisplayWebPush();
    void finalizeWebPush() const;

private:
	base::string16 title_;
	base::string16 message_;
	
    views::Widget* window_widget_;

    WebPushContentViews* content_view_;

    std::unique_ptr<content::DesktopNotificationDelegate> delegate_;
};

class WebPushContentViews : public views::WidgetDelegateView,
                          public gfx::AnimationDelegate,
                          public views::ButtonListener
                          {
public:                          
	  WebPushContentViews();
	  WebPushContentViews(const WebPushViewRequest* req);
      void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  const WebPushViewRequest* getReq() const {return request_;}
  void CreateWidget();
  void SetWebPushMessage(const base::string16& msg);
  void SetWebPushTitle(const base::string16& title);
  void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) override ;
  
private:

  const WebPushViewRequest* request_;

    
  base::string16 title_;
  base::string16 meesage_;

  views::Label* title_label_;
  views::Label* message_label_;
  views::LabelButton* exit_button_;
  View* button_view_;

  DISALLOW_COPY_AND_ASSIGN(WebPushContentViews);

} ; 
}// namespace WebPushView
#endif  // CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEB_PUSH_CONTENT_VIEWS_H_