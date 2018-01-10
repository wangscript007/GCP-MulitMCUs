/*
 * @brief Common ring buffer support functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __RING_BUFF_H
#define __RING_BUFF_H

#ifdef __CC_ARM
#define INLINE  __inline
#else
#define INLINE inline
#endif


/** @defgroup Ring_Buffer CHIP: Simple ring buffer implementation
 * @ingroup CHIP_Common
 * @{
 */

/**
 * @brief Ring buffer structure
 */
typedef struct {
    void *data;
    int count;
    int itemSz;
    unsigned long int head;
    unsigned long int tail;
} RINGBUFF_T;

/**
 * @def        RB_VHEAD(rb)
 * volatile typecasted head index
 */
#define RB_VHEAD(rb)              (*(volatile unsigned long int *) &(rb)->head)

/**
 * @def        RB_VTAIL(rb)
 * volatile typecasted tail index
 */
#define RB_VTAIL(rb)              (*(volatile unsigned long int *) &(rb)->tail)

/**
 * @brief    Initialize ring buffer
 * @param    RingBuff    : Pointer to ring buffer to initialize
 * @param    buffer        : Pointer to buffer to associate with RingBuff
 * @param    itemSize    : Size of each buffer item size
 * @param    count        : Size of ring buffer
 * @note    Memory pointed by @a buffer must have correct alignment of
 *             @a itemSize, and @a count must be a power of 2 and must at
 *             least be 2 or greater.
 * @return    Nothing
 */
int RingBuff_Init(RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count);

/**
 * @brief    Resets the ring buffer to empty
 * @param    RingBuff    : Pointer to ring buffer
 * @return    Nothing
 */
static INLINE void RingBuff_Flush(RINGBUFF_T *RingBuff)
{
    RingBuff->head = RingBuff->tail = 0;
}

/**
 * @brief    Return size the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @return    Size of the ring buffer in bytes
 */
static INLINE int RingBuff_GetSize(RINGBUFF_T *RingBuff)
{
    return RingBuff->count;
}

/**
 * @brief    Return number of items in the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @return    Number of items in the ring buffer
 */
static INLINE int RingBuff_GetCount(RINGBUFF_T *RingBuff)
{
    return RB_VHEAD(RingBuff) - RB_VTAIL(RingBuff);
}

/**
 * @brief    Return number of free items in the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @return    Number of free items in the ring buffer
 */
static INLINE int RingBuff_GetFree(RINGBUFF_T *RingBuff)
{
    return RingBuff->count - RingBuff_GetCount(RingBuff);
}

/**
 * @brief    Return number of items in the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @return    1 if the ring buffer is full, otherwise 0
 */
static INLINE int RingBuff_IsFull(RINGBUFF_T *RingBuff)
{
    return (RingBuff_GetCount(RingBuff) >= RingBuff->count);
}

/**
 * @brief    Return empty status of ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @return    1 if the ring buffer is empty, otherwise 0
 */
static INLINE int RingBuff_IsEmpty(RINGBUFF_T *RingBuff)
{
    return RB_VHEAD(RingBuff) == RB_VTAIL(RingBuff);
}

/**
 * @brief    Insert a single item into ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @param    data        : pointer to item
 * @return    1 when successfully inserted,
 *            0 on error (Buffer not initialized using
 *            RingBuff_Init() or attempted to insert
 *            when buffer is full)
 */
int RingBuff_Insert(RINGBUFF_T *RingBuff, const void *data);

/**
 * @brief    Insert an array of items into ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @param    data        : Pointer to first element of the item array
 * @param    num            : Number of items in the array
 * @return    number of items successfully inserted,
 *            0 on error (Buffer not initialized using
 *            RingBuff_Init() or attempted to insert
 *            when buffer is full)
 */
int RingBuff_InsertMult(RINGBUFF_T *RingBuff, const void *data, int num);

/**
 * @brief    Pop an item from the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @param    data        : Pointer to memory where popped item be stored
 * @return    1 when item popped successfuly onto @a data,
 *             0 When error (Buffer not initialized using
 *             RingBuff_Init() or attempted to pop item when
 *             the buffer is empty)
 */
int RingBuff_Pop(RINGBUFF_T *RingBuff, void *data);

/**
 * @brief    Pop an array of items from the ring buffer
 * @param    RingBuff    : Pointer to ring buffer
 * @param    data        : Pointer to memory where popped items be stored
 * @param    num            : Max number of items array @a data can hold
 * @return    Number of items popped onto @a data,
 *             0 on error (Buffer not initialized using RingBuff_Init()
 *             or attempted to pop when the buffer is empty)
 */
int RingBuff_PopMult(RINGBUFF_T *RingBuff, void *data, int num);


/**
 * @}
 */

#endif /* __RING_BUFFER_H_ */