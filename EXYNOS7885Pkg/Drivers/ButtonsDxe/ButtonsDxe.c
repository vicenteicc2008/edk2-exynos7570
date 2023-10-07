// Copyright (C) 2016, Samsung Electronics. All rights reserved.<BR>
// Copyright (C) 2023, vicenteicc2008. All rights reserved.<BR>

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ButtonsLib

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

VOID ProcessArray( VOID )
{
   UINT8      difference = 0;
   UINT8     *p;
   UINT8     KeyIndex;
   KEY_TYPE   key;


   // assume no matrix change first
   isChanged = FALSE;

   for( KeyIndex = 0; KeyIndex < numOfkeys; ++KeyIndex )
   {
      difference = pCurrButtonArray[KeyIndex] ^ pPrevButtonArray[KeyIndex];

      // If the previous and current button arrays differ in data in
      // it means a key at that index changed state

      if (difference) {
         isChanged = TRUE;
         // get local key code
         key = keyMap[KeyIndex];

         //  key is released
         if(!pCurrButtonArray[KeyIndex])
         {

            // remove this key from keyPressed array if there is
            if( RemoveAKey( keysPressed, key ))
            {
               numOfKeysPressed--;
            }
            // store this key in keyReleased array
            if( StoreAKey( keysReleased, key ))
            {
               numOfKeysReleased++;
            }
         }
         else
         {
            // key is pressed
            if( StoreAKey( keysPressed, key ))
            {
               numOfKeysPressed++;
            }
         } // press or release
      } // difference state
   } // for key index loop

   // swap pointers for next comparison session
   p = pCurrButtonArray;
   pCurrButtonArray = pPrevButtonArray;
   pPrevButtonArray = p;

   return;
}

BOOLEAN PollForKey()
{
  UINT8 i;
  EFI_STATUS status;
  EfiKeyFound = FALSE;

  // get the button array
  status = PollButtonArray( pCurrButtonArray );
  if(status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "ButtonsDxe - PollButtonKeys failed status:0x%08X\r\n",status));
    return FALSE;
  }

  // process the matrix
  ProcessArray();

  if( isChanged == TRUE )
  {
    // There is matrix change detected.
    //Translate to  EFI keys and put them into the key buffer.
    if(ConvertEfiKeyCode( keysPressed,
                          numOfKeysPressed,
                          keysReleased,
                          numOfKeysReleased,
                          MAX_KEYS_PRESSED_RELEASED,
                          EfiKeys,
                          &numOfEfiKeys) == EFI_SUCCESS)
    {
      for( i=0; i<numOfEfiKeys; i++ )
      {

        *keyBufferWrite = EfiKeys[i];
        keyBufferWrite++;
        if(keyBufferWrite >= &keyBuffer[MAX_KEYS_TO_BUFFER] )
        {
          keyBufferWrite = &keyBuffer[0];
        }

        if(TimerRunning)
        {
          *TimerEfiKeyBufferWrite = EfiKeys[i];
          TimerEfiKeyBufferWrite++;

          if(TimerEfiKeyBufferWrite >= &TimerEfiKeyBuffer[MAX_KEYS_TO_BUFFER])
          {
             TimerEfiKeyBufferWrite = &TimerEfiKeyBuffer[0];
          }
        }
        EfiKeyFound = TRUE;
      }
    }

    //Clear the keys released.
    SetMem(keysReleased, MAX_KEYS_PRESSED_RELEASED, NONE);
    numOfKeysReleased = 0;
    SetMem(EfiKeys, MAX_KEYS_PRESSED_RELEASED, NONE);
    numOfEfiKeys = 0;
  }

  //Returns TRUE if any key was found.  FALSE if not.
  return EfiKeyFound;
}

BOOLEAN IsKeyInBuffer()
{
  return (keyBufferRead != keyBufferWrite);
}

VOID EFIAPI WaitForKeyCallback (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  if( PollForKey() || IsKeyInBuffer() )
  {
    //Notify the system that a key is pending
    gBS->SignalEvent(Event);
  }
}

VOID EFIAPI PollForKeyCallback (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_KEY_DATA         KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This = NULL;

  TimerRKStroke = TRUE;
  ( void )KeysDxeReadKeyStrokeEx( This,
                                  &KeyData );
  TimerRKStroke = FALSE;
}

EFI_STATUS KeysDxeResetCommon(void)
{
  //Reset the key device. This will reset and clear the key buffer
  keyBufferRead = keyBufferWrite = keyBuffer;
  numOfKeysReleased = numOfKeysPressed = 0;

  TimerEfiKeyBufferRead = TimerEfiKeyBufferWrite = TimerEfiKeyBuffer;

  SetMem(matrixA, MAX_KEYS_PRESSED_RELEASED*sizeof(UINT8), 0x00);
  SetMem(matrixB, MAX_KEYS_PRESSED_RELEASED*sizeof(UINT8), 0x00);

  SetMem(keysPressed,  MAX_KEYS_PRESSED_RELEASED * sizeof(KEY_TYPE), NONE);
  SetMem(keysReleased, MAX_KEYS_PRESSED_RELEASED * sizeof(KEY_TYPE), NONE);
  SetEfiKeyDetection(FALSE);
  TimerRKStroke  = FALSE;
  return EFI_SUCCESS;
}



EFI_STATUS EFIAPI ButtonsInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
) {
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_EVENT   WaitKeyEvt;

  DEBUG((EFI_D_INFO, "Welcome to ButtonsDxe\n"));

  // Tu código de inicialización de botones va aquí
  keyBufferRead = keyBufferWrite = keyBuffer;
  TimerEfiKeyBufferRead = TimerEfiKeyBufferWrite = TimerEfiKeyBuffer;

  Status = ButtonsInit(&numOfkeys,
                        keyMap
                      );

  NotifyList = AllocateZeroPool(sizeof(LIST_ENTRY));
  if ( NULL == NotifyList )
  {
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeListHead ( NotifyList );

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ButtonsInitialize: ButtonsInit failed = (0x%x)\n", Status));
  }

  Status = gBS->CreateEvent (
                    EVT_NOTIFY_WAIT,
                    TPL_CALLBACK,
                    WaitForKeyCallback,
                    NULL,
                    &WaitKeyEvt
                    );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "ButtonsDxeInitialize: Create Event failed,  Status = (0x%x)\r\n", Status));
    goto ErrorExit;
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
