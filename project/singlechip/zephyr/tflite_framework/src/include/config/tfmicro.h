/*--------------------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/04/2021  -  13:12                 |
 *--------------------------------------------------------------------
 *| tfmicro.h                 | tensor flow lite micro config        |
 *--------------------------------------------------------------------*/

/*> Guard Area */
#ifndef __TFMICRO_CONFIG_H__
#define __TFMICRO_CONFIG_H__

/*> Config Area 	*/
/*>> Memory Config 	*/
#define MEMORY_OPERATION_SIZE			136 * 1024		/*< Maximum available space for operation 	>*/
/*>> Layer Config	*/
#define LAYER_NAME_NUMBER			256			/*< layer number				>*/
#define LAYER_NAME_STR_LEN			256			/*< name str len 				>*/
/*>> TF2D Config	*/
#define TF2D_INPUT_2D_INDEX			0			/*< Input index 				>*/
#define TF2D_OUTPUT_2D_INDEX			0			/*< Output index 				>*/
#define TF2D_OP_2D_SIZE				5			/*< Applicable only to 2D OP operators 		>*/
#define TF2D_CORE_INTER_STATIC						/*< using static create inter			>*/
#define TF2D_OUTPUT_LYER_NUMBER_INDEX		1			/*< Output lyer number				>*/
#define TF2D_OUTPUT_LYER_NUMBER_ADDR		0			/*< lyer number addr				>*/
#define TF2D_DATA_STR_FORMAT			2			/*< Delete \r					>*/
#define TF2D_DATA_CHECK_FORMAT			'\n'			/*< As \n					>*/
#define TF2D_OUTPUTDATA_ADDR			1			/*< data addr					>*/
/*> End Guard Area */
#endif //__TFMICRO_CONFIG_H__ -- END
