#include "comm.h"

#define COMM_QUEUE_SIZE 100

static COMM_FNC_PTR(COMM_TX_FNC);
static COMM_FNC_PTR(COMM_RX_FNC);

static uint32_t tx_head, tx_tail;
static uint32_t rx_head, rx_tail;

static struct comm_msg MsgQueueTX[COMM_QUEUE_SIZE];
static struct comm_msg MsgQueueRX[COMM_QUEUE_SIZE];

void comm_init(COMM_FNC_PTR(TX), COMM_FNC_PTR(RX))
{
  COMM_TX_FNC = TX;
  COMM_RX_FNC = RX;
}

void comm_update()
{
  uint32_t new_tail;
  static uint32_t rx_count = 0;
  static struct comm_msg rxb;

  if(tx_head != tx_tail) // There are messages waiting in the TX queue
    {
    // Send message
    COMM_TX_FNC((uint8_t *)&MsgQueueTX[tx_head], sizeof(MsgQueueTX[tx_head]));

    // Update head pointer
    if(tx_head == COMM_QUEUE_SIZE - 1)
      tx_head = 0;
    else
        tx_head++;
    }

  rx_count += COMM_RX_FNC((uint8_t *)&rxb+rx_count, sizeof(struct comm_msg) - rx_count);
  if(rx_count == sizeof(struct comm_msg)) // There are data waiting in the UART0 RX buffer
    {
    // Calculate new tail
    if(rx_tail == COMM_QUEUE_SIZE - 1)
      new_tail = 0;
    else
      new_tail = rx_tail+1;

    if(new_tail != rx_head) // There is room for the new message
      {
      // Reset RX counter
      rx_count = 0;

      MsgQueueRX[rx_tail] = rxb;

      // Update tail pointer
      rx_tail = new_tail;
      }
    }
}

bool comm_put(struct comm_msg *msg)
{
  unsigned int new_tail;

  // Calculate new tail
  if(tx_tail == COMM_QUEUE_SIZE - 1)
    new_tail = 0;
  else
    new_tail = tx_tail+1;

  if(new_tail == tx_head) // Queue Full!
    return FALSE;

  // Copy new message to queue
  memcpy(&MsgQueueTX[tx_tail], msg, sizeof(struct comm_msg));

  // Update tail pointer
  tx_tail = new_tail;

  return TRUE;
}

bool comm_get(struct comm_msg *msg)
{
  if(rx_tail == rx_head) // Queue Empty!
    return FALSE;

  // Read message from queue
  memcpy(msg, &MsgQueueRX[rx_head], sizeof(struct comm_msg));

  // Calculate new head
  if(rx_head == COMM_QUEUE_SIZE - 1)
    rx_head = 0;
  else
    rx_head++;

  return TRUE;
}

bool comm_ready()
{
  return rx_tail != rx_head;
}
