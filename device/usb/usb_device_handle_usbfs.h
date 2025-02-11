// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_USB_USB_DEVICE_HANDLE_USBFS_H_
#define DEVICE_USB_USB_DEVICE_HANDLE_USBFS_H_

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "base/files/scoped_file.h"
#include "device/usb/usb_device_handle.h"

struct usbdevfs_urb;

namespace base {
class SequencedTaskRunner;
class SingleThreadTaskRunner;
}

namespace device {

// Implementation of a USB device handle on top of the Linux USBFS ioctl
// interface available on Linux, Chrome OS and Android.
class UsbDeviceHandleUsbfs : public UsbDeviceHandle {
 public:
  // Constructs a new device handle from an existing |device| and open file
  // descriptor to that device. |blocking_task_runner| must have a
  // MessageLoopForIO.
  UsbDeviceHandleUsbfs(
      scoped_refptr<UsbDevice> device,
      base::ScopedFD fd,
      scoped_refptr<base::SequencedTaskRunner> blocking_task_runner);

  // UsbDeviceHandle implementation.
  scoped_refptr<UsbDevice> GetDevice() const override;
  void Close() override;
  void SetConfiguration(int configuration_value,
                        const ResultCallback& callback) override;
  void ClaimInterface(int interface_number,
                      const ResultCallback& callback) override;
  void ReleaseInterface(int interface_number,
                        const ResultCallback& callback) override;
  void SetInterfaceAlternateSetting(int interface_number,
                                    int alternate_setting,
                                    const ResultCallback& callback) override;
  void ResetDevice(const ResultCallback& callback) override;
  void ClearHalt(uint8_t endpoint, const ResultCallback& callback) override;
  void ControlTransfer(UsbEndpointDirection direction,
                       TransferRequestType request_type,
                       TransferRecipient recipient,
                       uint8_t request,
                       uint16_t value,
                       uint16_t index,
                       scoped_refptr<net::IOBuffer> buffer,
                       size_t length,
                       unsigned int timeout,
                       const TransferCallback& callback) override;
  void IsochronousTransferIn(
      uint8_t endpoint_number,
      const std::vector<uint32_t>& packet_lengths,
      unsigned int timeout,
      const IsochronousTransferCallback& callback) override;
  void IsochronousTransferOut(
      uint8_t endpoint_number,
      scoped_refptr<net::IOBuffer> buffer,
      const std::vector<uint32_t>& packet_lengths,
      unsigned int timeout,
      const IsochronousTransferCallback& callback) override;
  // To support DevTools this function may be called from any thread and on
  // completion |callback| will be run on that thread.
  void GenericTransfer(UsbEndpointDirection direction,
                       uint8_t endpoint_number,
                       scoped_refptr<net::IOBuffer> buffer,
                       size_t length,
                       unsigned int timeout,
                       const TransferCallback& callback) override;
  const UsbInterfaceDescriptor* FindInterfaceByEndpoint(
      uint8_t endpoint_address) override;

 protected:
  ~UsbDeviceHandleUsbfs() override;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner() const {
    return task_runner_;
  }

  // Stops |helper_| and releases ownership of |fd_| without closing it.
  void ReleaseFileDescriptor();

 private:
  class FileThreadHelper;
  struct Transfer;
  struct InterfaceInfo {
    uint8_t alternate_setting;
  };
  struct EndpointInfo {
    UsbTransferType type;
    const UsbInterfaceDescriptor* interface;
  };

  virtual void CloseBlocking();
  void SetConfigurationBlocking(int configuration_value,
                                const ResultCallback& callback);
  void SetConfigurationComplete(int configuration_value,
                                bool success,
                                const ResultCallback& callback);
  void ReleaseInterfaceBlocking(int interface_number,
                                const ResultCallback& callback);
  void ReleaseInterfaceComplete(int interface_number,
                                const ResultCallback& callback);
  void SetInterfaceBlocking(int interface_number,
                            int alternate_setting,
                            const ResultCallback& callback);
  void ResetDeviceBlocking(const ResultCallback& callback);
  void ClearHaltBlocking(uint8_t endpoint_address,
                         const ResultCallback& callback);
  void IsochronousTransferInternal(uint8_t endpoint_address,
                                   scoped_refptr<net::IOBuffer> buffer,
                                   size_t total_length,
                                   const std::vector<uint32_t>& packet_lengths,
                                   unsigned int timeout,
                                   const IsochronousTransferCallback& callback);
  void GenericTransferInternal(
      UsbEndpointDirection direction,
      uint8_t endpoint_number,
      scoped_refptr<net::IOBuffer> buffer,
      size_t length,
      unsigned int timeout,
      const TransferCallback& callback,
      scoped_refptr<base::SingleThreadTaskRunner> callback_runner);
  void ReapedUrbs(const std::vector<usbdevfs_urb*>& urbs);
  void TransferComplete(std::unique_ptr<Transfer> transfer);
  void RefreshEndpointInfo();
  void ReportIsochronousError(
      const std::vector<uint32_t>& packet_lengths,
      const UsbDeviceHandle::IsochronousTransferCallback& callback,
      UsbTransferStatus status);
  void SetUpTimeoutCallback(Transfer* transfer, unsigned int timeout);

  static void CancelTransfer(Transfer* transfer, UsbTransferStatus status);

  scoped_refptr<UsbDevice> device_;
  base::ScopedFD fd_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  scoped_refptr<base::SequencedTaskRunner> blocking_task_runner_;

  // Maps claimed interfaces by interface number to their current alternate
  // setting.
  std::map<uint8_t, InterfaceInfo> interfaces_;

  // Maps endpoints (by endpoint address) to the interface they are a part of.
  // Only endpoints of currently claimed and selected interface alternates are
  // included in the map.
  std::map<uint8_t, EndpointInfo> endpoints_;

  // Helper object owned by the blocking task thread. It will be freed if that
  // thread's message loop is destroyed but can also be freed by this class on
  // destruction.
  FileThreadHelper* helper_;

  std::list<std::unique_ptr<Transfer>> transfers_;
};

}  // namespace device

#endif  // DEVICE_USB_USB_DEVICE_HANDLE_USBFS_H_
