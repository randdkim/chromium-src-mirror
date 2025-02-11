// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/pending_extension_info.h"

#include "base/logging.h"

namespace extensions {

PendingExtensionInfo::PendingExtensionInfo(
    const std::string& id,
    const std::string& install_parameter,
    const GURL& update_url,
    const Version& version,
    ShouldAllowInstallPredicate should_allow_install,
    bool is_from_sync,
    Manifest::Location install_source,
    bool mark_acknowledged,
    bool remote_install)
    : id_(id),
      update_url_(update_url),
      version_(version),
      install_parameter_(install_parameter),
      should_allow_install_(should_allow_install),
      is_from_sync_(is_from_sync),
      install_source_(install_source),
      mark_acknowledged_(mark_acknowledged),
      remote_install_(remote_install) {
}

PendingExtensionInfo::PendingExtensionInfo()
    : update_url_(),
      should_allow_install_(NULL),
      is_from_sync_(true),
      install_source_(Manifest::INVALID_LOCATION),
      mark_acknowledged_(false),
      remote_install_(false) {
}

PendingExtensionInfo::PendingExtensionInfo(const PendingExtensionInfo& other) =
    default;

PendingExtensionInfo::~PendingExtensionInfo() {}

bool PendingExtensionInfo::operator==(const PendingExtensionInfo& rhs) const {
  return id_ == rhs.id_;
}

int PendingExtensionInfo::CompareTo(const PendingExtensionInfo& other) const {
  DCHECK_EQ(id_, other.id_);
  if (version_.IsValid() && other.version_.IsValid()) {
    int comparison = version_.CompareTo(other.version_);

    // If the versions differ then return the version comparison result.
    if (comparison != 0)
      return comparison;
  }

  // The versions aren't specified, or they are the same version. Check
  // the install source.
  if (install_source_ == other.install_source_) {
    // Same install source, so |this| has the same precedence as |other|.
    return 0;
  }

  // Different install sources; |this| has higher precedence if
  // |install_source_| is the higher priority source.
  Manifest::Location higher_priority_source =
      Manifest::GetHigherPriorityLocation(
          install_source_, other.install_source_);

  return higher_priority_source == install_source_ ? 1 : -1;
}

}  // namespace extensions
