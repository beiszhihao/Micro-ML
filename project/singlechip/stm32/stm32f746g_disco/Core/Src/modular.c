#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "modular.h"
#include "network.h"
#include "network_data.h"

static ai_handle network = AI_HANDLE_NULL;

AI_ALIGNED(32)
static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

int usart_printf(UART_HandleTypeDef *phuart, const char* format, ...) {
	va_list aptr;
	char buffer[1024] = { 0 };

	if(phuart == NULL) {

		return -1;
	}

	va_start(aptr, format);
	vsprintf(buffer, format, aptr);
	va_end(aptr);
	
	return HAL_UART_Transmit(phuart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

}

int usart_read(UART_HandleTypeDef *phuart, uint8_t *buffer, size_t len, int timer) {

	if (phuart == NULL) {

		return -1;
	}	

	return HAL_UART_Receive(phuart, buffer, len, timer);
}

int ai_init() {
	ai_error err;

	err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
	if (err.type != AI_ERROR_NONE) {
		
		return err.type;
	}

	const ai_network_params params = AI_NETWORK_PARAMS_INIT(
			AI_NETWORK_DATA_WEIGHTS(ai_network_data_weights_get()),
	    		AI_NETWORK_DATA_ACTIVATIONS(activations)
			);
	if (!ai_network_init(network, &params)) {
	
		err = ai_network_get_error(network);
		return err.type;
	}

	return 0;

}

int ai_run(const void *in_data, void *out_data)
{
	ai_i32 n_batch;
	__attribute__((unused)) ai_error err;
	ai_buffer ai_input[AI_NETWORK_IN_NUM] = AI_NETWORK_IN;
	ai_buffer ai_output[AI_NETWORK_OUT_NUM] = AI_NETWORK_OUT;
	
	ai_input[0].n_batches = 1;
	ai_input[0].data = AI_HANDLE_PTR(in_data);
	ai_output[0].n_batches = 1;
	ai_output[0].data = AI_HANDLE_PTR(out_data);
	n_batch = ai_network_run(network, &ai_input[0], &ai_output[0]);

	if (n_batch != 1) {
		err = ai_network_get_error(network);
		return err.type;
	};

	return 0;
}

float algo_sigmoid(float x) {
	float y; 
	
	y= 1/(1+expf(-x));
	
	return y;
}

void draw_rect(uint8_t *buffer, int x1, int y1, int x2, int y2, int width, int height, int c, uint16_t color, int comp) {
	
	for (int i = (y1 * (width * c) + (x1 * c)); i < (y1 * (width * c) + (x1 * c)) + (x2 * c) - ((x2 * c)  / comp); ++i) {
		
		buffer[i - 1] = color;

	}
	
	for (int i = (x2 * (width * c) + (x1 * c)); i < (x2 * (width * c) + (x1 * c)) + (x2 * c) - ((x2 * c) / comp); ++i) {
		
		buffer[i - 1] = color;

	}
	

	int up = 1;
	for (int i = ((y1 + up) * (width * c) + (x1 * c)); i < (x2 * (width * c) + (x1 * c)); i = ((y1 + up) * (width * c) + (x1 * c))) {
		int rt = 0;
		int of = ((x2 * c) / comp);
		
		if(of % 2 == 0)
			rt = 0;
		else
			rt = 1;

		buffer[i - 1] = color;

		buffer[(i - 1) + (x2 * c) - of - rt] = color;
		up++;
	}
}
