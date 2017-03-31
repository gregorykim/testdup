// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/notifications/permission_prompt_impl.h"

#include <stddef.h>

#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/strings/string16.h"
#include "components/url_formatter/elide_url.h"
#include "ui/accessibility/ax_view_state.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/gfx/text_constants.h"
#include "ui/gfx/vector_icons_public.h"
#include "ui/views/bubble/bubble_dialog_delegate.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/button/menu_button_listener.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/layout/layout_constants.h"
#include "base/strings/utf_string_conversions.h"

namespace {

const int kItemMajorSpacing = 9;

}  // namespace



PermissionSettingsUI::PermissionInfo::PermissionInfo()
    : type(CONTENT_SETTINGS_TYPE_DEFAULT),
      setting(CONTENT_SETTING_DEFAULT),
      default_setting(CONTENT_SETTING_DEFAULT),
      source(content_settings::SETTING_SOURCE_NONE),
      is_incognito(false) {}






namespace Permission_prompt {

class PermissionMenuModel : public ui::SimpleMenuModel,
                            public ui::SimpleMenuModel::Delegate {
 public:
    
  typedef base::Callback<void(const PermissionSettingsUI::PermissionInfo&)>
      ChangeCallback;

  // Create a new menu model for permission settings.
  PermissionMenuModel(const PermissionSettingsUI::PermissionInfo& info,
                      const ChangeCallback& callback);
  // Creates a special-case menu model that only has the allow and block
  // options.  It does not track a permission type.  |setting| is the
  // initial selected option.  It must be either CONTENT_SETTING_ALLOW or
  // CONTENT_SETTING_BLOCK.
  PermissionMenuModel(//const GURL& url,
                      ContentSetting setting,
                      const ChangeCallback& callback);
  ~PermissionMenuModel() override;

  // Overridden from ui::SimpleMenuModel::Delegate:
  bool IsCommandIdChecked(int command_id) const override;
  bool IsCommandIdEnabled(int command_id) const override;
  void ExecuteCommand(int command_id, int event_flags) override;

 private:
//  HostContentSettingsMap* host_content_settings_map_;

  // The permission info represented by the menu model.
  PermissionSettingsUI::PermissionInfo permission_;

  // Callback to be called when the permission's setting is changed.
  ChangeCallback callback_;

  DISALLOW_COPY_AND_ASSIGN(PermissionMenuModel);
};
PermissionMenuModel::PermissionMenuModel(
     const PermissionSettingsUI::PermissionInfo& info,
    const ChangeCallback& callback)
    : ui::SimpleMenuModel(this),
       permission_(info),
      callback_(callback) {
  DCHECK(!callback_.is_null());
  base::string16 label;

  ContentSetting effective_default_setting = permission_.default_setting;
  
  switch (effective_default_setting) {
    case CONTENT_SETTING_ALLOW:
      label = base::ASCIIToUTF16("Allow");
      break;
    case CONTENT_SETTING_BLOCK:
      label = base::ASCIIToUTF16("Block");
      break;
    case CONTENT_SETTING_ASK:
      label = base::ASCIIToUTF16("Ask");
      break;
    default:
      break;
  }
}

PermissionMenuModel::PermissionMenuModel(ContentSetting setting,
                                         const ChangeCallback& callback)
    : ui::SimpleMenuModel(this), 
      callback_(callback) {
  DCHECK(setting == CONTENT_SETTING_ALLOW || setting == CONTENT_SETTING_BLOCK);
  permission_.type = CONTENT_SETTINGS_TYPE_DEFAULT;
  permission_.setting = setting;
  permission_.default_setting = CONTENT_SETTING_NUM_SETTINGS;
 }

PermissionMenuModel::~PermissionMenuModel() {}

bool PermissionMenuModel::IsCommandIdChecked(int command_id) const {
  ContentSetting setting = permission_.setting; 
  return setting == command_id;
}

bool PermissionMenuModel::IsCommandIdEnabled(int command_id) const {
  return true;
}

void PermissionMenuModel::ExecuteCommand(int command_id, int event_flags) {
  permission_.setting = static_cast<ContentSetting>(command_id);
  callback_.Run(permission_);
}
  
///////////////////////////////////////////////////////////////////////////////
// View implementation for the permissions bubble.
class PermissionsBubbleDialogDelegateView
    : public views::BubbleDialogDelegateView{
 public:
  PermissionsBubbleDialogDelegateView(
      PermissionPromptImpl* owner,
      const std::vector<bool>& accept_state);
  ~PermissionsBubbleDialogDelegateView() override;

  void CloseBubble();
  void SizeToContents();

  // BubbleDialogDelegateView:
  bool ShouldShowCloseButton() const override;
  const gfx::FontList& GetTitleFontList() const override;
  base::string16 GetWindowTitle() const override;
  void OnWidgetDestroying(views::Widget* widget) override;
  gfx::Size GetPreferredSize() const override;
  void GetAccessibleState(ui::AXViewState* state) override;
  bool Cancel() override;
  bool Accept() override;
  bool Close() override;
  int GetDefaultDialogButton() const override;
  int GetDialogButtons() const override;
  base::string16 GetDialogButtonLabel(ui::DialogButton button) const override;

  
  // Updates the anchor's arrow and view. Also repositions the bubble so it's
  // displayed in the correct location.
  void UpdateAnchor(views::View* anchor_view,
                    const gfx::Point& anchor_point,
                    views::BubbleBorder::Arrow anchor_arrow);

 private:
  PermissionPromptImpl* owner_;
  base::string16 display_origin_;
  
  DISALLOW_COPY_AND_ASSIGN(PermissionsBubbleDialogDelegateView);
};

PermissionsBubbleDialogDelegateView::PermissionsBubbleDialogDelegateView(
    PermissionPromptImpl* owner,
    const std::vector<bool>& accept_state)
    : owner_(owner){


  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kVertical, 0, 0,
                                        kItemMajorSpacing));

   //    DCHECK(index < accept_state.size());
   // The row is laid out containing a leading-aligned label area and a
   // trailing column which will be filled if there are multiple permission
   // requests.
   views::View* row = new views::View();
   views::GridLayout* row_layout = new views::GridLayout(row);
   row->SetLayoutManager(row_layout);
   views::ColumnSet* columns = row_layout->AddColumnSet(0);
   columns->AddColumn(views::GridLayout::LEADING, views::GridLayout::FILL,
                      0, views::GridLayout::USE_PREF, 0, 0);
   columns->AddColumn(views::GridLayout::TRAILING, views::GridLayout::FILL,
                      100, views::GridLayout::USE_PREF, 0, 0);
   row_layout->StartRow(0, 0);
   
   views::View* label_container = new views::View();
   label_container->SetLayoutManager(new views::BoxLayout(
       views::BoxLayout::kHorizontal, views::kCheckboxIndent, 0,
       views::kItemLabelSpacing));
   views::Label* label =
       new views::Label(base::ASCIIToUTF16("Show Notifications"));
   label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
   label_container->AddChildView(label);
   row_layout->AddView(label_container);
   
   row_layout->AddView(new views::View());
   
   
   AddChildView(row);
   
}

PermissionsBubbleDialogDelegateView::~PermissionsBubbleDialogDelegateView() {
  if (owner_)
    owner_->Closing();
}

void PermissionsBubbleDialogDelegateView::CloseBubble() {
  owner_ = nullptr;
  GetWidget()->Close();
}

bool PermissionsBubbleDialogDelegateView::ShouldShowCloseButton() const {
  return true;
}

const gfx::FontList& PermissionsBubbleDialogDelegateView::GetTitleFontList()
    const {
  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  return rb.GetFontList(ui::ResourceBundle::BaseFont);
}

base::string16 PermissionsBubbleDialogDelegateView::GetWindowTitle() const {
  return base::string16();
}

void PermissionsBubbleDialogDelegateView::SizeToContents() {
  BubbleDialogDelegateView::SizeToContents();
}

void PermissionsBubbleDialogDelegateView::OnWidgetDestroying(
    views::Widget* widget) {
  views::BubbleDialogDelegateView::OnWidgetDestroying(widget);
  if (owner_) {
    owner_->Closing();
    owner_ = nullptr;
  }
}

gfx::Size PermissionsBubbleDialogDelegateView::GetPreferredSize() const {
  // TODO(estade): bubbles should default to this width.
  const int kWidth = 320 - GetInsets().width();
  return gfx::Size(kWidth, GetHeightForWidth(kWidth));
}

void PermissionsBubbleDialogDelegateView::GetAccessibleState(
    ui::AXViewState* state) {
  views::BubbleDialogDelegateView::GetAccessibleState(state);
  state->role = ui::AX_ROLE_ALERT_DIALOG;
}

int PermissionsBubbleDialogDelegateView::GetDefaultDialogButton() const {
  // To prevent permissions being accepted accidentally, and as a security
  // measure against crbug.com/619429, permission prompts should not be accepted
  // as the default action.
  return ui::DIALOG_BUTTON_NONE;
}

int PermissionsBubbleDialogDelegateView::GetDialogButtons() const {
  int buttons = ui::DIALOG_BUTTON_OK;
  buttons |= ui::DIALOG_BUTTON_CANCEL;
  return buttons;
}

base::string16 PermissionsBubbleDialogDelegateView::GetDialogButtonLabel(
    ui::DialogButton button) const {
  if (button == ui::DIALOG_BUTTON_CANCEL)
    return base::ASCIIToUTF16("Deny");

  return base::ASCIIToUTF16("Allow");
}

bool PermissionsBubbleDialogDelegateView::Cancel() {

  if (owner_) {
    owner_->Deny();
  }
  return true;
}

bool PermissionsBubbleDialogDelegateView::Accept() {
  if (owner_) {
    owner_->Accept();
  }
  return true;
}

bool PermissionsBubbleDialogDelegateView::Close() {
  return true;
}
 
void PermissionsBubbleDialogDelegateView::UpdateAnchor(
    views::View* anchor_view,
    const gfx::Point& anchor_point,
    views::BubbleBorder::Arrow anchor_arrow) {
  set_arrow(anchor_arrow);

  // Update the border in the bubble: will either add or remove the arrow.
  views::BubbleFrameView* frame =
      views::BubbleDialogDelegateView::GetBubbleFrameView();
  views::BubbleBorder::Arrow adjusted_arrow = anchor_arrow;
  if (base::i18n::IsRTL())
    adjusted_arrow = views::BubbleBorder::horizontal_mirror(adjusted_arrow);
  frame->SetBubbleBorder(std::unique_ptr<views::BubbleBorder>(
      new views::BubbleBorder(adjusted_arrow, shadow(), color())));

  // Reposition the bubble based on the updated arrow and view.
  SetAnchorView(anchor_view);
  // The anchor rect is ignored unless |anchor_view| is nullptr.
  SetAnchorRect(gfx::Rect(anchor_point, gfx::Size()));
}

//////////////////////////////////////////////////////////////////////////////
// PermissionPromptImpl

PermissionPromptImpl::PermissionPromptImpl(Delegate* delegate,
													 const GURL& req_url,
													 const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
													 content::WebContents * webcontent)
	:bubble_delegate_(nullptr),
     delegate_(delegate),
     requesting_original_url_(req_url),
     callback_(callback)
{
}

PermissionPromptImpl::~PermissionPromptImpl() {
}

void PermissionPromptImpl::Show(const std::vector<bool>& values) {
  if (bubble_delegate_)
    bubble_delegate_->CloseBubble();

  bubble_delegate_ =
      new PermissionsBubbleDialogDelegateView(this, values);

  // Set |parent_window| because some valid anchors can become hidden.
  bubble_delegate_->set_parent_window(NULL/*
      platform_util::GetViewForWindow(browser_->window()->GetNativeWindow())*/);

  views::BubbleDialogDelegateView::CreateBubble(bubble_delegate_)->Show();
  bubble_delegate_->SizeToContents();

  bubble_delegate_->UpdateAnchor(GetAnchorView(),
                                 GetAnchorPoint(),
                                 GetAnchorArrow());
}

bool PermissionPromptImpl::CanAcceptRequestUpdate() {
  return !(bubble_delegate_ && bubble_delegate_->IsMouseHovered());
}

void PermissionPromptImpl::Hide() {
  if (bubble_delegate_) {
    bubble_delegate_->CloseBubble();
    bubble_delegate_ = nullptr;
  }
}

bool PermissionPromptImpl::IsVisible() {
  return bubble_delegate_ != nullptr;
}

void PermissionPromptImpl::UpdateAnchorPosition() {
  if (IsVisible()) {
    bubble_delegate_->set_parent_window(/*
        platform_util::GetViewForWindow(browser_->window()->GetNativeWindow())*/NULL);
    bubble_delegate_->UpdateAnchor(GetAnchorView(),
                                   GetAnchorPoint(),
                                   GetAnchorArrow());
  }
}

gfx::NativeWindow PermissionPromptImpl::GetNativeWindow() {
  if (bubble_delegate_ && bubble_delegate_->GetWidget())
    return bubble_delegate_->GetWidget()->GetNativeWindow();
  return nullptr;
}

void PermissionPromptImpl::Closing() {
  if (bubble_delegate_)
    bubble_delegate_ = nullptr;
  if (delegate_)
    delegate_->Closing();
}

void PermissionPromptImpl::Accept() {
  if (delegate_)
    delegate_->Accept(callback_,requesting_original_url_);
}

void PermissionPromptImpl::Deny() {
  if (delegate_)
    delegate_->Deny(callback_,requesting_original_url_);
}

}//Permission_prompt

