/*
 * rpi_communication.h
 *
 *  Created on: May 16, 2025
 *      Author: jp
 */

#ifndef INC_RPI_COMMUNICATION_H_
#define INC_RPI_COMMUNICATION_H_

int write_data_to_pi(
		SPI_HandleTypeDef *hspi,
		const void* buff,	/* Pointer to the data to be written */
		unsigned int btw,			/* Number of bytes to write */
		unsigned int* bw			/* Pointer to number of bytes written */
		);

int write_file_length_to_pi(SPI_HandleTypeDef *hspi, unsigned int* number_of_bytes_written);
int initiate_file_write_to_pi(SPI_HandleTypeDef *hspi);
int create_wave_file(SPI_HandleTypeDef *hspi);
#endif /* INC_RPI_COMMUNICATION_H_ */
