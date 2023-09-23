#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
EFIAPI
ExynosGpioEntryPoint (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    DEBUG((EFI_D_INFO, "Hello World\n"));

    // Devolver EFI_SUCCESS para indicar que el controlador se ha inicializado correctamente.
    return EFI_SUCCESS;
}
