/*! \file
 * Driver for keyboards.
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/*!
 * Interface for a keyboard device.
 */
typedef struct {
    /*!
     * Name of the keyboard.
     */
    const char* name;

    /*!
     * Initializes the keyboard.
     * \return 1 on success, 0 on fail
     */
    int (*init)(void);
    
    /*!
     * Reads a single character from the keyboard
     * \return A single char corresponding to a key press.
     */
    int (*read)(void);
} KB_DRIVER;

//! PS/2 keyboard driver (default driver)
extern KB_DRIVER ps2_kb;

extern KB_DRIVER *std_kb;

#endif