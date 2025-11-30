#include <stdio.h>

#include "pico/stdlib.h"

#include "protocol.h"

protocol_data_t protocol_data;
uint32_t packet_index = 0;
uint32_t packet_end;

void handle_uart_input(char c)
{
  if (packet_index == 0 && c == PACKET_START)
  {
    // Packet start
    packet_index++;
  }
  else if (packet_index == 1)
  {
    // Packet type
    protocol_data.incoming_packet_header = c;
    packet_index++;
  }
  else if (packet_index == 2)
  {
    // Data Length
    if (c == 0) {
      protocol_data.incoming_packet_ready = 1;
      packet_index = 0;
    } else {
      packet_end = c + 2;
      packet_index++;
    }
  }
  else if (packet_index >= 3 && packet_index < packet_end && packet_index - 3 < PACKET_MAX_LENGTH - 1)
  {
    // Data start
    protocol_data.incoming_packet_data[packet_index - 3] = c;
    packet_index++;
  }
  else if (packet_index == packet_end)
  {
    // End
    protocol_data.incoming_packet_data[packet_index - 3] = c;
    protocol_data.incoming_packet_data[packet_index - 2] = 0;
    protocol_data.incoming_packet_ready = 1;
    packet_index = 0;
  } else {
    // Reset
    packet_index = 0;
  }
}

void protocol_tick()
{
  while (true)
  {
    int32_t c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT)
      handle_uart_input(c);
    else
      break;
  }
}

void protocol_send_packet(char header, char *data, uint8_t length)
{
  putchar_raw(PACKET_START);
  putchar_raw(header);
  putchar_raw(length);
  for (uint8_t i = 0; i < length; i++)
  {
    putchar_raw(data[i]);
  }
}

void protocol_send_packet_int32(char header, int32_t value)
{
  putchar_raw(PACKET_START);
  putchar_raw(header);
  putchar_raw(4);
  putchar_raw((value >> 24) & 0xff);
  putchar_raw((value >> 16) & 0xff);
  putchar_raw((value >> 8) & 0xff);
  putchar_raw(value & 0xff);
}
