/*--------------------------------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/04/2021  -  13:31 PM                       |
 *--------------------------------------------------------------------------
 *| tfmicro.cc             | Run the wrapper library of the CNN model      |
 *--------------------------------------------------------------------------*/

/*> Hand Area 			*/
#include <tfmicro/tfmicro.h>		/*< Statement file >*/

/*> Implement Area 		*/
/*>> constructor 		*/
TfLiteMicro::TfLiteMicro(){

}

/*>> destructor 		*/
TfLiteMicro::~TfLiteMicro(){

}

/*> Debug			*/
void TfLiteMicro::debug(bool Open){

	ErrLog.debug(Open);
}
