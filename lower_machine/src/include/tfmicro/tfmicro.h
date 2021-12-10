/*--------------------------------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/04/2021  -  13:33 PM                       |
 *--------------------------------------------------------------------------
 *| tfmicro.h              | Run the wrapper library of the CNN model      |
 *--------------------------------------------------------------------------*/

/*> Guard Area		*/
#ifndef __TFLITE_MICRO_H__
#define __TFLITE_MICRO_H__

/*> Hand Area 		*/
#include <tfmicro/tools.h>		/*< basic tensor flow lite micro hand file >*/
#include <config/tfmicro.h>		/*< config 				   >*/
#include <micro_log/micro_log.h>	/*< log model				   >*/

/*> Statement Area 	*/
struct ModelAttribute{
	
	int  OutputLayerNumber;
	char OutPutData[LAYER_NAME_NUMBER][LAYER_NAME_STR_LEN];

};

class TfLiteMicro{
	
	public:
		TfLiteMicro();
		~TfLiteMicro();
		void debug(bool Open);
		virtual bool init() = 0;

	protected:
		tflite::ErrorReporter* 				Error_Reporter;			/*< log 		>*/
		tflite::MicroErrorReporter			MicroErrorReporter;		/*< old log 		>*/
		const tflite::Model* 				Model;				/*< Model 		>*/
		tflite::MicroInterpreter* 			Interpreter;			/*< oper 		>*/
		TfLiteTensor* 					Input;				/*< input data 		>*/
		TfLiteTensor* 					Output;				/*< output 		>*/
		uint8_t						Arena[MEMORY_OPERATION_SIZE];	/*< memonry 		>*/
		tflite::MicroMutableOpResolver<TF2D_OP_2D_SIZE>	OpResolver;			/*< Op Resolver		>*/
		MicroLog					ErrLog;				/*< Log model		>*/	
		struct ModelAttribute				ModelAttr;			/*< Model Attribute	>*/
};

/*> End Guard Area	*/
#endif //__TFLITE_MICRO_H__ -- END
