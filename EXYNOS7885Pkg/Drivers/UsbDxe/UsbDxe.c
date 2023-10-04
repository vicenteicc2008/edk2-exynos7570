// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/ExynosUsbDevice.h>

EFI_STATUS
EFIAPI
Usb20PeriphStart (
  IN USB_DEVICE_DESCRIPTOR   *DeviceDescriptor,
  IN VOID                   **Descriptors,
  IN USB_DEVICE_EVENT_CALLBACK  EventCallback
  )
{
  UINT8                    *Ptr;

  ASSERT (DeviceDescriptor != NULL);
  ASSERT (Descriptors[0] != NULL);
  ASSERT (EventCallback != NULL);

  mDeviceDescriptor = DeviceDescriptor;
  mDescriptors = Descriptors[0];

  // Right now we just support one configuration
  ASSERT (mDeviceDescriptor->NumConfigurations == 1);
  // ... and one interface
  mConfigDescriptor = (USB_CONFIG_DESCRIPTOR *)mDescriptors;
  ASSERT (mConfigDescriptor->NumInterfaces == 1);

  Ptr = ((UINT8 *) mDescriptors) + sizeof (USB_CONFIG_DESCRIPTOR);
  mInterfaceDescriptor = (USB_INTERFACE_DESCRIPTOR *) Ptr;
  Ptr += sizeof (USB_INTERFACE_DESCRIPTOR);

  mEndpointDescriptors = (USB_ENDPOINT_DESCRIPTOR *) Ptr;

  mEventCallback = EventCallback;

  return usb_init();
}

EFI_STATUS
Usb20PeriphStop (
  VOID
  )
{
  mUsbIf.udc_stop();
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
Usb20PeriphTransfer (
  IN        UINT8  EndpointIndex,
  IN        UINTN  Size,
  IN        VOID  *Buffer
  )
{
  REQUEST_ENTRY *RequestEntry = AllocatePool (sizeof(REQUEST_ENTRY));
  if (!RequestEntry)
    return EFI_OUT_OF_RESOURCES;

  RequestEntry->EndpointIndex = EndpointIndex;
  RequestEntry->req = mUdcRequest;
  RequestEntry->buf = Buffer;
  RequestEntry->len = Size;
  RequestEntry->bytes_written = 0;

  if (mEndpoints[RequestEntry->EndpointIndex].IsIn)
    // flush buffer to main memory before giving to udc
    arch_clean_invalidate_cache_range((UINTN) RequestEntry->buf, RequestEntry->len);
  else
    // discard the cache contents before starting the download
    arch_invalidate_cache_range((UINTN) RequestEntry->buf, RequestEntry->len);

  EfiAcquireLock(&mLock);
  InsertTailList (&mRequestQueue, &RequestEntry->Link);
  if (mRequestState == REQ_STATE_IDLE)
    req_queue_request(RequestEntry);
  EfiReleaseLock(&mLock);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UsbDeviceAllocateTransferBuffer (
  IN   UINTN                  Size,
  OUT  VOID                 **Buffer
)
{
  VOID *Memory = memalign(CACHE_LINE, ROUNDUP(Size, CACHE_LINE));
  if (!Memory) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Buffer = Memory;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UsbDeviceFreeTransferBuffer (
  IN VOID                   *Buffer
)
{
  free(Buffer);
  return EFI_SUCCESS;
}

STATIC USB_DEVICE_PROTOCOL mUsbDevice = {
  Usb20PeriphStart,
  Usb20PeriphStop,
  Usb20PeriphTransfer,
  UsbDeviceAllocateTransferBuffer,
  UsbDeviceFreeTransferBuffer
};

EFI_STATUS
EFIAPI
UsbDxeInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HANDLE  Handle;

  // Tu código de inicialización del driver va aquí

  // Ejemplo: Imprimir un mensaje de depuración
  DEBUG((EFI_D_INFO, "¡Hello from UsbDxe!\n"));

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  ReportEvents,
                  NULL,
                  &mEventsAvailable
                  );
  ASSERT_EFI_ERROR (Status);

  Handle = NULL;
  return gBS->InstallProtocolInterface (
    &Handle,
    &gExynosUsbDeviceProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &mUsbDevice
    );

}
