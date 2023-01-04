/*-----------------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/04/2021  -  11:26              |
 *-----------------------------------------------------------------
 *| system.h               | System Properties Configuration      |
 *-----------------------------------------------------------------*/

/*> Guard Area */
#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

/*> Config Area 			*/
/*>> Describes the configuration 	*/
#define DESCRIBE_AUTHOR			"zhihao.zhou"
#define DESCRIBE_MAIL			"zhihaox.zhou@intel.com"
#define DESCRIBE_INFO			"This is ISH AI Test Device!"
/*>> Version 				*/
#define VERSION_MAIN			1		/*< It only changes when there is a major update 						>*/
#define VERSION_CHILD			0		/*< This version number will change when new features are added 				>*/
#define VERSION_SMALL			0		/*< This version number changes when bugs are fixed orminor changes are made 			>*/
#define VERSION_TIME			"11/15/2021"	/*< The time of last release 									>*/
#define VERSION_MARK			"alpha"		/*< alpha:Internal beta:Test rc:upcoming list:long-term maintenance release:stable 		>*/
#define VERSION_CODE			"MIG"		/*< Code 											>*/
#define VERSION_GREETINGS		"Death is not the end, being forgotten is"
/*>> Device config			*/
/*>>> Uart				*/
#define DEVICE_UART_NAME		"UART_1"	/*< Uart Device											>*/
#define DEVICE_UART_BAUDRATE		115200		/*< Uart baudrate										>*/
/*>> Kernel buff			*/
#define KERNEL_DATA_BUFF_SIZE		400 * 1024	/*< Buffer Data											>*/
#define KERNEL_DATA_ALIG		(4 * 10)	/*< Data Alig											>*/
#define KERNEL_MODEL_BUFF_ADDR		0		/*< Model addr											>*/

/*> End Guard Ares */
#endif //__IMAGE_CONFIG_H__ -- END
