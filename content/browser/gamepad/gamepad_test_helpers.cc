// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/gamepad/gamepad_test_helpers.h"

#include "content/browser/gamepad/gamepad_service.h"

namespace content {

MockGamepadDataFetcher::MockGamepadDataFetcher(
    const blink::WebGamepads& test_data)
    : test_data_(test_data),
      read_data_(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                 base::WaitableEvent::InitialState::NOT_SIGNALED) {}

MockGamepadDataFetcher::~MockGamepadDataFetcher() {
}

void MockGamepadDataFetcher::GetGamepadData(blink::WebGamepads* pads,
                                            bool devices_changed_hint) {
  {
    base::AutoLock lock(lock_);
    *pads = test_data_;
  }
  read_data_.Signal();
}

void MockGamepadDataFetcher::WaitForDataRead() {
  return read_data_.Wait();
}

void MockGamepadDataFetcher::WaitForDataReadAndCallbacksIssued() {
  // The provider will read the data on the background thread (setting the
  // event) and *then* will issue the callback on the client thread. Waiting for
  // it to read twice is a simple way to ensure that it was able to issue
  // callbacks for the first read (if it issued one).
  WaitForDataRead();
  WaitForDataRead();
}

void MockGamepadDataFetcher::SetTestData(const blink::WebGamepads& new_data) {
  base::AutoLock lock(lock_);
  test_data_ = new_data;
}

GamepadTestHelper::GamepadTestHelper() {
}

GamepadTestHelper::~GamepadTestHelper() {
}

GamepadServiceTestConstructor::GamepadServiceTestConstructor(
    const blink::WebGamepads& test_data) {
  data_fetcher_ = new MockGamepadDataFetcher(test_data);
  gamepad_service_ =
      new GamepadService(std::unique_ptr<GamepadDataFetcher>(data_fetcher_));
}

GamepadServiceTestConstructor::~GamepadServiceTestConstructor() {
  delete gamepad_service_;
}

}  // namespace content
