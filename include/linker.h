/*! \file
 * Wrapper for symbols defined in the linker.
 */
#ifndef LINKER_H
#define LINKER_H

#include <stdint.h>

extern uint8_t __KERNEL_VMA;
extern uint8_t __KERNEL_LMA;
extern uint8_t __KERNEL_SIZE;

#define KERNEL_VMA (&__KERNEL_VMA)
#define KERNEL_LMA (&__KERNEL_LMA)
#define KERNEL_SIZE (uint32_t)(&__KERNEL_SIZE)

extern uint8_t __REALMD_VMA;
extern uint8_t __REALMD_LMA;
extern uint8_t __REALMD_SIZE;

#define REALMD_VMA (&__REALMD_VMA)
#define REALMD_LMA (&__REALMD_LMA)
#define REALMD_SIZE (uint32_t)(&__REALMD_SIZE)

#endif