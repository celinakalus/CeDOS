/*! \file
 * Interface for accessing the programmable interrupt contoller (PIC).
 */
#ifndef OS_PIC_H
#define OS_PIC_H

#define PIC1_COMMAND			0x20
#define PIC1_DATA				0x21
#define PIC2_COMMAND			0xA0
#define PIC2_DATA				0xA1

#define PIC_END_OF_INTERRUPT	0x20

#define PIC1_OFFSET				0x20
#define PIC2_OFFSET				0x28

/*!
 * Moves irqs to appropriate addresses and enables all PIC interrupts
 * \return 1 on success, 0 on fail
 */
int pic_init(void);

/*!
 * Unmasks interrupt number \p irq.
 * \param irq Number of the interrupt to unmask.
 * \return 1 on success, 0 on fail
 */
int pic_unmask_interrupt(int irq);

/*!
 * Masks interrupt number \p irq.
 * \param irq Number of the interrupt to mask.
 * \return 1 on success, 0 on fail
 */
int pic_mask_interrupt(int irq);

#endif