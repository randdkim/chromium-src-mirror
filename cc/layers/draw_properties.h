// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DRAW_PROPERTIES_H_
#define CC_LAYERS_DRAW_PROPERTIES_H_

#include <stddef.h>

#include <memory>

#include "cc/trees/occlusion.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/transform.h"

namespace cc {

// Container for properties that layers need to compute before they can be
// drawn.
struct CC_EXPORT DrawProperties {
  DrawProperties();
  ~DrawProperties();

  // Transforms objects from content space to target surface space, where
  // this layer would be drawn.
  gfx::Transform target_space_transform;

  // Transforms objects from content space to screen space (viewport space).
  gfx::Transform screen_space_transform;

  // Known occlusion above the layer mapped to the content space of the layer.
  Occlusion occlusion_in_content_space;

  // DrawProperties::opacity may be different than LayerImpl::opacity,
  // particularly in the case when a RenderSurface re-parents the layer's
  // opacity, or when opacity is compounded by the hierarchy.
  float opacity;

  // xxx_is_animating flags are used to indicate whether the DrawProperties
  // are actually meaningful on the main thread. When the properties are
  // animating, the main thread may not have the same values that are used
  // to draw.
  bool screen_space_transform_is_animating;

  // True if the layer needs to be clipped by clip_rect.
  bool is_clipped;

  // This rect is a bounding box around what part of the layer is visible, in
  // the layer's coordinate space.
  gfx::Rect visible_layer_rect;

  // In target surface space, the rect that encloses the clipped, drawable
  // content of the layer.
  gfx::Rect drawable_content_rect;

  // In target surface space, the original rect that clipped this layer. This
  // value is used to avoid unnecessarily changing GL scissor state.
  gfx::Rect clip_rect;

  // The maximum scale during the layers current animation at which content
  // should be rastered at to be crisp.
  float maximum_animation_contents_scale;

  // The scale during the layer animation start at which content should be
  // rastered at to be crisp.
  float starting_animation_contents_scale;
};

}  // namespace cc

#endif  // CC_LAYERS_DRAW_PROPERTIES_H_
