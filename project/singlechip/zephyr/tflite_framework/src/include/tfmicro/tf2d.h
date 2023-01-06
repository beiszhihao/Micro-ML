/*-------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/04/2021  -  14:08 PM |
 *-------------------------------------------------------
 *| tf2d.h                     | For image data         |
 *-------------------------------------------------------*/

/*> Guard Area		*/
#ifndef __TF2D_H__
#define __TF2D_H__

/*> Hand Area 		*/
#include <tfmicro/tfmicro.h>	/*< Parent class >*/
#include <string.h>
#include <stdlib.h>

/*> struct Area 	*/
struct Tf2DResult{
	int index;
	int score;
};
struct Tf2DFinal{
	char Name[256];
};
/*> Statement Area 	*/
class Tf2D : public TfLiteMicro{
	
	public:
		Tf2D() noexcept;
		~Tf2D();
		virtual bool 		init();
		bool 	     		Create_Destructor(unsigned char* model);
		bool			    SetModelAttr(unsigned char* data);
		bool                Write2dData(unsigned char* buff2D,int len);
		Tf2DResult		    Operation(int OutPutLayer);
		Tf2DFinal	 	    Forecast(unsigned char* data,size_t len);
		struct ModelAttribute* 	GetModelAttr();
	private:
		void SerializeAttr(unsigned char* data);
		bool IsModel;
		bool IsAttr;
};

/*> End Guard Area */
#endif //__TF2D_H__ -- END
