#ifndef COMM_H
#define COMM_H

#include <string.h>
#include <dev/cpu.h>
#include <dev/can.h>

// Macros to create a function and its pointer to TX and RX functions
#define COMM_FNC(fnc)      unsigned int   fnc (uint8_t *data, uint32_t size)
#define COMM_FNC_PTR(fnc)  unsigned int (*fnc)(uint8_t *    , uint32_t     )

#define COMM_FNC_CAN 0
#define COMM_FNC_LED 1
#define COMM_FNC_DS  2
#define COMM_FNC_AIN 3

struct comm_msg {
  uint32_t fnc;
  union {
    uint32_t  ds;
    uint32_t led;
    struct {
      uint32_t vin, term, vbat;
    } ad;
    CAN_MSG can;
  } data;
};

void comm_init(COMM_FNC_PTR(TX), COMM_FNC_PTR(RX));
void comm_update();
bool comm_put(struct comm_msg *msg);
bool comm_get(struct comm_msg *msg);
bool comm_ready();
#endif
