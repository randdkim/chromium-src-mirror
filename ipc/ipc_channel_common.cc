// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "ipc/ipc_channel.h"
#include "ipc/ipc_channel_mojo.h"

namespace IPC {

// static
std::unique_ptr<Channel> Channel::CreateClient(
    const IPC::ChannelHandle& channel_handle,
    Listener* listener) {
  if (channel_handle.mojo_handle.is_valid()) {
    return ChannelMojo::Create(
        mojo::ScopedMessagePipeHandle(channel_handle.mojo_handle),
        Channel::MODE_CLIENT, listener);
  }
  return Channel::Create(channel_handle, Channel::MODE_CLIENT, listener);
}

// static
std::unique_ptr<Channel> Channel::CreateNamedServer(
    const IPC::ChannelHandle& channel_handle,
    Listener* listener) {
  return Channel::Create(channel_handle, Channel::MODE_NAMED_SERVER, listener);
}

// static
std::unique_ptr<Channel> Channel::CreateNamedClient(
    const IPC::ChannelHandle& channel_handle,
    Listener* listener) {
  return Channel::Create(channel_handle, Channel::MODE_NAMED_CLIENT, listener);
}

#if defined(OS_POSIX)
// static
std::unique_ptr<Channel> Channel::CreateOpenNamedServer(
    const IPC::ChannelHandle& channel_handle,
    Listener* listener) {
  return Channel::Create(channel_handle, Channel::MODE_OPEN_NAMED_SERVER,
                         listener);
}
#endif

// static
std::unique_ptr<Channel> Channel::CreateServer(
    const IPC::ChannelHandle& channel_handle,
    Listener* listener) {
  if (channel_handle.mojo_handle.is_valid()) {
    return ChannelMojo::Create(
        mojo::ScopedMessagePipeHandle(channel_handle.mojo_handle),
        Channel::MODE_SERVER, listener);
  }
  return Channel::Create(channel_handle, Channel::MODE_SERVER, listener);
}

Channel::~Channel() {
}

bool Channel::IsSendThreadSafe() const {
  return false;
}

void Channel::OnSetAttachmentBrokerEndpoint() {
  CHECK(!did_start_connect_);
}

void Channel::WillConnect() {
  did_start_connect_ = true;
}

}  // namespace IPC

