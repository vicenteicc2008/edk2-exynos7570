#ifndef EXYNOS_GPIO_PROTOCOL_H
#define EXYNOS_GPIO_PROTOCOL_H

#define EXYNOS_GPIO_PROTOCOL_GUID \
  { 0xDEADBEEF, 0x1234, 0x5678, { 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78 } }

typedef struct _EXYNOS_GPIO_PROTOCOL EXYNOS_GPIO_PROTOCOL;

EFI_FORWARD_DECLARATION (EXYNOS_GPIO_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EXYNOS_GPIO_GET_INPUT_VALUE) (
  IN  EXYNOS_GPIO_PROTOCOL    *This,
  IN  UINT32                  GpioPin,
  OUT BOOLEAN                 *GpioValue
);

struct _EXYNOS_GPIO_PROTOCOL {
  EXYNOS_GPIO_GET_INPUT_VALUE    GetInputValue;
  // Agregar otras funciones de GPIO según sea necesario.
};

extern EFI_GUID gExynosGpioProtocolGuid;

#endif // EXYNOS_GPIO_PROTOCOL_H
