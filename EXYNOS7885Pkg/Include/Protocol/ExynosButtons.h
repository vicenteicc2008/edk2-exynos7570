#ifndef _EXYNOS_BUTTONS_H_
#define _EXYNOS_BUTTONS_H_

#include <Uefi.h>

// Reemplaza con la GUID correcta para tu protocolo GPIO
extern EFI_GUID gExynosGpioProtocolGuid;

// Define una estructura para el protocolo GPIO de botones
typedef struct _EXYNOS_BUTTONS_PROTOCOL EXYNOS_BUTTONS_PROTOCOL;

// Enumeración para los posibles estados de los botones
typedef enum {
    ButtonStateUp,
    ButtonStateDown,
    // Agrega otros estados según sea necesario
} BUTTON_STATE;

// Función para inicializar el protocolo GPIO de botones
EFI_STATUS
EFIAPI
InitializeExynosButtonsProtocol(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
);

// Función para obtener el estado actual de un botón específico
EFI_STATUS
EFIAPI
GetButtonState(
    IN EXYNOS_BUTTONS_PROTOCOL *This,
    IN UINTN ButtonIndex,
    OUT BUTTON_STATE *State
);

// Otras funciones y definiciones relacionadas con los botones GPIO

#endif // _EXYNOS_BUTTONS_H_
