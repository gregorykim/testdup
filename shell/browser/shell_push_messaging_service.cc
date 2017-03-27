// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_push_messaging_service.h"

#include <vector>

#include "base/base64url.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/guid.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/platform_notification_service.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/notification_resources.h"
#include "content/public/common/platform_notification_data.h"
#include "content/public/common/push_subscription_options.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "content/shell/browser/shell_host_content_settings_map_factory.h"
#include "content/shell/browser/shell_pref_service.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/gcm_driver/gcm_client_factory.h"
#include "components/gcm_driver/gcm_desktop_utils.h"
#include "components/gcm_driver/gcm_driver.h"
#include "components/gcm_driver/gcm_driver_constants.h"
#include "components/gcm_driver/gcm_driver_desktop.h"
#include "components/prefs/pref_registry.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_service_factory.h"
#include "components/syncable_prefs/pref_service_syncable.h"

using instance_id::InstanceID;

namespace {

// The GCM channel's enabled state.
// const char kGCMChannelStatus[] = "gcm.channel_status";

// The GCM channel's polling interval (in seconds).
// const char kGCMChannelPollIntervalSeconds[] = "gcm.poll_interval";

// Last time when checking with the GCM channel status server is done.
// const char kGCMChannelLastCheckTime[] = "gcm.check_time";

// const int kDefaultPollIntervalSeconds = 60 * 60;  // 60 minutes.

const char kPushMessagingAppIdentifierPrefix[] = "wp:";
const char kInstanceIDGuidSuffix[] = "-V2";

// sizeof is strlen + 1 since it's null-terminated.
// const size_t kPrefixLength = sizeof(kPushMessagingAppIdentifierPrefix) - 1;
const size_t kGuidSuffixLength = sizeof(kInstanceIDGuidSuffix) - 1;

const char kSeparator = '#';    // Ok as only the origin of the url is used.
// const size_t kGuidLength = 36;  // "%08X-%04X-%04X-%04X-%012llX"

const char kPushMessagingGcmEndpoint[] =
    "https://android.googleapis.com/gcm/send/";

const char kPushMessagingPushProtocolEndpoint[] =
    "https://fcm.googleapis.com/fcm/send/";

blink::WebPushPermissionStatus ToPushPermission(
    blink::mojom::PermissionStatus permission_status) {
  switch (permission_status) {
    case blink::mojom::PermissionStatus::GRANTED:
      return blink::WebPushPermissionStatusGranted;
    case blink::mojom::PermissionStatus::DENIED:
      return blink::WebPushPermissionStatusDenied;
    case blink::mojom::PermissionStatus::ASK:
      return blink::WebPushPermissionStatusPrompt;
  }
  NOTREACHED();
  return blink::WebPushPermissionStatusDenied;
}

content::PushUnregistrationStatus ToUnregisterStatus(
    InstanceID::Result result) {
  switch (result) {
    case InstanceID::SUCCESS:
      return content::PUSH_UNREGISTRATION_STATUS_SUCCESS_UNREGISTERED;
    case InstanceID::INVALID_PARAMETER:
    case InstanceID::DISABLED:
    case InstanceID::SERVER_ERROR:
    case InstanceID::UNKNOWN_ERROR:
      return content::PUSH_UNREGISTRATION_STATUS_PENDING_SERVICE_ERROR;
    case InstanceID::ASYNC_OPERATION_PENDING:
    case InstanceID::NETWORK_ERROR:
      return content::PUSH_UNREGISTRATION_STATUS_PENDING_NETWORK_ERROR;
  }
  NOTREACHED();
  return content::PUSH_UNREGISTRATION_STATUS_SERVICE_NOT_AVAILABLE;
}

content::PushUnregistrationStatus ToUnregisterStatus(
    gcm::GCMClient::Result gcm_result) {
  switch (gcm_result) {
    case gcm::GCMClient::SUCCESS:
      return content::PUSH_UNREGISTRATION_STATUS_SUCCESS_UNREGISTERED;
    case gcm::GCMClient::INVALID_PARAMETER:
    case gcm::GCMClient::GCM_DISABLED:
    case gcm::GCMClient::SERVER_ERROR:
    case gcm::GCMClient::UNKNOWN_ERROR:
      return content::PUSH_UNREGISTRATION_STATUS_PENDING_SERVICE_ERROR;
    case gcm::GCMClient::ASYNC_OPERATION_PENDING:
    case gcm::GCMClient::NETWORK_ERROR:
    case gcm::GCMClient::TTL_EXCEEDED:
      return content::PUSH_UNREGISTRATION_STATUS_PENDING_NETWORK_ERROR;
  }
  NOTREACHED();
  return content::PUSH_UNREGISTRATION_STATUS_SERVICE_NOT_AVAILABLE;
}

}  // namespace

ShellPushMessagingService::ShellPushMessagingService()
    : weak_factory_(this) {
DLOG(WARNING) << "~~~~~~~~~~~~~~~ShellPushMessagingService create !!";
  base::SequencedWorkerPool* worker_pool =
      content::BrowserThread::GetBlockingPool();
  scoped_refptr<base::SequencedTaskRunner> blocking_task_runner(
      worker_pool->GetSequencedTaskRunnerWithShutdownBehavior(
          worker_pool->GetSequenceToken(),
          base::SequencedWorkerPool::SKIP_ON_SHUTDOWN));
  content::ShellBrowserContext* context = content::ShellContentBrowserClient::Get()->browser_context();
  base::FilePath path = context->GetPath();

  net::URLRequestContextGetter* context_getter =
      content::BrowserContext::GetDefaultStoragePartition(context)->
              GetURLRequestContext();
  std::string product_category_for_subtypes = "com.chrome.qnx";
  // std::string channel = "http://channel.status.request.url";

  // scoped_refptr<PrefRegistrySimple> registry(new PrefRegistrySimple);
  // registry->RegisterBooleanPref(kGCMChannelStatus, true);
  // registry->RegisterIntegerPref(kGCMChannelPollIntervalSeconds, kDefaultPollIntervalSeconds);
  // registry->RegisterInt64Pref(kGCMChannelLastCheckTime, 0);
  // PrefServiceFactory factory;
  // path = path.Append("Prefs");
  // factory.SetUserPrefsFile(path, blocking_task_runner.get());
  // base::ThreadRestrictions::SetIOAllowed(true);
  // prefs_ = factory.Create(registry.get());
  prefs_.reset(ShellPrefService::Get());

  driver_ = gcm::CreateGCMDriverDesktop(
      base::WrapUnique(new gcm::GCMClientFactory), prefs_.get(),
      path.Append(gcm_driver::kGCMStoreDirname), context_getter, version_info::Channel::STABLE,
      product_category_for_subtypes,
      content::BrowserThread::GetTaskRunnerForThread(
          content::BrowserThread::UI),
      content::BrowserThread::GetTaskRunnerForThread(
          content::BrowserThread::IO),
      blocking_task_runner);
  base::ThreadRestrictions::SetIOAllowed(false);

  // scoped_refptr<HostContentSettingsMap> settings_map_;
  // settings_map_ = new HostContentSettingsMap(
  //   prefs_.get(),
  //   false,
  //   true);

  // syncable_prefs::PrefServiceSyncable* pref_service =
  //     ShellPrefService::GetSyncable();
  //   pref_service->RegisterMergeDataFinishedCallback(
  //       base::Bind(&HostContentSettingsMap::MigrateDomainScopedSettings,
  //                  settings_map_->GetWeakPtr(), true /* after_sync */));
  GURL host("https://mail.google.com");
  ContentSetting setting = ShellHostContentSettingsMapFactory::Get()->GetContentSetting(
            host, GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS, 
      std::string());
  DLOG(WARNING) << "!!! setting : " << setting;
  ShellHostContentSettingsMapFactory::Get()->SetContentSettingDefaultScope(
            host, GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS, 
      std::string(), CONTENT_SETTING_ALLOW);
  // settings_map_->FlushLossyWebsiteSettings();
  setting = ShellHostContentSettingsMapFactory::Get()->GetContentSetting(
            host, GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS, 
      std::string());
  DLOG(WARNING) << "!!! setting 2 : " << setting;
  // settings_map_->ShutdownOnUIThread();
  // content::BrowserThread::PostTask(
  //     content::BrowserThread::IO, FROM_HERE,
  //     base::Bind(&ShellPushMessagingService::CreateGCMOnIO, base::Unretained(this)));
}

ShellPushMessagingService::~ShellPushMessagingService() {
DLOG(WARNING) << "~~~~~~~~~~~~~~~ShellPushMessagingService destroy !!";
  // task_runner_->DeleteSoon(FROM_HERE, driver_.release());
  // if (ShellHostContentSettingsMapFactory::Get())
  //   ShellHostContentSettingsMapFactory::Get()->ShutdownOnUIThread();
  if (driver_)
    driver_.release();
}

bool ShellPushMessagingService::CanHandle(const std::string& app_id) const {
  return base::StartsWith(app_id, kPushMessagingAppIdentifierPrefix,
                          base::CompareCase::INSENSITIVE_ASCII);
}

void ShellPushMessagingService::ShutdownHandler() {
  // Shutdown() should come before and it removes us from the list of app
  // handlers of gcm::GCMDriver so this shouldn't ever been called.
  NOTREACHED();
}

const char kPushMessagingForcedNotificationTag[] =
    "user_visible_auto_notification";

void ShellPushMessagingService::OnMessage(const std::string& app_id,
                                         const gcm::IncomingMessage& message) {

  DLOG(WARNING) << "app_id:" << app_id;
  for (std::map<std::string, std::string>::const_iterator it=message.data.begin(); it!=message.data.end(); ++it)
    DLOG(WARNING) << it->first << " => " << it->second;

  content::PlatformNotificationData notification_data;
  notification_data.title = base::ASCIIToUTF16(message.data.at("ar_app_name"));
  notification_data.direction =
      content::PlatformNotificationData::DIRECTION_LEFT_TO_RIGHT;
  notification_data.body = base::ASCIIToUTF16(message.data.at("alert"));
  notification_data.tag = kPushMessagingForcedNotificationTag;
  notification_data.icon = GURL();
  notification_data.silent = true;

  content::ShellContentBrowserClient::Get()->GetPlatformNotificationService()->DisplayPersistentNotification(
      content::ShellContentBrowserClient::Get()->browser_context(), app_id, GURL() /* service_worker_scope */, GURL(),
      notification_data, content::NotificationResources());
  NOTIMPLEMENTED();
}

void ShellPushMessagingService::OnMessagesDeleted(const std::string& app_id) {
  NOTIMPLEMENTED();
}

void ShellPushMessagingService::OnSendError(
      const std::string& app_id,
      const gcm::GCMClient::SendErrorDetails& send_error_details) {
  NOTIMPLEMENTED();

}

void ShellPushMessagingService::OnSendAcknowledged(const std::string& app_id,
                          const std::string& message_id) {
  NOTIMPLEMENTED();

}

// GetEndpoint method ----------------------------------------------------------

GURL ShellPushMessagingService::GetEndpoint(bool standard_protocol) const {
  return GURL(standard_protocol ? kPushMessagingPushProtocolEndpoint
                                : kPushMessagingGcmEndpoint);
}

// Subscribe and GetPermissionStatus methods -----------------------------------

void ShellPushMessagingService::SubscribeFromDocument(
    const GURL& requesting_origin,
    int64_t service_worker_registration_id,
    int renderer_id,
    int render_frame_id,
    const content::PushSubscriptionOptions& options,
    const RegisterCallback& callback) {
  std::string app_id = GetAppId(requesting_origin);
  NormalizeSenderInfo(options.sender_info);
  std::string sender_id = options.sender_info;
  // DLOG(WARNING) << "app_id:" << app_id;
  // DLOG(WARNING) << "sender_id:" << sender_id;
  std::vector<std::string> sender_ids;
  sender_ids.push_back(sender_id);
  driver_->AddAppHandler(app_id, this);
  driver_->Register(
      app_id,
      sender_ids,
      base::Bind(&ShellPushMessagingService::DidSubscribe, base::Unretained(this), app_id, sender_id, callback));
}

void ShellPushMessagingService::DidSubscribe(
    const std::string& app_id,
    const std::string& sender_id,
    const RegisterCallback& callback,
    const std::string& subscription_id,
    gcm::GCMClient::Result result) {

  content::PushRegistrationStatus status =
      content::PUSH_REGISTRATION_STATUS_SERVICE_ERROR;

  switch (result) {
    case InstanceID::SUCCESS:
        driver_->GetEncryptionInfo(app_id,
                                   base::Bind(&ShellPushMessagingService::DidSubscribeWithEncryptionInfo,
                                              base::Unretained(this), callback, subscription_id,
                                              content::PUSH_REGISTRATION_STATUS_SUCCESS_FROM_PUSH_SERVICE));
      return;
    case InstanceID::INVALID_PARAMETER:
    case InstanceID::DISABLED:
    case InstanceID::ASYNC_OPERATION_PENDING:
    case InstanceID::SERVER_ERROR:
    case InstanceID::UNKNOWN_ERROR:
      DLOG(ERROR) << "Push messaging subscription failed; InstanceID::Result = "
                  << result;
      status = content::PUSH_REGISTRATION_STATUS_SERVICE_ERROR;
      break;
    case InstanceID::NETWORK_ERROR:
      status = content::PUSH_REGISTRATION_STATUS_NETWORK_ERROR;
      break;
    default:
      break;
  }

  SubscribeEndWithError(callback, status);
}

void ShellPushMessagingService::SubscribeEndWithError(
    const RegisterCallback& callback,
    content::PushRegistrationStatus status) {
  DidSubscribeWithEncryptionInfo(callback, std::string() /* subscription_id */, status,
               std::string() /* p256dh */,
               std::string() /* auth */);
}

void ShellPushMessagingService::DidSubscribeWithEncryptionInfo(
    const RegisterCallback& callback,
    const std::string& subscription_id,
    content::PushRegistrationStatus status,
    const std::string& p256dh,
    const std::string& auth_secret) {
  callback.Run(subscription_id,
               std::vector<uint8_t>(p256dh.begin(), p256dh.end()),
               std::vector<uint8_t>(auth_secret.begin(), auth_secret.end()),
               status);

}

void ShellPushMessagingService::DidGetEncryptionInfo(
    const PushMessagingService::EncryptionInfoCallback& callback,
    const std::string& p256dh,
    const std::string& auth_secret) const {
  // DLOG(WARNING) << "p256dh : " <<p256dh;
  callback.Run(true,
               std::vector<uint8_t>(p256dh.begin(), p256dh.end()),
               std::vector<uint8_t>(auth_secret.begin(), auth_secret.end()));
}

void ShellPushMessagingService::SubscribeFromWorker(
    const GURL& requesting_origin,
    int64_t service_worker_registration_id,
    const content::PushSubscriptionOptions& options,
    const RegisterCallback& register_callback) {
  std::string app_id = GetAppId(requesting_origin);
  NormalizeSenderInfo(options.sender_info);
  std::string sender_id = options.sender_info;
  std::vector<std::string> sender_ids;
  sender_ids.push_back(sender_id);
  driver_->AddAppHandler(app_id, this);
  driver_->Register(
      app_id,
      sender_ids,
      base::Bind(&ShellPushMessagingService::DidSubscribe, base::Unretained(this), app_id, sender_id, register_callback));
}

blink::WebPushPermissionStatus ShellPushMessagingService::GetPermissionStatus(
    const GURL& origin,
    bool user_visible) {
  if (!user_visible)
    return blink::WebPushPermissionStatusDenied;

  // Because the Push API is tied to Service Workers, many usages of the API
  // won't have an embedding origin at all. Only consider the requesting
  // |origin| when checking whether permission to use the API has been granted.
  return blink::WebPushPermissionStatusGranted;
}

// GetEncryptionInfo methods ---------------------------------------------------

void ShellPushMessagingService::GetEncryptionInfo(
    const GURL& origin,
    int64_t service_worker_registration_id,
    const std::string& sender_id,
    const PushMessagingService::EncryptionInfoCallback& callback) {
  std::string app_id = GetAppId(origin);
  driver_->AddAppHandler(app_id, this);

  driver_->GetEncryptionInfo(app_id,
    base::Bind(&ShellPushMessagingService::DidGetEncryptionInfo, base::Unretained(this), callback));
}

void ShellPushMessagingService::Unsubscribe(
    const GURL& requesting_origin,
    int64_t service_worker_registration_id,
    const std::string& sender_id,
    const UnregisterCallback& callback) {
  std::string app_id = GetAppId(requesting_origin);
  driver_->Unregister(
      app_id,
      base::Bind(&ShellPushMessagingService::DidUnsubscribe, base::Unretained(this), callback));
}

void ShellPushMessagingService::DidUnsubscribe(
                 const UnregisterCallback& callback,
                 gcm::GCMClient::Result result) {
  callback.Run(ToUnregisterStatus(result));
}

bool ShellPushMessagingService::SupportNonVisibleMessages() {
  return false;
}

// Helper methods --------------------------------------------------------------

std::string ShellPushMessagingService::NormalizeSenderInfo(
    const std::string& sender_info) const {
  // Only encode the |sender_info| when it is a NIST P-256 public key in
  // uncompressed format, verified through its length and the 0x04 prefix byte.
  if (sender_info.size() != 65 || sender_info[0] != 0x04)
    return sender_info;

  std::string encoded_sender_info;
  base::Base64UrlEncode(sender_info, base::Base64UrlEncodePolicy::OMIT_PADDING,
                        &encoded_sender_info);

  return encoded_sender_info;
}

std::string ShellPushMessagingService::GetAppId(
    const GURL& requesting_origin) {
  // Use uppercase GUID for consistency with GUIDs Push has already sent to GCM.
  // Also allows detecting case mangling; see code commented "crbug.com/461867".
  std::string guid = base::ToUpperASCII(base::GenerateGUID());
  // All new push subscriptions are Instance ID tokens.
  guid.replace(guid.size() - kGuidSuffixLength, kGuidSuffixLength,
               kInstanceIDGuidSuffix);
  CHECK(!guid.empty());
  std::string app_id =
      kPushMessagingAppIdentifierPrefix + requesting_origin.spec() + kSeparator + guid;
  return app_id;
}