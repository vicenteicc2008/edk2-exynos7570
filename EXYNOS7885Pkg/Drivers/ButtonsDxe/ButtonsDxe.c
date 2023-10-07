// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>

EFI_STATUS ButtonsInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_STATUS  Status;

    DEBUG((EFI_D_INFO, "Hello from ButtonsDxe"));

	if (EFI_ERROR (Status))
    {
      DEBUG(( EFI_D_INFO, "ButtonsDxeInitialize: ButtonsDxeInitialize failed, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
    }

	ErrorExit:
      return Status;
