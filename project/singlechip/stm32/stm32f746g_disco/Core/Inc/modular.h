#include "usart.h"
#include "network.h"
#include "network_data.h"

extern int usart_printf(UART_HandleTypeDef *phuart, const char* format, ...);
extern int usart_read(UART_HandleTypeDef *phuart, uint8_t *buffer, size_t len, int timer);
extern int ai_init();
extern int ai_run(const void *in_data, void *out_data);
extern float algo_sigmoid(float x);
extern void draw_rect(uint8_t *buffer, int x1, int y1, int x2, int y2, int width, int height, int c, uint16_t color, int comp);
