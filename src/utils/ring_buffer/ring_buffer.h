#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stdbool.h>

#include "ring_buffer_config.h"

typedef struct RingBuffer{
  uint16_t u16Head;
  uint16_t u16Count;
  uint16_t u16Size;
  uint8_t au8Data[RING_BUFFER_LEN_MAX];
} RingBuffer_t;

/**
 * Function to initialize a buffer structure with the given size.
 * Returns true if we were able to initialize the buffer and false otherwise.
 */
bool RingBuffer_Init(RingBuffer_t *arg_psBuff, uint16_t arg_u16Size);

/**
 * Add the specified data pointer to the end of the buffer. If the buffer is
 * full, the oldest queued item will be replaced by default.
 * Returns true if the data was stored and false otherwise.
 */
bool RingBuffer_Add(RingBuffer_t *arg_psBuff, uint8_t arg_u8Data);

/**
 * Return a pointer to the head of the buffer without removing that item.
 */
uint8_t RingBuffer_Peek(RingBuffer_t *arg_psBuff);

/**
 * Remove an item from the head of the buffer and return its pointer.
 */
uint8_t RingBuffer_Remove(RingBuffer_t *arg_psBuff);

/**
 * Determine if the given buffer is empty.
 * Returns true if the buffer is empty and false otherwise.
 */
bool RingBuffer_IsEmpty(RingBuffer_t *arg_psBuff);

/**
 * Determine if the given buffer is full.
 * Returns true if the buffer is full and false otherwise.
 */
bool RingBuffer_IsFull(RingBuffer_t *arg_psBuff);

#endif // __RING_BUFFER_H__