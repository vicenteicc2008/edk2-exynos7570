#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleTextInEx.h>

typedef enum {
  NoButtons = 0,
  VolUpButton = 1,
  VolDownButton = 2
} BUTTON_STATE;

typedef struct {
  GPIO_BUTTON_SERVICES_PROTOCOL ButtonServicesProtocol;
  BUTTON_STATE ButtonState;
} GPIO_BUTTON_SERVICES_PROTOCOL;

GPIO_BUTTON_SERVICES_PROTOCOL *gBsp = NULL;

#define GPIO_BSP_FROM_BSP(a) \
  BASE_CR(a, GPIO_BUTTON_SERVICES_PROTOCOL, ButtonServicesProtocol)

EFI_STATUS
EFIAPI
PreBootVolumeUpButtonThenPowerButtonCheck(
    IN GPIO_BUTTON_SERVICES_PROTOCOL *This,
    OUT BOOLEAN *PreBootVolumeUpButtonThenPowerButton
) {
  GPIO_BUTTON_SERVICES_PROTOCOL *Bsp;

  DEBUG((DEBUG_VERBOSE, "%a \n", __FUNCTION__));

  Bsp = GPIO_BSP_FROM_BSP(This);
  *PreBootVolumeUpButtonThenPowerButton = (Bsp->ButtonState == VolUpButton);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PreBootVolumeDownButtonThenPowerButtonCheck(
    IN GPIO_BUTTON_SERVICES_PROTOCOL *This,
    OUT BOOLEAN *PreBootVolumeDownButtonThenPowerButton
) {
  *PreBootVolumeDownButtonThenPowerButton = FALSE;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PreBootClearVolumeButtonState(IN GPIO_BUTTON_SERVICES_PROTOCOL *This) {
  GPIO_BUTTON_SERVICES_PROTOCOL *Bsp;

  DEBUG((DEBUG_VERBOSE, "%a \n", __FUNCTION__));
  Bsp = GPIO_BSP_FROM_BSP(This);
  Bsp->ButtonState = NoButtons;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
KeyNotify(IN EFI_KEY_DATA *KeyData) {
  if (gBsp == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (KeyData->Key.ScanCode == SCAN_UP) {
    gBsp->ButtonState = VolUpButton;
    DEBUG((DEBUG_INFO, "%a: Vol+ Button Detected\n", __FUNCTION__));
  } else if (KeyData->Key.ScanCode == SCAN_DOWN) {
    gBsp->ButtonState = VolDownButton;
    DEBUG((DEBUG_INFO, "%a: Vol- Button Detected\n", __FUNCTION__));
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetButtonState() {
  EFI_STATUS Status;
  EFI_HANDLE Handle = NULL;
  EFI_DEVICE_PATH_PROTOCOL *ButtonsDxeDevicePath;
  EFI_KEY_DATA KeyData;
  VOID *NotifyHandle;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleEx;

  Status = EFI_SUCCESS;

  DEBUG((DEBUG_VERBOSE, "%a: Entry\n", __FUNCTION__));

  ButtonsDxeDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&KeypadDevicePath;

  Status = gBS->LocateDevicePath(
      &gEfiSimpleTextInputExProtocolGuid, &ButtonsDxeDevicePath, &Handle);
  if (EFI_ERROR(Status)) {
    DEBUG(
        (DEBUG_ERROR,
         "Failed to locate ButtonsDxe device path for button service protocol, "
         "Status = %r.\n",
         Status));
    goto Exit;
  }

  Status = gBS->OpenProtocol(
      Handle, &gEfiSimpleTextInputExProtocolGuid, (VOID **)&SimpleEx,
      gImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR(Status)) {
    DEBUG(
        (DEBUG_ERROR,
         "Failed to open ButtonsDxe protocol for button service protocol, "
         "Status = %r.\n",
         Status));
    goto Exit;
  }

  // Prevent Key persistence when chainloaded
  SimpleEx->Reset(SimpleEx, TRUE);

  KeyData.KeyState.KeyToggleState = 0;
  KeyData.KeyState.KeyShiftState = 0;

  KeyData.Key.UnicodeChar = 0;
  KeyData.Key.ScanCode = SCAN_UP;

  Status = SimpleEx->RegisterKeyNotify(
      SimpleEx, &KeyData, &KeyNotify, &NotifyHandle);
  if (EFI_ERROR(Status)) {
    DEBUG(
        (DEBUG_ERROR,
         "Failed to register volume up notification for button service "
         "protocol, "
         "Status = %r.\n",
         Status));
    goto Exit;
  }

  KeyData.Key.ScanCode = SCAN_DOWN;

  Status = SimpleEx->RegisterKeyNotify(
      SimpleEx, &KeyData, &KeyNotify, &NotifyHandle);
  if (EFI_ERROR(Status)) {
    DEBUG(
        (DEBUG_ERROR,
         "Failed to register volume down notification for button service "
         "protocol, "
         "Status = %r.\n",
         Status));
    goto Exit;
  }

Exit:
  return Status;
}

EFI_STATUS
EFIAPI
ButtonsInit(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((DEBUG_INFO, "%a \n", __FUNCTION__));

  gBsp = AllocateZeroPool(sizeof(GPIO_BUTTON_SERVICES_PROTOCOL));
  if (gBsp == NULL) {
    DEBUG(
        (DEBUG_ERROR,
         "Failed to allocate memory for button service protocol.\n"));
    return EFI_SUCCESS;
  }

  gBsp->ButtonServicesProtocol.PreBootVolumeDownButtonThenPowerButtonCheck =
      PreBootVolumeDownButtonThenPowerButtonCheck;
  gBsp->ButtonServicesProtocol.PreBootVolumeUpButtonThenPowerButtonCheck =
      PreBootVolumeUpButtonThenPowerButtonCheck;
  gBsp->ButtonServicesProtocol.PreBootClearVolumeButtonState =
      PreBootClearVolumeButtonState;
  gBsp->ButtonState = NoButtons;

  Status = GetButtonState();
  if (EFI_ERROR(Status)) {
    FreePool(gBsp);
    return Status;
  }

  // Install the protocol
  Status = gBS->InstallMultipleProtocolInterfaces(
      &ImageHandle, &gButtonServicesProtocolGuid, gBsp, NULL);

  if (EFI_ERROR(Status)) {
    DEBUG(
        (DEBUG_ERROR,
         "Button Services Protocol Publisher: install protocol error, Status = "
         "%r.\n",
         Status));
    FreePool(gBsp);
    return Status;
  }

  DEBUG((DEBUG_INFO, "Button Services Protocol Installed!\n"));
  return EFI_SUCCESS;
}
