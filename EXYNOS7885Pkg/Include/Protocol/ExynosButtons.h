#ifndef _EXYNOS_BUTTONS_H_
#define _EXYNOS_BUTTONS_H_

#include <Uefi.h>
#include <Library/UefiLib.h>

#define EXYNOS_BUTTONS_PROTOCOL_GUID \
  { 0x1ebc06b9, 0x36e6, 0x452d, { 0x94, 0xb7, 0x1e, 0xc7, 0xf1, 0xe0, 0x55, 0x97 } }

// key types
typedef enum {
    NONE = 0,
	  PWR,
    VOL_UP,
    VOL_DOWN,
    FOCUS,
    CAMERA,
    ENTER,
    HOME,
    ROTATE_LOCK,
    KEY_LAST
}KEY_TYPE;

typedef enum {
  BUTTON_NOT_PRESSED = 0,
  BUTTON_PRESSED
} BUTTON_STATE;

typedef struct _EXYNOS_BUTTONS_PROTOCOL EXYNOS_BUTTONS_PROTOCOL;

typedef EFI_STATUS (EFIAPI *READ_ENTER_KEY_STATE)(
  IN EXYNOS_BUTTONS_PROTOCOL *This,
  OUT BUTTON_STATE *State
);

typedef EFI_STATUS (EFIAPI *INITIALIZE_BUTTONS_PROTOCOL)(
  IN EXYNOS_BUTTONS_PROTOCOL *This
);

struct _EXYNOS_BUTTONS_PROTOCOL {
  READ_ENTER_KEY_STATE ReadEnterKeyState;      // Función para leer el estado del botón de encendido como "Enter"
  INITIALIZE_BUTTONS_PROTOCOL InitializeButtons;
};

extern EFI_GUID gExynosButtonsProtocolGuid;

EFI_STATUS EFIAPI InitializeButtonsProtocol(IN EXYNOS_BUTTONS_PROTOCOL *This);

#endif // _EXYNOS_BUTTONS_H_
