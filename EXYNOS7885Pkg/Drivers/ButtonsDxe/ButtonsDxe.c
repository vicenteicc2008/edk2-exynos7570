// Copyright (C) 2016, Samsung Electronics. All rights reserved.<BR>
// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/ExynosButtons.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>

#define EXYNOS_KEYPAD_DEVICE_GUID \
    { 0xD7F58A0E, 0xBED2, 0x4B5A, { 0xBB, 0x43, 0x8A, 0xB2, 0x3D, 0xD0, 0xE2, 0xB0} }

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

typedef struct _KEYPAD_CONSOLE_IN_EX_NOTIFY {
  EFI_HANDLE                            NotifyHandle;
  EFI_KEY_DATA                          KeyData;
  EFI_KEY_NOTIFY_FUNCTION               KeyNotificationFn;
  LIST_ENTRY                            NotifyEntry;
} KEYPAD_CONSOLE_IN_EX_NOTIFY;

/* call back periodic timer maintained by the driver
   in order to poll PMIC and inform the clients registered
   with keypad driver about the status of input key. */
EFI_EVENT      CallbackTimer;

// EFI_SIMPLE_TEXT_INPUT_PROTOCOL implementation
EFI_STATUS KeysDxeReset(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
  IN BOOLEAN                        ExtendedVerification );

EFI_STATUS KeysDxeReadKeyStroke(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
  OUT EFI_INPUT_KEY                   *Key);

EFI_SIMPLE_TEXT_INPUT_PROTOCOL KeypadSimpleInputProtocolImplementation = {
   KeysDxeReset,
   KeysDxeReadKeyStroke,
   0    //WaitForKey
};

/* Protos for EfiSimpleTextInputExProtocol implementation */
EFI_STATUS
EFIAPI
KeysDxeResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification);

EFI_STATUS
EFIAPI
KeysDxeReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData);

EFI_STATUS
EFIAPI
KeysDxeSetStateEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState);

EFI_STATUS
EFIAPI
KeysDxeRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN  EFI_KEY_DATA                       *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                         *NotifyHandle);

EFI_STATUS
EFIAPI
KeysDxeUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle);

BOOLEAN
EFIAPI
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegisteredData,
  IN EFI_KEY_DATA  *InputData );

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL KeypadSimpleInputExProtocolImplementation = {
   KeysDxeResetEx,
   KeysDxeReadKeyStrokeEx,
   0,                          //  WaitForKeyEx
   KeysDxeSetStateEx,
   KeysDxeRegisterKeyNotify,
   KeysDxeUnregisterKeyNotify,
};

BOOLEAN StoreAKey(
  KEY_TYPE *pArray,
  KEY_TYPE key
)
{
  UINT8 i;
  for( i=0; i<MAX_KEYS_PRESSED_RELEASED; i++ )
  {
    if( pArray[i] == NONE )
    {
      pArray[i] = key;
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Remove a local key from an array.

  @param  pArray         An array pointer to which the key will be removed from.
  @param  key            The key will be removed.

  @retval TRUE           Remove successfully.
  @retval FALSE          Remove failed.

**/
BOOLEAN RemoveAKey(
  KEY_TYPE *pArray,
  KEY_TYPE key
)
{
  UINT8 i;
  for( i=0; i<MAX_KEYS_PRESSED_RELEASED; i++ )
  {
    if( pArray[i] == key )
    {
      pArray[i] = NONE;
      return TRUE;
    }
  }
  return FALSE;
}

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
