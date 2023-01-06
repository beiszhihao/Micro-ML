/*-------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/09/2021  -  13:00 PM |
 *-------------------------------------------------------
 *| uart.h                    | Config uart             |
 *----------------------------------------------------*/

/* Guard Area		*/
#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

/*>Config AGR		*/
/*>> V1			*/
#define AGR_V1
#ifdef  AGR_V1
#define AGR_SIZE		    sizeof(AGR_HAND_V1)	    /*< agr size 				>*/
#define AGR_NAME		    AGR_HAND_V1		    /*< Required for extension code 	>*/
#define AGR_SIGN_NAME		sign			/*< Agr sign				>*/
#define AGR_DATA_LEN_NAME	data_size		/*< Agr Data Size Name			>*/
#define AGR_HELLO		    0			/*< sey hello 				>*/
#define AGR_SET_MODEL		1			/*< set model				>*/
#define AGR_SET_MODEL_ATTR	2			/*< set model attr			>*/
#define AGR_FORECAST		3			/*< Forecast data			>*/
#define AGR_RESET_MEMORY    4           /*< Reset Memory addr       >*/
struct AGR_HAND_V1{
	
	int sign;					/*< static 				>*/
	int data_size;					/*< data size 				>*/
};
#endif

/*>Config recv		*/
#define RECV_TIME_OUT	500				/*< recv time out	>*/
/*<UART core        */
#define UART_CORE_RECV_INTEL_OSE        /*< Intem ose       >*/

/* End Guard Area	*/
#endif //__UART_CONFIG_H__ -- END
