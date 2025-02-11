/* Generated by wayland-scanner 1.11.0 */

/*
 * Copyright 2016 The Chromium Authors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include "wayland-util.h"

extern const struct wl_interface wl_surface_interface;
extern const struct wl_interface zwp_notification_surface_v1_interface;
extern const struct wl_interface zwp_remote_surface_v1_interface;

static const struct wl_interface *types[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&zwp_remote_surface_v1_interface,
	&wl_surface_interface,
	NULL,
	&zwp_notification_surface_v1_interface,
	&wl_surface_interface,
	NULL,
	&wl_surface_interface,
	&wl_surface_interface,
};

static const struct wl_message zwp_remote_shell_v1_requests[] = {
	{ "destroy", "", types + 0 },
	{ "get_remote_surface", "nou", types + 6 },
	{ "get_notification_surface", "6nos", types + 9 },
};

static const struct wl_message zwp_remote_shell_v1_events[] = {
	{ "configure", "iiiiii", types + 0 },
	{ "activated", "?o?o", types + 12 },
};

WL_EXPORT const struct wl_interface zwp_remote_shell_v1_interface = {
	"zwp_remote_shell_v1", 7,
	3, zwp_remote_shell_v1_requests,
	2, zwp_remote_shell_v1_events,
};

static const struct wl_message zwp_remote_surface_v1_requests[] = {
	{ "destroy", "", types + 0 },
	{ "set_app_id", "s", types + 0 },
	{ "set_window_geometry", "iiii", types + 0 },
	{ "set_scale", "f", types + 0 },
	{ "fullscreen", "2", types + 0 },
	{ "maximize", "2", types + 0 },
	{ "minimize", "2", types + 0 },
	{ "restore", "2", types + 0 },
	{ "pin", "3", types + 0 },
	{ "unpin", "3", types + 0 },
	{ "unfullscreen", "3", types + 0 },
	{ "set_rectangular_shadow", "4iiii", types + 0 },
	{ "set_title", "5s", types + 0 },
	{ "set_top_inset", "5i", types + 0 },
};

static const struct wl_message zwp_remote_surface_v1_events[] = {
	{ "set_fullscreen", "", types + 0 },
	{ "unset_fullscreen", "", types + 0 },
	{ "close", "", types + 0 },
	{ "set_maximized", "2", types + 0 },
	{ "unset_maximized", "2", types + 0 },
	{ "set_minimized", "2", types + 0 },
	{ "unset_minimized", "2", types + 0 },
	{ "set_pinned", "3", types + 0 },
	{ "unset_pinned", "3", types + 0 },
	{ "state_type_changed", "7u", types + 0 },
};

WL_EXPORT const struct wl_interface zwp_remote_surface_v1_interface = {
	"zwp_remote_surface_v1", 7,
	14, zwp_remote_surface_v1_requests,
	10, zwp_remote_surface_v1_events,
};

static const struct wl_message zwp_notification_surface_v1_requests[] = {
	{ "destroy", "", types + 0 },
};

WL_EXPORT const struct wl_interface zwp_notification_surface_v1_interface = {
	"zwp_notification_surface_v1", 6,
	1, zwp_notification_surface_v1_requests,
	0, NULL,
};

