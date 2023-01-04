/*-------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/05/2021  -  15:01 PM |
 *-------------------------------------------------------
 *| micro_log.h                 | Config log            |
 *-------------------------------------------------------*/

/*> Guard Area 							*/
#ifndef __MICRO_LOG_CONFIG_H__
#define __MICRO_LOG_CONFIG_H__

/*>Config Area							*/
/*>> BUFF							*/
#define BUFF_SIZE			1 * 1024		/*< How many characters can you output at most         >*/
/*>> Exception vector table					*/
#define EXCEPTION_SUCCESS		0			/*< Operation succeeded 		               >*/
#define EXCEPTION_ERROR			1			/*< Operation error			               >*/
#define EXCEPTION_FAIL			2			/*< Operation fail			       	       >*/
#define EXCEPTION_NORMAL		3			/*< Normal log output			       	       >*/
#define EXCEPTION_UNKNOWN		4			/*< UNKNOWN				       	       >*/
#define EXCEPTION_MAX			EXCEPTION_UNKNOWN	/*< What is the maximum number of exceptions supported >*/

/*> End Guard Area 						*/
#endif	//__MICRO_LOG_CONFIG_H__
