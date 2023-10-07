// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>

// key buffer for passing key to console.
#define MAX_KEYS_TO_BUFFER   32
#define KEYPAD_TIMER_INTERVAL 500000  //50 milli seconds

EFI_INPUT_KEY  keyBuffer[MAX_KEYS_TO_BUFFER] = {0};
EFI_INPUT_KEY* keyBufferRead;
EFI_INPUT_KEY* keyBufferWrite;

EFI_INPUT_KEY  TimerEfiKeyBuffer[MAX_KEYS_TO_BUFFER] = {0};
EFI_INPUT_KEY* TimerEfiKeyBufferRead;
EFI_INPUT_KEY* TimerEfiKeyBufferWrite;


// Number of buttons
UINT8     numOfkeys;
UINT8     numOfKeyRegistrations;

#define   MAX_KEYS_PRESSED_RELEASED  8

UINT8     matrixA[MAX_KEYS_PRESSED_RELEASED];
UINT8     matrixB[MAX_KEYS_PRESSED_RELEASED];
UINT8    *pCurrButtonArray = matrixA;
UINT8    *pPrevButtonArray = matrixB;
BOOLEAN   isChanged = FALSE;
BOOLEAN   TimerRKStroke = FALSE;
BOOLEAN   EfiKeyFound = FALSE;
BOOLEAN   TimerRunning = FALSE;

// currently pressed keys storage
KEY_TYPE  keysPressed[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfKeysPressed = 0;

// currently just released key storage
KEY_TYPE  keysReleased[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfKeysReleased = 0;

// currently detected UEFI key storage
EFI_INPUT_KEY EfiKeys[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfEfiKeys = 0;

// key map storage
KEY_TYPE  keyMap[MAX_KEYS_PRESSED_RELEASED];

// To store the NotifyEntry of each notification.
LIST_ENTRY     *NotifyList;

EFI_STATUS EFIAPI ButtonsInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
) {
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((EFI_D_INFO, "Welcome to ButtonsDxe\n"));

  // Tu código de inicialización de botones va aquí
  

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
