// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_SHELL_PUSH_MESSAGING_SERVICE_IMPL_H_
#define CONTENT_SHELL_BROWSER_SHELL_PUSH_MESSAGING_SERVICE_IMPL_H_

#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/content_settings/core/browser/content_settings_observer.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/gcm_driver/common/gcm_messages.h"
#include "components/gcm_driver/gcm_app_handler.h"
#include "components/gcm_driver/gcm_client.h"
#include "components/gcm_driver/instance_id/instance_id.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/push_messaging_service.h"
#include "content/public/common/push_event_payload.h"
#include "content/public/common/push_messaging_status.h"
#include "third_party/WebKit/public/platform/modules/permissions/permission_status.mojom.h"
#include "third_party/WebKit/public/platform/modules/push_messaging/WebPushPermissionStatus.h"

class PushMessagingAppIdentifier;
class PushMessagingServiceObserver;
class PushMessagingServiceTest;
class ScopedKeepAlive;
struct PushSubscriptionOptions;
class PrefService;
class HostContentSettingsMap;

namespace gcm {
class GCMDriver;
}
namespace instance_id {
class InstanceIDDriver;
}

namespace user_prefs {
class PrefRegistrySyncable;
}

class ShellPushMessagingService : public content::PushMessagingService,
                                  public gcm::GCMAppHandler {
 public:

  ShellPushMessagingService();
  ~ShellPushMessagingService() override;

  // gcm::GCMAppHandler implementation.
  void ShutdownHandler() override;
  void OnMessage(const std::string& app_id,
                 const gcm::IncomingMessage& message) override;
  void OnMessagesDeleted(const std::string& app_id) override;
  void OnSendError(
      const std::string& app_id,
      const gcm::GCMClient::SendErrorDetails& send_error_details) override;
  void OnSendAcknowledged(const std::string& app_id,
                          const std::string& message_id) override;
  bool CanHandle(const std::string& app_id) const override;

  // content::PushMessagingService implementation:
  GURL GetEndpoint(bool standard_protocol) const override;
  void SubscribeFromDocument(const GURL& requesting_origin,
                             int64_t service_worker_registration_id,
                             int renderer_id,
                             int render_frame_id,
                             const content::PushSubscriptionOptions& options,
                             const RegisterCallback& callback) override;
  void SubscribeFromWorker(const GURL& requesting_origin,
                           int64_t service_worker_registration_id,
                           const content::PushSubscriptionOptions& options,
                           const RegisterCallback& callback) override;
  void GetEncryptionInfo(const GURL& origin,
                         int64_t service_worker_registration_id,
                         const std::string& sender_id,
                         const EncryptionInfoCallback& callback) override;
  void Unsubscribe(const GURL& requesting_origin,
                   int64_t service_worker_registration_id,
                   const std::string& sender_id,
                   const UnregisterCallback&) override;
  blink::WebPushPermissionStatus GetPermissionStatus(
      const GURL& origin,
      bool user_visible) override;
  bool SupportNonVisibleMessages() override;

// Helper methods --------------------------------------------------------------

  std::string NormalizeSenderInfo(
    const std::string& sender_info) const;

 private:
  void DidSubscribe(
    const std::string& app_id,
    const std::string& sender_id,
    const RegisterCallback& callback,
    const std::string& subscription_id,
    gcm::GCMClient::Result result);
  void DidSubscribeWithEncryptionInfo(
    const RegisterCallback& callback,
    const std::string& subscription_id,
    content::PushRegistrationStatus status,
    const std::string& p256dh,
    const std::string& auth_secret);
  // void DidGetEncryptionInfo(
  //   const std::string& app_id,
  //   const std::string& sender_id,
  //   const PushMessagingService::EncryptionInfoCallback& callback,
  //   const std::string& registration_id,
  //   gcm::GCMClient::Result result);
  void DidGetEncryptionInfo(
    const PushMessagingService::EncryptionInfoCallback& callback,
    const std::string& p256dh,
    const std::string& auth_secret) const;
  void DidUnsubscribe(
                 const UnregisterCallback& callback,
                 gcm::GCMClient::Result result);
  void SubscribeEndWithError(
    const RegisterCallback& callback,
    content::PushRegistrationStatus status);
  std::string GetAppId(const GURL& requesting_origin);
  std::unique_ptr<gcm::GCMDriver> driver_;
  std::unique_ptr<PrefService> prefs_;
  
  // scoped_refptr<base::SequencedTaskRunner> task_runner_;

  base::WeakPtrFactory<ShellPushMessagingService> weak_factory_;
  DISALLOW_COPY_AND_ASSIGN(ShellPushMessagingService);
};

#endif  // CONTENT_SHELL_BROWSER_SHELL_PUSH_MESSAGING_SERVICE_IMPL_H_
