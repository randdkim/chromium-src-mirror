// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_BLUEZ_BLUETOOTH_SERVICE_RECORD_BLUEZ_H_
#define DEVICE_BLUETOOTH_BLUEZ_BLUETOOTH_SERVICE_RECORD_BLUEZ_H_

#include <cstdint>
#include <map>
#include <vector>

#include "base/macros.h"
#include "device/bluetooth/bluetooth_export.h"
#include "device/bluetooth/bluez/bluetooth_service_attribute_value_bluez.h"

namespace base {
class Value;
}

namespace bluez {

class DEVICE_BLUETOOTH_EXPORT BluetoothServiceRecordBlueZ {
 public:
  // Possible types of errors raised when creating, removing or getting service
  // records.
  enum ErrorCode {
    ERROR_ADAPTER_NOT_READY,
    ERROR_RECORD_ALREADY_EXISTS,
    ERROR_RECORD_DOES_NOT_EXIST,
    ERROR_DEVICE_DISCONNECTED,
    ERROR_INVALID_ARGUMENTS,
    UNKNOWN
  };

  explicit BluetoothServiceRecordBlueZ(
      const std::map<uint16_t, BluetoothServiceAttributeValueBlueZ>&
          attributes);
  BluetoothServiceRecordBlueZ(const BluetoothServiceRecordBlueZ& record);
  ~BluetoothServiceRecordBlueZ();

  // Returns a list of Attribute IDs that exist within this service record.
  const std::vector<uint16_t> GetAttributeIds() const;
  // Returns the value associated with a given attribute ID in |value|.
  const BluetoothServiceAttributeValueBlueZ& GetAttributeValue(
      uint16_t attribute_id) const;

 private:
  std::map<uint16_t, BluetoothServiceAttributeValueBlueZ> attributes_;
};

}  // namespace bluez

#endif  // DEVICE_BLUETOOTH_BLUEZ_BLUETOOTH_SERVICE_RECORD_BLUEZ_H_
