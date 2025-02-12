#ifndef _ORDER_HANDLER_H_
#define _ORDER_HANDLER_H_

/**
  @brief Setup OrderHandler.
 */
void OrderHandler_Init(void);

/**
  @brief OrderHandler is notified message has been received
 */
void OrderHandler_MessageReceived(void);

/**
  @brief OrderHandler is notified message has been transmitted
 */
void OrderHandler_MessageTransmitted(void);

/**
  @brief OrderHandler needs to decode a message.
 */
void OrderHandler_HandleRx(void);

#endif // _ORDER_HANDLER_H_