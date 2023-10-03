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

  // Ejemplo: Agregar tus interfaces de protocolo
  // EFI_HANDLE Handle;
  // Status = gBS->InstallProtocolInterface (
  //   &Handle,
  //   &gYourProtocolGuid,
  //   EFI_NATIVE_INTERFACE,
  //   YourProtocolInterface
  // );

  // Comprobar errores y devolver el estado
  if (EFI_ERROR(Status)) {
    // Manejar casos de error
    DEBUG((EFI_D_ERROR, "UsbDxeInit failed: %r\n", Status));
  }

  return EFI_SUCCESS;
}
