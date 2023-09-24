#ifndef _GPIO_PROTOCOL_H_
#define _GPIO_PROTOCOL_H_

#include <Uefi/UefiBaseType.h>

// Forward declaration of the protocol structure.
typedef struct _EFI_GPIO_PROTOCOL EFI_GPIO_PROTOCOL;

// Define GPIO pin configurations.
typedef enum {
  EfiGpioIoInput,
  EfiGpioIoOutput,
  // Add more configurations as needed.
} EFI_GPIO_IO_MODE;

typedef enum {
  EfiGpioDriveStrength2mA,
  EfiGpioDriveStrength4mA,
  EfiGpioDriveStrength6mA,
  // Add more drive strength levels as needed.
} EFI_GPIO_DRIVE_STRENGTH;

typedef enum {
  EfiGpioPullDefault,
  EfiGpioPullUp,
  EfiGpioPullDown,
  // Add more pull-up/pull-down configurations as needed.
} EFI_GPIO_PULL_CONFIG;

// Function prototypes for GPIO protocol functions.

/**
  Set the I/O mode of a GPIO pin.

  @param[in]  This         A pointer to the EFI_GPIO_PROTOCOL instance.
  @param[in]  GpioPin      The GPIO pin number to configure.
  @param[in]  IoMode       The I/O mode to set for the GPIO pin.
  @param[in]  DriveStrength The drive strength configuration for the GPIO pin.
  @param[in]  PullConfig   The pull-up/pull-down configuration for the GPIO pin.

  @retval EFI_SUCCESS      The GPIO pin was configured successfully.
  @retval EFI_UNSUPPORTED  The GPIO pin configuration is not supported.
  @retval Others           An error occurred while configuring the GPIO pin.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_GPIO_SET_IO_MODE)(
  IN EFI_GPIO_PROTOCOL *This,
  IN UINT32 GpioPin,
  IN EFI_GPIO_IO_MODE IoMode,
  IN EFI_GPIO_DRIVE_STRENGTH DriveStrength,
  IN EFI_GPIO_PULL_CONFIG PullConfig
);

/**
  Enable or disable GPIO pin wakeup functionality (if supported).

  @param[in]  This         A pointer to the EFI_GPIO_PROTOCOL instance.
  @param[in]  GpioPin      The GPIO pin number to enable or disable for wakeup.
  @param[in]  Enable       TRUE to enable wakeup, FALSE to disable it.

  @retval EFI_SUCCESS      The GPIO pin wakeup was configured successfully.
  @retval EFI_UNSUPPORTED  GPIO pin wakeup is not supported.
  @retval Others           An error occurred while configuring GPIO pin wakeup.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_GPIO_SET_WAKEUP_ENABLE)(
  IN EFI_GPIO_PROTOCOL *This,
  IN UINT32 GpioPin,
  IN BOOLEAN Enable
);

/**
  Other GPIO-related functions can be added here as needed.

  For example, functions to read/write GPIO pin values, etc.

**/

/**
  The EFI_GPIO_PROTOCOL structure.

  This structure defines the GPIO protocol interface.
**/
struct _EFI_GPIO_PROTOCOL {
  EFI_GPIO_SET_IO_MODE SetIoMode;
  EFI_GPIO_SET_WAKEUP_ENABLE SetWakeupEnable;
  // Add other GPIO-related function pointers here.
};

extern EFI_GUID gExynosButtonsProtocolGuid;

#endif // _GPIO_PROTOCOL_H_
