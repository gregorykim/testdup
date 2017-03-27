
#ifndef CONTENT_BROWSER_NOTIFICATIONS_NOTIFICATION_PERMISSION_DATA_H_
#define CONTENT_BROWSER_NOTIFICATIONS_NOTIFICATION_PERMISSION_DATA_H_
namespace content {

class NotificationPermissionData {
  std::string origin_;
  bool permission_;
  NotificationPermissionData(std::string origin, bool permission) {
    origin_ = origin;
    permission_ = permission;
  }
};

}
#endif