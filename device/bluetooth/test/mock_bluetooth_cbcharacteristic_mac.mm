// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/test/mock_bluetooth_cbcharacteristic_mac.h"

#include "base/mac/foundation_util.h"
#include "base/mac/scoped_nsobject.h"
#include "device/bluetooth/bluetooth_gatt_characteristic.h"

using base::mac::ObjCCast;
using base::scoped_nsobject;

namespace device {

namespace {

CBCharacteristicProperties AddCBCharacteristicProperties(
    CBCharacteristicProperties value1,
    CBCharacteristicProperties value2) {
  return static_cast<CBCharacteristicProperties>(value1 | value2);
}

CBCharacteristicProperties GattCharacteristicPropertyToCBCharacteristicProperty(
    BluetoothGattCharacteristic::Properties gatt_property) {
  CBCharacteristicProperties result =
      static_cast<CBCharacteristicProperties>(0);
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_BROADCAST) {
    result = AddCBCharacteristicProperties(result,
                                           CBCharacteristicPropertyBroadcast);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_READ) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyRead);
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_WRITE_WITHOUT_RESPONSE) {
    result = AddCBCharacteristicProperties(
        result, CBCharacteristicPropertyWriteWithoutResponse);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_WRITE) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyWrite);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_NOTIFY) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyNotify);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_INDICATE) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyIndicate);
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_AUTHENTICATED_SIGNED_WRITES) {
    result = AddCBCharacteristicProperties(
        result, CBCharacteristicPropertyAuthenticatedSignedWrites);
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_EXTENDED_PROPERTIES) {
    result = AddCBCharacteristicProperties(
        result, CBCharacteristicPropertyExtendedProperties);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_RELIABLE_WRITE) {
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_WRITABLE_AUXILIARIES) {
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_READ_ENCRYPTED) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyRead);
  }
  if (gatt_property & BluetoothGattCharacteristic::PROPERTY_WRITE_ENCRYPTED) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyWrite);
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_READ_ENCRYPTED_AUTHENTICATED) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyRead);
  }
  if (gatt_property &
      BluetoothGattCharacteristic::PROPERTY_WRITE_ENCRYPTED_AUTHENTICATED) {
    result =
        AddCBCharacteristicProperties(result, CBCharacteristicPropertyWrite);
  }
  return result;
}
}  // namespace
}  // device

@interface MockCBCharacteristic () {
  // Owner of this instance.
  CBService* _service;
  scoped_nsobject<CBUUID> _UUID;
  CBCharacteristicProperties _cb_properties;
  base::scoped_nsobject<NSData> _value;
}
@end

@implementation MockCBCharacteristic

- (instancetype)initWithService:(CBService*)service
                         CBUUID:(CBUUID*)uuid
                     properties:(int)properties {
  self = [super init];
  if (self) {
    _service = service;
    _UUID.reset([uuid retain]);
    _cb_properties =
        device::GattCharacteristicPropertyToCBCharacteristicProperty(
            properties);
  }
  return self;
}

- (BOOL)isKindOfClass:(Class)aClass {
  if (aClass == [CBCharacteristic class] ||
      [aClass isSubclassOfClass:[CBCharacteristic class]]) {
    return YES;
  }
  return [super isKindOfClass:aClass];
}

- (BOOL)isMemberOfClass:(Class)aClass {
  if (aClass == [CBCharacteristic class] ||
      [aClass isSubclassOfClass:[CBCharacteristic class]]) {
    return YES;
  }
  return [super isKindOfClass:aClass];
}

- (void)simulateReadWithValue:(NSData*)value error:(NSError*)error {
  _value.reset([value copy]);
  CBPeripheral* peripheral = _service.peripheral;
  [peripheral.delegate peripheral:peripheral
      didUpdateValueForCharacteristic:self.characteristic
                                error:error];
}

- (void)simulateWriteWithError:(NSError*)error {
  CBPeripheral* peripheral = _service.peripheral;
  [peripheral.delegate peripheral:peripheral
      didWriteValueForCharacteristic:self.characteristic
                               error:error];
}

- (CBUUID*)UUID {
  return _UUID.get();
}

- (CBCharacteristic*)characteristic {
  return ObjCCast<CBCharacteristic>(self);
}

- (CBService*)service {
  return _service;
}

- (CBCharacteristicProperties)properties {
  return _cb_properties;
}

- (NSData*)value {
  return _value.get();
}

@end