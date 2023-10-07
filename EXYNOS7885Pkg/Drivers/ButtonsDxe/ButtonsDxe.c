// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>

EFI_STATUS EFIAPI ButtonsInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
) {
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((EFI_D_INFO, "Hello from ButtonsDxe\n"));

  // Tu código de inicialización de botones va aquí
  // ...

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ButtonsDxeInitialize: Failed, Status = (0x%x)\n", Status));
  }

  return Status;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  return ButtonsInitialize(ImageHandle, SystemTable);
}
