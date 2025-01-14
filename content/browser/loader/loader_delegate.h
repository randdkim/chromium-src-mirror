// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_LOADER_LOADER_DELEGATE_H_
#define CONTENT_BROWSER_LOADER_LOADER_DELEGATE_H_

#include <inttypes.h>

#include "net/base/load_states.h"

class GURL;

namespace content {

// Delegate from loader to the rest of content. Lives on the IO thread.
//
// This is used for breaking dependencies between content at-large and
// content/browser/loader which will eventually be moved to a separate
// networking service. All methods in this interface should be asynchronous,
// since eventually this will be a Mojo interface. See https://crbug.com/622050
// and https://crbug.com/598073.
class LoaderDelegate {
 public:
  virtual ~LoaderDelegate() {}

  // Notification that the load state for the route identified by child_id and
  // route_id has changed.
  virtual void LoadStateChanged(int child_id,
                                int route_id,
                                const GURL& url,
                                const net::LoadStateWithParam& load_state,
                                uint64_t upload_position,
                                uint64_t upload_size) = 0;
};

}  // namespace content

#endif  // CONTENT_BROWSER_LOADER_LOADER_DELEGATE_H_
