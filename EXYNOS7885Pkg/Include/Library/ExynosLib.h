/** @file

  Copyright (c) 2012, Samsung Electronics Co. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EXYNOSLIB_H__
#define __EXYNOSLIB_H__

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/**
  gExynosPkgTokenSpaceGuid GUID definition.
*/
#define EXYNOS7885PKG_TOKEN_SPACE_GUID \
         { 0x99a14446, 0xaad7, 0xe460, {0xb4, 0xe5, 0x1f, 0x79, 0xaa, 0xa4, 0x93, 0xfd } }

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the gExynosPkgTokenSpaceGuid GUID.
*/
extern EFI_GUID gEXYNOS7885PkgTokenSpaceGuid;


UINT32
EFIAPI
GpioBase (
  IN  UINTN Port
  );

#endif // __EXYNOSLIB_H__