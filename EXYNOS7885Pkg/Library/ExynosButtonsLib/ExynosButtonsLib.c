/** @file
ButtonsLib.c

Button driver is responsible for detecting key press/release through PMIC HW
and service. It implements the interfaces of simple text input protocol.

This file is platform dependent, it supports ButtonsDxe based on the platform.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All rights reserved.
Qualcomm Technologies Proprietary and Confidential.

**/
#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ButtonsLib.h>
#include <Library/PcdLib.h>

#include "ButtonsLibPrivate.h"

BOOLEAN isEfiKeyDetected = FALSE;
BOOLEAN isHomeKeyDetected = FALSE;
extern EFI_GUID gSamsungTokenSpaceGuid;

#define NUMBER_OF_KEYS 4

/*** Define the Key Map for all Platforms ***/

//KeyMap for CDP, MTP and FLUID
KEY_TYPE KeyMap_8x09[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE
};


/**
Find a local key to an array.

@param  pArray         An array pointer to which the key will be found from.
@param  sizeOfArray    The array size.
@param  key            The key will be found.

@retval TRUE           Found successfully.
@retval FALSE          Found failed.

**/

BOOLEAN FoundAKey(
   KEY_TYPE *pArray,
   UINT8     sizeOfArray,
   KEY_TYPE  key
   )
{
   UINT8 i;
   for( i=0; i<sizeOfArray; i++ )
   {
      if( pArray[i] == key )
      {
         return TRUE;
      }
   }
   return FALSE;
}


/**
Translate local keys into EFI keys.

The press session starts with any key is pressed and ends with no key is
pressed.

Only generate one EFI key per press session and this EFI key is generated
only based on the above table.

For the case of multiple keys pressed, check the possible combo keys first,
first detected combo keys will be used to generate the EFI key. If there is
no valid combo keys detected, then check the single key case, first detected
single key will be used to generate the EFI key.

Once an EFI key is generated in a session, even though there is still other
key currently pressed, no more EFI key will be generated in this session and
just wait for user to release those pressed keys.

@param  pKeysPressed                Pointer to currently pressed keys array.
@param  numOfKeysPressed            Number of keys pressed in pKeysPressed
@param  pKeysReleased               Pointer to currently released keys array.
@param  numOfKeysReleased           Number of keys released in pKeysReleased
@param  sizeOfPressedReleasedArray  Size of pKeysPresed/pKeysPressed arrays.
@param  pEfiKeys                    Pointer to Uefi keys array.
@param  pNumOfEfiKeys               Number of Uefi keys detected.

@retval EFI_SUCCESS                 UEFI key detected
@retval EFI_NOT_READY               No UEFI key detected

**/
EFI_STATUS ConvertEfiKeyCode (
   KEY_TYPE       *pKeysPressed,
   UINT8           numOfKeysPressed,
   KEY_TYPE       *pKeysReleased,
   UINT8           numOfKeysReleased,
   UINT8           sizeOfPressedReleasedArray,
   EFI_INPUT_KEY  *pEfiKeys,
   UINT8          *pNumOfEfiKeys
   )
{
   EFI_STATUS retVal = EFI_NOT_FOUND;
   EFI_INPUT_KEY EfiKey;

   EfiKey.ScanCode = SCAN_NULL;
   EfiKey.UnicodeChar = 0;
   *pNumOfEfiKeys = 0;

   BOOLEAN bCameraKeyIsPressed;
   BOOLEAN bVolUpKeyIsPressed;
   BOOLEAN bVolDownKeyIsPressed;
   BOOLEAN bCameraKeyIsReleased;
   BOOLEAN bHomeKeyIsPressed;
   BOOLEAN bRotateLockKeyIsPressed;
   BOOLEAN bRotateLockKeyIsReleased;
   BOOLEAN bPwrKeyPressed;

   if( isEfiKeyDetected == FALSE )
   {
      bCameraKeyIsPressed      = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, CAMERA);
      bVolUpKeyIsPressed       = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, VOL_UP);
      bVolDownKeyIsPressed     = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, VOL_DOWN);
      bCameraKeyIsReleased     = FoundAKey(pKeysReleased,sizeOfPressedReleasedArray, CAMERA);
      bHomeKeyIsPressed        = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, HOME);
      bRotateLockKeyIsPressed  = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, ROTATE_LOCK);
      bRotateLockKeyIsReleased = FoundAKey(pKeysReleased, sizeOfPressedReleasedArray, ROTATE_LOCK);
      bPwrKeyPressed           = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, PWR);

      // assume EFI key found
      isEfiKeyDetected = TRUE;

      if( bCameraKeyIsPressed && bVolUpKeyIsPressed )
      {
         // combo key found
         EfiKey.ScanCode = SCAN_HOME;
      }
      else if( bCameraKeyIsPressed && bVolDownKeyIsPressed )
      {
         // combo key found
         EfiKey.ScanCode = SCAN_DELETE;
      }
      else if( bRotateLockKeyIsPressed && bVolUpKeyIsPressed)
      {
     	 EfiKey.ScanCode = SCAN_ESC;
      }
      else if( bRotateLockKeyIsPressed && bVolDownKeyIsPressed)
      {
     	 EfiKey.ScanCode = SCAN_DELETE;
      }
      else if( bVolUpKeyIsPressed )
      {
         if( bPwrKeyPressed )
         {
           // combo key found
           EfiKey.ScanCode = SCAN_HOME;
        }
        else
        {
           // single key found
           EfiKey.ScanCode = SCAN_UP;
        }
      }
      else if( bVolDownKeyIsPressed )
      {
         if( bPwrKeyPressed )
         {
           // combo key found
           EfiKey.ScanCode = SCAN_DELETE;
         }
         else
         {
           // single key found
           EfiKey.ScanCode = SCAN_DOWN;
         }
      }
      else if( bCameraKeyIsReleased || bRotateLockKeyIsReleased )
      {
         // single key found
         EfiKey.UnicodeChar = CHAR_CARRIAGE_RETURN;
      }
      else if( bHomeKeyIsPressed )
      {
        EfiKey.ScanCode = SCAN_HOME;
      }
      else if( bPwrKeyPressed )
      {
        EfiKey.ScanCode = CHAR_CARRIAGE_RETURN;
      }
      else
      {
         // no EFI key found, set the flag to FALSE.
         isEfiKeyDetected = FALSE;
      }


      if( isEfiKeyDetected == TRUE )
      {
         // an EFI key is detected, return it.
         CopyMem( &pEfiKeys[*pNumOfEfiKeys], &EfiKey, sizeof(EFI_INPUT_KEY));
         (*pNumOfEfiKeys)++;

         retVal = EFI_SUCCESS;
      }
   }

   if( numOfKeysPressed == 0)
   {
      // no more key pressed, reset flag for next press session.
      isEfiKeyDetected = FALSE;
   }

   return retVal;
}



/**
Initialize KeyMap based on the platform.
Also initialze Power Key.

@param  pNumberOfKeys            Pointer to number of keys
@param  pKeyMap                  Pointer to key map.
@retval EFI_SUCCESS              Initialization successful
@retval non EFI_SUCCESS          Initialization failed

**/

EFI_STATUS InitializeKeyMap (
   UINT8         *pNumberofKeys,
   KEY_TYPE      *pKeyMap
   )
{

   CopyMem( pKeyMap, KeyMap_8x09, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
   *pNumberofKeys = NUMBER_OF_KEYS;
}

/**
Initialize all button GPIOs on PMIC for input based
on platform

@param  None
@retval EFI_SUCCESS              Configuration successful
@retval non EFI_SUCCESS          Configuration failed

**/
EFI_STATUS ConfigureButtonGPIOs ( VOID )
{

   EFI_STATUS Status;

   // volume up
   Status = TLMMProtocol->ConfigGpio(
             (UINT32)EFI_GPIO_CFG(VOLUME_UP_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA),
             TLMM_GPIO_ENABLE);

   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "ConfigureButtonGPIOs:failed to configure VOL+ button, Status = (0x%x)\r\n", Status));
      return Status;
   }
   else 
   {
      DEBUG(( EFI_D_INFO | EFI_D_LOAD, "ConfigureButtonGPIOs:configured VOL+ button\r\n"));
   }

   // volume down
   Status = TLMMProtocol->ConfigGpio(
             (UINT32)EFI_GPIO_CFG(VOLUME_DOWN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA),
             TLMM_GPIO_ENABLE);

   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "ConfigureButtonGPIOs:failed to configure VOL- button, Status = (0x%x)\r\n", Status));
      return Status;
   }
   else 
   {
      DEBUG(( EFI_D_INFO | EFI_D_LOAD, "ConfigureButtonGPIOs:configured VOL- button\r\n"));
   }

}

/**
Initialize all PMIC GPIOs as input based on platform.
Also configure the power key

@param  pNumberOfKeys            Pointer to number of keys
@param  pKeyMap                  Pointer to key map.
@retval EFI_SUCCESS              Initialization successful
@retval non EFI_SUCCESS          Initialization failed

**/
EFI_STATUS ButtonsInit (
   UINT8         *pNumberofKeys,
   KEY_TYPE      *pKeyMap
   )
{
   EFI_STATUS Status;

   // Intialize KeyMap
   Status = InitializeKeyMap( pNumberofKeys, pKeyMap );
   if ( EFI_ERROR(Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: InitializeKeyMap() failed, Status =  (0x%x)\r\n", Status ));
      return Status;
   }
   else 
   {
      DEBUG(( EFI_D_INFO | EFI_D_LOAD, "ButtonsInit: InitializeKeyMap()\r\n"));
   }

   // Configure button GPIOs
   Status = ConfigureButtonGPIOs();
   if ( EFI_ERROR(Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: ConfigureButtonGPIOs() failed, Status =  (0x%x)\r\n", Status ));
      return Status;
   }
   else 
   {
      DEBUG(( EFI_D_INFO | EFI_D_LOAD, "ButtonsInit: ConfigureButtonGPIOs()\r\n"));
   }

   // Configure PON Debounce
   Status = ConfigurePONDebounce();
   if ( EFI_ERROR(Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: ConfigurePONDebounce() failed, Status =  (0x%x)\r\n", Status ));
      return Status;
   }
   else 
   {
      DEBUG(( EFI_D_INFO | EFI_D_LOAD, "ButtonsInit: ConfigurePONDebounce()\r\n"));
   }
}

/**
Read gpio status on PMIC.

@param  GPIO                     MSM GPIO.
@param  pGpioButtonPressed       Pointer if GPIO activity (press) happened.

@retval EFI_SUCCESS              Retrieve status successfully
@retval non EFI_SUCCESS          Retrieve status failed

**/
EFI_STATUS ReadGpioStatus(UINT32 Gpio, BOOLEAN *pGpioButtonPressed )
{
   EFI_STATUS Status = EFI_SUCCESS;
   UINT32 value = GPIO_HIGH_VALUE;
   UINT32 config = 0;

   config = EFI_GPIO_CFG( Gpio, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA );

   //GPIOs initially Pulled-Up
   if(GPIO_LOW_VALUE == value)
   {
     *pGpioButtonPressed = TRUE;
   }
   else
   {
     *pGpioButtonPressed = FALSE;
   }
   return Status;
}


/**
Poll button array.

@param  pButtonArray             Pointer to buttons array.

@retval EFI_SUCCESS              Retrieve matrix successfully
@retval non EFI_SUCCESS          Retrieve matrix failed

**/
EFI_STATUS PollButtonArray( UINT8 *pButtonArray )
{
   EFI_STATUS Status;
   BOOLEAN ButtonPressed = FALSE;

   //for Power Key
   Status = PollPowerKey(&ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
     DEBUG(( EFI_D_ERROR, "PollButtonArray: PollPowerKey failed Status = (0x%x)\r\n", Status));
     return Status;
   }
   *(pButtonArray + 0) = ButtonPressed;

   //for volume up
   Status = ReadGpioStatus( VOLUME_UP_GPIO, &ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
     DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadGpioStatus failed for VOL+ button, Status = (0x%x)\r\n", Status));
     return Status;
   }
   *(pButtonArray + 1) = ButtonPressed;

   //for volume down
   Status = ReadGpioStatus( VOLUME_DOWN_GPIO, &ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
     DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadGpioStatus failed for VOL- button, Status = (0x%x)\r\n", Status));
     return Status;
   }

   *(pButtonArray + 2) = ButtonPressed;
   
}

/**
  Set the value of isEfiKeyDetected .

  @param  flag  Boolean value .

  @retval none

**/
VOID SetEfiKeyDetection(BOOLEAN flag)
{
   isEfiKeyDetected = flag;
}

/**
  return the value of isHomeKeyDetected .

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsHomeKeyDetected(VOID)
{
    return isHomeKeyDetected;
}


/**
  return TRUE if Key press notification is supported.

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsNotificationSupported(VOID)
{
  return TRUE;
}
