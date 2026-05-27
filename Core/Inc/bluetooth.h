#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "main.h"

#define RX_BUFFER_SIZE 32

void Bluetooth_Init(UART_HandleTypeDef *huart);

void bluetoothCommand(char *cmd, volatile uint8_t *r, volatile uint8_t *g, volatile uint8_t *b, volatile uint8_t *update_flag);
void Bluetooth_Handle_Receive(UART_HandleTypeDef *huart, uint8_t rx_byte, char *buffer, uint8_t *index, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *update_flag);

#endif /* BLUETOOTH_H */
