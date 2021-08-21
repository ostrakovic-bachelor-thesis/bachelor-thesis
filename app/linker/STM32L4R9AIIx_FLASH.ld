/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32L4R9AIIx Device with
**                2048KByte FLASH, 640KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Ac6.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Ac6 permit registered System Workbench for MCU users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the System Workbench for MCU toolchain.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x200A0000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x2000;      /* required amount of heap  */
_Min_Stack_Size = 0x800; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
FLASH          (rx) : ORIGIN = 0x08000000, LENGTH = 128K

/* For bitmap of size 390*390*3 it's needed 456300 bytes of space (111.4 pages of 4kB) */
/* But because of alignment, this parts of FLASH memory are rounded up to 112 pages (448kB) */
FLASH_BGBMP1    (rx) : ORIGIN = 0x08020000, LENGTH = 448K

FLASH_BGBMP2    (rx) : ORIGIN = 0x08090000, LENGTH = 448K

FLASH_BGBMP3    (rx) : ORIGIN = 0x08100000, LENGTH = 448K

/* Part of FLASH memory dedicated for storing smaller bitmaps */
FLASH_BMPS      (rx) : ORIGIN = 0x08170000, LENGTH = 568K

/* Part of FLASH memory dediated for storing metadata about bitmaps in 'FLASH_BMPS' section */
FLASH_BMPS_META (rx) : ORIGIN = 0x081FE000, LENGTH = 4K

/* Part of FLASH to store USER INFO */
FLASH_INFO      (rx) : ORIGIN = 0x081FF000, LENGTH = 4K

RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 640K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH

  .ARM.extab   :
  {
  . = ALIGN(8);
  *(.ARM.extab* .gnu.linkonce.armextab.*)
  . = ALIGN(8);
  } >FLASH
  .ARM : {
	. = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
	. = ALIGN(8);
  } >FLASH

  .preinit_array     :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
	. = ALIGN(8);
  } >FLASH

  .init_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
	. = ALIGN(8);
  } >FLASH
  .fini_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
	. = ALIGN(8);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH


  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM


  /*===================================================*/
  /*=============== USER DEFINED SECTION ==============*/

  .backgroundBitmap1 :
  {
    . = ALIGN(12);
    _backgroundBitmap1_start = .;
    backgroundBitmap1 = .;

    *(.backgroundBitmap1)

    _backgroundBitmap1_end = .;
  } > FLASH_BGBMP1


  .backgroundBitmap2 :
  {
    . = ALIGN(12);
    _backgroundBitmap2_start = .;
    backgroundBitmap2 = .;

    *(.backgroundBitmap2)

    _backgroundBitmap2_end = .;
  } > FLASH_BGBMP2


  .backgroundBitmap3 :
  {
    . = ALIGN(12);
    _backgroundBitmap3_start = .;
    backgroundBitmap3 = .;

    *(.backgroundBitmap3)

    _backgroundBitmap3_end = .;
  } > FLASH_BGBMP3


  .bitmaps :
  {
    . = ALIGN(12);
    _bitmaps_start = .;
    bitmaps = .;

    *(.bitmaps)

    . = ALIGN(12);
    _bitmaps_end = .;
  } > FLASH_BMPS


  .bitmapsMetadata :
  {
    . = ALIGN(8);
    _bitmapsMetadata_start = .;
    bitmapsMetadata = .;

    *(.bitmapsMetadata)

    . = ALIGN(8);
    _bitmapsMetadata_end = .;
  } > FLASH_BMPS_META


  .user_info_flash :
  {
    . = ALIGN(8);
    _user_info_flash_start = .;

    *(.user_info_flash)

    . = ALIGN(8);
    _user_info_flash_end = .;
  } > FLASH_INFO

  /*===================================================*/
  /*===================================================*/


  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}


