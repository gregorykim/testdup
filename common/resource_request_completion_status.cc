// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/resource_request_completion_status.h"

namespace content {

ResourceRequestCompletionStatus::ResourceRequestCompletionStatus() {}
ResourceRequestCompletionStatus::ResourceRequestCompletionStatus(
    const ResourceRequestCompletionStatus& status) = default;
ResourceRequestCompletionStatus::~ResourceRequestCompletionStatus() {}

}  // namespace content
