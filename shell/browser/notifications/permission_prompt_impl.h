// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_SHELL_PERMISSION_PROMPT_IMPL_H_
#define CONTENT_BROWSER_SHELL_PERMISSION_PROMPT_IMPL_H_

#include <memory>
#include <string>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/public/browser/web_contents.h"
#include "content/shell/browser/notifications/website_settings_ui.h"
#include "third_party/WebKit/public/platform/modules/permissions/permission_status.mojom.h"
#include "ui/base/models/simple_menu_model.h"
#include "ui/gfx/geometry/point.h"
#include "ui/views/bubble/bubble_border.h"
#include "url/gurl.h"

namespace views {
class View;
}

namespace Permission_prompt {

class Delegate {
 public:
  virtual ~Delegate() {}
  virtual void Accept(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
                                         const GURL req_url) = 0;
  virtual void Deny(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
                                         const GURL req_url) = 0;
  virtual void Closing() = 0;
};



class PermissionsBubbleDialogDelegateView;

class PermissionPromptImpl {
 public:
  explicit PermissionPromptImpl(Delegate* delegate,
                                          const GURL& req_url,
                                          const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
                                          content::WebContents * webcontent);
  ~PermissionPromptImpl();

  // PermissionPrompt:
//  void SetDelegate(Delegate* delegate);
  void Show(const std::vector<bool>& values);
  bool CanAcceptRequestUpdate();
  void Hide();
  bool IsVisible();
  void UpdateAnchorPosition();
  gfx::NativeWindow GetNativeWindow();

  void Closing();
  void Accept();
  void Deny();


 private:
  // These three functions have separate implementations for Views-based and
  // Cocoa-based browsers, to allow this bubble to be used in either.

  // Returns the anchor point to anchor the permission bubble to, as a fallback.
  // Only used if GetAnchorView() returns nullptr.
  gfx::Point GetAnchorPoint(){
      gfx::Point temp(0,0);
      return temp;    
  }
  // Returns the type of arrow to display on the permission bubble.
  views::BubbleBorder::Arrow GetAnchorArrow(){return views::BubbleBorder::TOP_LEFT;}

  views::View* GetAnchorView(){return nullptr;}



//  content::WebContents * webcontent_;
  PermissionsBubbleDialogDelegateView* bubble_delegate_;
  Delegate* delegate_;
  const GURL requesting_original_url_;
  const base::Callback<void(blink::mojom::PermissionStatus)> callback_;

  DISALLOW_COPY_AND_ASSIGN(PermissionPromptImpl);
};
}//Permission_prompt

#endif  // CONTENT_BROWSER_SHELL_PERMISSION_PROMPT_IMPL_H_
