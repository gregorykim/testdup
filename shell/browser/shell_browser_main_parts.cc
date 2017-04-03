// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_browser_main_parts.h"

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "build/build_config.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/url_constants.h"
#include "content/shell/browser/shell.h"
#include "content/shell/browser/shell_access_token_store.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_devtools_manager_delegate.h"
#include "content/shell/browser/shell_net_log.h"
#include "content/shell/common/shell_switches.h"
#include "device/bluetooth/bluetooth_adapter_factory.h"
#include "device/geolocation/geolocation_delegate.h"
#include "device/geolocation/geolocation_provider.h"
#include "net/base/filename_util.h"
#include "net/base/net_module.h"
#include "net/grit/net_resources.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"

#if defined(OS_ANDROID)
#include "base/message_loop/message_loop.h"
#include "components/crash/content/browser/crash_dump_manager_android.h"
#include "net/android/network_change_notifier_factory_android.h"
#include "net/base/network_change_notifier.h"
#endif

#if defined(USE_AURA) && defined(USE_X11)
#include "ui/events/devices/x11/touch_factory_x11.h"  // nogncheck
#endif
#if !defined(OS_CHROMEOS) && defined(USE_AURA) && defined(OS_LINUX)
#include "ui/base/ime/input_method_initializer.h"
#endif
#if defined(OS_CHROMEOS)
#include "chromeos/dbus/dbus_thread_manager.h"
#include "device/bluetooth/dbus/bluez_dbus_manager.h"
#elif defined(OS_LINUX)
#include "device/bluetooth/dbus/dbus_bluez_manager_wrapper_linux.h"
#endif

#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/shell/browser/shell_host_content_settings_map_factory.h"

namespace content {

namespace {

// A provider of services for Geolocation.
class ShellGeolocationDelegate : public device::GeolocationDelegate {
 public:
  explicit ShellGeolocationDelegate(ShellBrowserContext* context)
      : context_(context) {}

  scoped_refptr<device::AccessTokenStore> CreateAccessTokenStore() final {
    return new ShellAccessTokenStore(context_);
  }

 private:
  ShellBrowserContext* context_;
  DISALLOW_COPY_AND_ASSIGN(ShellGeolocationDelegate);
};

GURL GetStartupURL() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kContentBrowserTest))
    return GURL();
  const base::CommandLine::StringVector& args = command_line->GetArgs();

#if defined(OS_ANDROID)
  // Delay renderer creation on Android until surface is ready.
  return GURL();
#endif

  if (args.empty())
    return GURL("https://www.google.com/");

  GURL url(args[0]);
  if (url.is_valid() && url.has_scheme())
    return url;

  return net::FilePathToFileURL(
      base::MakeAbsoluteFilePath(base::FilePath(args[0])));
}

base::StringPiece PlatformResourceProvider(int key) {
  if (key == IDR_DIR_HEADER_HTML) {
    base::StringPiece html_data =
        ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
            IDR_DIR_HEADER_HTML);
    return html_data;
  }
  return base::StringPiece();
}

}  // namespace

ShellBrowserMainParts::ShellBrowserMainParts(
    const MainFunctionParams& parameters)
    : parameters_(parameters),
      run_message_loop_(true) {
}

ShellBrowserMainParts::~ShellBrowserMainParts() {
}

#if !defined(OS_MACOSX)
void ShellBrowserMainParts::PreMainMessageLoopStart() {
#if defined(USE_AURA) && defined(USE_X11)
  ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif
}
#endif

void ShellBrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_ANDROID)
  base::MessageLoopForUI::current()->Start();
#endif

#if defined(OS_CHROMEOS)
  chromeos::DBusThreadManager::Initialize();
  bluez::BluezDBusManager::Initialize(
      chromeos::DBusThreadManager::Get()->GetSystemBus(),
      chromeos::DBusThreadManager::Get()->IsUsingFakes());
#elif defined(OS_LINUX)
  bluez::DBusBluezManagerWrapperLinux::Initialize();
#endif
}

void ShellBrowserMainParts::PreEarlyInitialization() {
#if !defined(OS_CHROMEOS) && defined(USE_AURA) && defined(OS_LINUX)
  ui::InitializeInputMethodForTesting();
#endif
#if defined(OS_ANDROID)
  net::NetworkChangeNotifier::SetFactory(
      new net::NetworkChangeNotifierFactoryAndroid());
#endif
}

void ShellBrowserMainParts::InitializeBrowserContexts() {
  set_browser_context(new ShellBrowserContext(false, net_log_.get()));
  set_off_the_record_browser_context(
      new ShellBrowserContext(true, net_log_.get()));
}

void ShellBrowserMainParts::InitializeMessageLoopContext() {
  ui::MaterialDesignController::Initialize();
  Shell::CreateNewWindow(browser_context_.get(),
                         GetStartupURL(),
                         NULL,
                         gfx::Size());
}

#if defined(OS_ANDROID)
int ShellBrowserMainParts::PreCreateThreads() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableCrashReporter)) {
    base::FilePath crash_dumps_dir =
        base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
            switches::kCrashDumpsDir);
    crash_dump_manager_.reset(new breakpad::CrashDumpManager(crash_dumps_dir));
  }

  return 0;
}
#endif

void ShellBrowserMainParts::PreMainMessageLoopRun() {
  net_log_.reset(new ShellNetLog("content_shell"));
  InitializeBrowserContexts();
  device::GeolocationProvider::SetGeolocationDelegate(
      new ShellGeolocationDelegate(browser_context()));
  Shell::Initialize();
  net::NetModule::SetResourceProvider(PlatformResourceProvider);
  ShellDevToolsManagerDelegate::StartHttpHandler(browser_context_.get());
  InitializeMessageLoopContext();

  // Init PermissionManager, ShellHostContentSettingsMapFactory
  ShellHostContentSettingsMapFactory::Get();
  browser_context_->GetPermissionManager();

  if (parameters_.ui_task) {
    parameters_.ui_task->Run();
    delete parameters_.ui_task;
    run_message_loop_ = false;
  }
}

bool ShellBrowserMainParts::MainMessageLoopRun(int* result_code)  {
  return !run_message_loop_;
}

void ShellBrowserMainParts::PostMainMessageLoopRun() {
  ShellDevToolsManagerDelegate::StopHttpHandler();

  // Destroy ShellHostContentSettingsMapFactory
  ShellHostContentSettingsMapFactory::Get()->ShutdownOnUIThread();

  browser_context_.reset();
  off_the_record_browser_context_.reset();
}

void ShellBrowserMainParts::PostDestroyThreads() {
#if defined(OS_CHROMEOS)
  device::BluetoothAdapterFactory::Shutdown();
  bluez::BluezDBusManager::Shutdown();
  chromeos::DBusThreadManager::Shutdown();
#elif defined(OS_LINUX)
  device::BluetoothAdapterFactory::Shutdown();
  bluez::DBusBluezManagerWrapperLinux::Shutdown();
#endif
}

}  // namespace
