/*
 * rpi_communication.c
 *
 *  Created on: May 16, 2025
 *      Author: jp
 */

#include "main.h"
#include <string.h>

int write_data_to_pi(
		SPI_HandleTypeDef *hspi,
		const void* buff,	/* Pointer to the data to be written */
		unsigned int btw,			/* Number of bytes to write */
		unsigned int* bw			/* Pointer to number of bytes written */
		)
{
	*bw = 0;

	//Send message start header:
	uint8_t header_len = 4;
	uint8_t header[] = {0xFA,0x05,0xFA,0x05};
	uint8_t header_recv_buf[header_len];

	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_SET);
	if(HAL_SPI_TransmitReceive(hspi, header, header_recv_buf, header_len, HAL_MAX_DELAY) != HAL_OK){
		//unable to send header
		HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
		return 0;
	}
	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
	//header sent

	uint8_t recv_buf[btw];
	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_SET);
	if (HAL_SPI_TransmitReceive(hspi, buff, recv_buf, btw, HAL_MAX_DELAY) != HAL_OK){
		//unable to send data
		HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
		return 0;
	}
	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
	//data sent

	*bw = btw;

	//Send message end footer:
	uint8_t footer_len = 5;
	uint8_t footer[] = {0xFB,0x06,0xFB,0x06,0x00};
	uint8_t footer_recv_buf[footer_len];
	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_SET);
	if(HAL_SPI_TransmitReceive(hspi, footer, footer_recv_buf, footer_len, HAL_MAX_DELAY) != HAL_OK){
		//unable to send footer
		HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
		return 0;
	}
	HAL_GPIO_WritePin(WRITE_PIN_GPIO_Port, WRITE_PIN_Pin, GPIO_PIN_RESET);
	//footer sent

	return 1;
}


int write_file_length_to_pi(SPI_HandleTypeDef *hspi, unsigned int* number_of_bytes_written) {
	uint8_t file_end_message_len = 8;
	uint8_t file_end_message[] = {
			0xFC, 0x08, 0xFC, 0x08, (*number_of_bytes_written >> 24) & 0xFF, (*number_of_bytes_written >> 16) & 0xFF, (*number_of_bytes_written >> 8) & 0xFF, (*number_of_bytes_written) & 0xFF
	};
	unsigned int num_of_bytes_written[] = {0};
	return write_data_to_pi(hspi, file_end_message, file_end_message_len, num_of_bytes_written);
}


int create_wave_file(SPI_HandleTypeDef *hspi)
{


	struct wav_header
	{
	  char riff[4];           /* "RIFF"                                  */
	  int32_t flength;        /* file length in bytes                    */
	  char wave[4];           /* "WAVE"                                  */
	  char fmt[4];            /* "fmt "                                  */
	  int32_t chunk_size;     /* size of FMT chunk in bytes (usually 16) */
	  int16_t format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
	  int16_t num_chans;      /* 1=mono, 2=stereo                        */
	  int32_t srate;          /* Sampling rate in samples per second     */
	  int32_t bytes_per_sec;  /* bytes per second = srate*bytes_per_samp */
	  int16_t bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
	  int16_t bits_per_samp;  /* Number of bits per sample               */
	  char data[4];           /* "data"                                  */
	  int32_t dlength;        /* data length in bytes (filelength - 44)  */
	};


	struct wav_header wavh;

	const int header_length = sizeof(struct wav_header);

	strncpy(wavh.riff, "RIFF", 4);
	strncpy(wavh.wave, "WAVE", 4);
	strncpy(wavh.fmt, "fmt ", 4);
	strncpy(wavh.data, "data", 4);

	wavh.chunk_size = 16;
	wavh.format_tag = 1;
	wavh.num_chans = 1;
	wavh.srate = 48000;
	wavh.bits_per_samp = 16;
	wavh.bytes_per_sec = (wavh.srate * wavh.bits_per_samp * wavh.num_chans) / 8;
	wavh.bytes_per_samp = (wavh.bits_per_samp * wavh.num_chans) / 8;


	unsigned int res;

	return write_data_to_pi(hspi, &wavh, header_length, &res);
}


