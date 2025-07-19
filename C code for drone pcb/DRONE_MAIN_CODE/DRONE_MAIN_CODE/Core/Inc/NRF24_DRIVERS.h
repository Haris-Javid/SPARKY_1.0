/*
 * NRF24_DRIVERS.h
 *
 *  Created on: Jul 1, 2025
 *      Author: haris
 */

#ifndef INC_NRF24_DRIVERS_H_
#define INC_NRF24_DRIVERS_H_

typedef enum {
  NO_DATA,
  RX_RECEIVED
} nRF24_RX_Status;

// Initializes the nRF24L01+ module with the given configuration parameters.
void nRF24_set_up(uint8_t width, uint8_t channel, uint8_t data_rate, uint8_t power_level, uint8_t CRC_mode, uint8_t delay, uint8_t attempts, uint8_t rx_pipe, uint8_t *addr, uint8_t AA_state, uint8_t payload_size);


// Sets the nRF24L01+ module to receive (RX) mode.
void nRF24_Switch_to_RX_mode();

// Checks if data has been received in the RX FIFO.
// If data is available, it updates the provided buffer and length, and returns RX_RECEIVED or NO_DATA.
nRF24_RX_Status RECEIVED_DATA(uint8_t *buffer, uint8_t length);

// Sends a data packet using the nRF24L01+ module.
void nRF24_start_sending(uint8_t *data, uint8_t length);

// Sets the nRF24L01+ module to transmit (TX) mode.
void nRF24_Switch_to_TX_mode();

//register dump
void get_nrf24_reg_values(uint8_t *output_buffer);

#endif /* INC_NRF24_DRIVERS_H_ */
