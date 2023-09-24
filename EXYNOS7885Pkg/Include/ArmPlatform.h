/** @file
*
*  Copyright (c) 2013-2017, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef __ARM_JUNO_H__
#define __ARM_JUNO_H__

//#include <VExpressMotherBoard.h>

/***********************************************************************************
// Platform Memory Map
************************************************************************************/

// Motherboard Peripheral and On-chip peripheral

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_ARM_OEM_ID           'S','A','M','A','1','0'   // OEMID 6 bytes long
#define EFI_ACPI_ARM_OEM_TABLE_ID     SIGNATURE_64('S','A','M','-','A','1','0','5') // OEM table id 8 bytes long
#define EFI_ACPI_ARM_OEM_REVISION     0x20140727
#define EFI_ACPI_ARM_CREATOR_ID       SIGNATURE_32('S','A','M',' ')
#define EFI_ACPI_ARM_CREATOR_REVISION 0x00000099

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define ARM_ACPI_HEADER(Signature, Type, Revision) {              \
    Signature,                      /* UINT32  Signature */       \
    sizeof (Type),                  /* UINT32  Length */          \
    Revision,                       /* UINT8   Revision */        \
    0,                              /* UINT8   Checksum */        \
    { EFI_ACPI_ARM_OEM_ID },        /* UINT8   OemId[6] */        \
    EFI_ACPI_ARM_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_ARM_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_ARM_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_ARM_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }

//
// Hardware platform identifiers
//
#define JUNO_REVISION_PROTOTYPE 0
#define JUNO_REVISION_R0        1
#define JUNO_REVISION_R1        2
#define JUNO_REVISION_R2        3
#define JUNO_REVISION_UKNOWN    0xFF

// Define if the exported ACPI Tables are based on ACPI 5.0 spec or latest
//#define ARM_JUNO_ACPI_5_0

//
// Address of the system registers that contain the MAC address
// assigned to the PCI Gigabyte Ethernet device.
//

/***********************************************************************************
// Motherboard memory-mapped peripherals
************************************************************************************/

// Define MotherBoard SYS flags offsets (from ARM_VE_BOARD_PERIPH_BASE)
//

#define DISTANCE_BTWN_PORTS	(0x20)

#define GPIO_CON		(0x00)
#define GPIO_DATAIN		(0x04)
#define GPIO_PUD		(0x08)
#define GPIO_DRV		(0x0C)

#define GPIO_DATAIN_MASK(x)     (1UL << (x))
#define GPIO_PUD_MASK(x)        (3UL << (x*2))
#define GPIO_DRV_MASK(x)        (3UL << (x*2))
#define GPIO_SFN_MASK(x)	(15UL <<(x*4))

#define GPIO_SFN_EN(x)		(2UL << (x*4))
#define GPIO_OP_EN(x)		(1UL << (x*4))

#define GPIO_PUD_DIS(x)		(0UL << (x*2))
#define GPIO_PDN_EN(x)		(1UL << (x*2))
#define GPIO_PUP_EN(x)		(3UL << (x*2))
#define GPIO_DATA_HIGH(x)	(1UL << (x))
#define GPIO_DATA_LOW(x)	(0UL << (x))

#define GPIO_DRV_SET(strength,pin)	(strength << (pin*2))

#define PIN0	(0x00)
#define PIN1	(0x01)
#define PIN2	(0x02)
#define PIN3	(0x03)
#define PIN4	(0x04)
#define PIN5	(0x05)
#define PIN6	(0x06)
#define PIN7	(0x07)

// 0x1140_0000
#define GPA0	(0x00)
#define GPA1	(0x01)
#define GPA2	(0x02)
#define GPB0	(0x03)
#define GPB1	(0x04)
#define GPB2	(0x05)
#define GPB3	(0x06)
#define GPC0	(0x07)
#define GPC1	(0x08)
#define GPC2	(0x09)
#define GPC3	(0x0A)
#define GPD0	(0x0B)
#define GPD1	(0xc0)
#define GPY0	(0x0D)
#define GPY1	(0x0E)
#define GPY2	(0x0F)
#define GPY3	(0x10)
#define GPY4	(0x11)
#define GPY5	(0x12)
#define GPY6	(0x13)
#define GPX0	(0x60)
#define GPX1	(0x61)
#define GPX2	(0x62)
#define GPX3	(0x63)

// 0x1340_0000
#define GPE0	(0x70)
#define GPE1	(0x71)
#define GPF0	(0x72)
#define GPF1	(0x73)
#define GPG0	(0x74)
#define GPG1	(0x75)
#define GPG2	(0x76)
#define GPH0	(0x77)
#define GPH1	(0x78)

// 0x10D1_0000
#define GPV0	(0x80)
#define GPV1	(0x81)
// ETC5PUD
#define GPV2	(0x83)
#define GPV3	(0x84)
// ETC8PUD
#define GPV4	(0x86)

// 0x0386_0000
#define GPZ	(0x90)

// Sites where the peripheral is fitted
//
#endif
