// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_COMMON_WM_OVERVIEW_OVERVIEW_ANIMATION_TYPE_H_
#define ASH_COMMON_WM_OVERVIEW_OVERVIEW_ANIMATION_TYPE_H_

namespace ash {

// Enumeration of the different overview mode animations.
enum OverviewAnimationType {
  // TODO(bruthig): Remove OVERVIEW_ANIMATION_NONE value and replace it with
  // correct animation type actions.
  OVERVIEW_ANIMATION_NONE,
  // Used to fade in the close button and label.
  OVERVIEW_ANIMATION_ENTER_OVERVIEW_MODE_FADE_IN,
  // Used to position windows when entering/exiting overview mode and when a
  // window is closed while overview mode is active.
  OVERVIEW_ANIMATION_LAY_OUT_SELECTOR_ITEMS,
  // Used to hide non-overview mode windows.
  OVERVIEW_ANIMATION_HIDE_WINDOW,
  // Used to restore windows to their original position when exiting overview
  // mode.
  OVERVIEW_ANIMATION_RESTORE_WINDOW
};

}  // namespace ash

#endif  // ASH_COMMON_WM_OVERVIEW_OVERVIEW_ANIMATION_TYPE_H_
