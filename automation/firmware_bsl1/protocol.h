#include "pico/stdlib.h"

static const char PACKET_START = 254;

// Host-to-device packets
static const char PKT_H2D_SET_COLOR = 0;
static const char PKT_H2D_GET_DEFAULT_RGB = 1;
static const char PKT_H2D_GET_FW_VERSION = 2;

// Device-to-host packets
static const char PKT_D2H_ACK = 0;
static const char PKT_D2H_LED_TEMP = 1;
static const char PKT_D2H_VBUS = 2;
static const char PKT_D2H_FW_VERSION = 3;
static const char PKT_D2H_DEFAULT_RGB = 4;

#define PACKET_MAX_LENGTH 128

typedef struct {
  char incoming_packet_header;
  char incoming_packet_data[PACKET_MAX_LENGTH];
  uint8_t incoming_packet_ready;
} protocol_data_t;

extern protocol_data_t protocol_data;

void protocol_tick();
void protocol_send_packet(char header, char *data, uint8_t length);
void protocol_send_packet_int32(char header, int32_t value);
