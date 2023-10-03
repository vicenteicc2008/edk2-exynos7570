// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
UsbDxeInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

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
