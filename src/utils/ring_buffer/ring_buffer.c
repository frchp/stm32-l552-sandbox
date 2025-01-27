#include <stdlib.h>

#include "ring_buffer.h"

#include "bsp.h"

/**
 * Function to initialize a buffer structure with the given size.
 * Returns true if we were able to initialize the buffer and false otherwise.
 */
bool RingBuffer_Init(RingBuffer_t *arg_psBuff, uint16_t arg_u16Size)
{
  bool loc_bRet = true;

  if (arg_u16Size > RING_BUFFER_LEN_MAX || arg_psBuff == NULL) 
  {
    loc_bRet = false;
  }
  else
  {
    // Atomically initialize the structure
    uint32_t loc_u32Status = Bsp_EnterCritical();
    arg_psBuff->u16Size = arg_u16Size;
    arg_psBuff->u16Head = 0u;
    arg_psBuff->u16Count = 0u;
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_bRet;
}

/**
 * Add the specified data pointer to the end of the buffer. If the buffer is
 * full, the oldest queued item will be replaced by default.
 * Returns true if the data was stored and false otherwise.
 */
bool RingBuffer_Add(RingBuffer_t *arg_psBuff, uint8_t arg_u8Data)
{
  bool loc_bRet = true;

  if (arg_psBuff == NULL) {
    loc_bRet = false;
  }
  else
  {
    uint32_t loc_u32Status = Bsp_EnterCritical();
    if (arg_psBuff->u16Count < arg_psBuff->u16Size) {
      uint16_t loc_u16Index = (arg_psBuff->u16Head + arg_psBuff->u16Count) % arg_psBuff->u16Size;

      // Insert this item at the end of the queue
      arg_psBuff->au8Data[loc_u16Index] = arg_u8Data;
      arg_psBuff->u16Count++;
    } else {
      // Overwrite what's at the head of the queue since we're out of space
      arg_psBuff->au8Data[arg_psBuff->u16Head] = arg_u8Data;
      arg_psBuff->u16Head = (arg_psBuff->u16Head + 1) % arg_psBuff->u16Size;
    }
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_bRet;
}

/**
 * Return a pointer to the head of the buffer without removing that item.
 */
uint8_t RingBuffer_Peek(RingBuffer_t *arg_psBuff)
{
  uint8_t loc_u8Ret = 0u;

  if (arg_psBuff == NULL) {
    // Nothing to do
  }
  else
  {
    uint32_t loc_u32Status = Bsp_EnterCritical();
    if (arg_psBuff->u16Count > 0) {
      loc_u8Ret = arg_psBuff->au8Data[arg_psBuff->u16Head];
    }
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_u8Ret;
}

/**
 * Remove an item from the head of the buffer and return its pointer.
 */
uint8_t RingBuffer_Remove(RingBuffer_t *arg_psBuff)
{
  uint8_t loc_u8Ret = 0u;

  if (arg_psBuff == NULL) {
    // Nothing to do
  }
  else
  {
    uint32_t loc_u32Status = Bsp_EnterCritical();
    if (arg_psBuff->u16Count > 0) {
      loc_u8Ret = arg_psBuff->au8Data[arg_psBuff->u16Head];
      arg_psBuff->u16Head = (arg_psBuff->u16Head + 1) % arg_psBuff->u16Size;
      arg_psBuff->u16Count--;
    }
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_u8Ret;
}

/**
 * Determine if the given buffer is empty.
 * Returns true if the buffer is empty and false otherwise.
 */
bool RingBuffer_IsEmpty(RingBuffer_t *arg_psBuff)
{
  bool loc_bRet = true;

  if (arg_psBuff == NULL) {
    // Nothing to do
  }
  else
  {
    uint32_t loc_u32Status = Bsp_EnterCritical();
    loc_bRet = (arg_psBuff->u16Count == 0);
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_bRet;
}

/**
 * Determine if the given buffer is full.
 * Returns true if the buffer is full and false otherwise.
 */
bool RingBuffer_IsFull(RingBuffer_t *arg_psBuff)
{
  bool loc_bRet = true;

  if (arg_psBuff == NULL) {
    // Nothing to do
  }
  else
  {
    uint32_t loc_u32Status = Bsp_EnterCritical();
    loc_bRet = (arg_psBuff->u16Count == arg_psBuff->u16Size);
    Bsp_ExitCritical(loc_u32Status);
  }

  return loc_bRet;
}