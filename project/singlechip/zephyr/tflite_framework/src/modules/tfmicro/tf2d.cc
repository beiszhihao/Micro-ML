/*----------------------------------------------------
 *| zhihao.zhou@intel.com | 11/04/2021  -  14:09 PM |
 *----------------------------------------------------
 *| tf2d.cc                 | For image data         |
 *----------------------------------------------------*/

/*> Hand Area */
#include <tfmicro/tf2d.h>		/*< extern >*/
#include <string.h>

/*> Implementation Area 		*/
/*>> Constructor 			*/
Tf2D::Tf2D(){
	
	Error_Reporter  = NULL;
	Model 		= NULL;
	Interpreter	= NULL;
	Input		= NULL;
	Output		= NULL;
	memset(&ModelAttr,0,sizeof(struct ModelAttribute));

}

/*>> Deconstructor 			*/
Tf2D::~Tf2D(){

}
/*>> Initialize a 2D vector graph	*/
bool Tf2D::init(){
	
   //Init error log
   Error_Reporter = &MicroErrorReporter;
   //init resolver for op
   OpResolver.AddAveragePool2D();
   OpResolver.AddConv2D();
   OpResolver.AddDepthwiseConv2D();
   OpResolver.AddReshape();
   OpResolver.AddSoftmax();
   IsModel = false;
   IsAttr  = false; 

   ErrLog.output(EXCEPTION_SUCCESS,"Tf2d initialized successfully");
   return true;

}
/*>> Set Model */
bool Tf2D::Create_Destructor(unsigned char* model){
	
	if(model == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"The model pointer is null");
		return false;
	}

	//init model
	Model = tflite::GetModel(model);
	if(Model->version() != TFLITE_SCHEMA_VERSION){
		
		ErrLog.output(EXCEPTION_ERROR,"The version of this model is not supported");
		return false;
	}
	//TODO
	/*< DYNAMIC
	 * This method uses memory allocation to construct the calculation view
	 * which effectively improves the scalability, but not every time 
	 *
	 *< Static
	 * This is the most stable method. By modifying the tfmicro code, 
	 * two functions are extended, and the scalability can be achieved without using pointers
	 * Static does not support switching models
	 *@ zhihao.zhou 
	 */
#ifndef TF2D_CORE_INTER_STATIC
	
	ErrLog.output(EXCEPTION_NORMAL,"Create inter dynamically");
	if(Interpreter != NULL){
		
		delete Interpreter;
		Interpreter = NULL;
	}

	Interpreter = new tflite::MicroInterpreter(Model,OpResolver,Arena,MEMORY_OPERATION_SIZE,Error_Reporter);

#else

	ErrLog.output(EXCEPTION_NORMAL,"Create inter static");
	static tflite::MicroInterpreter Still_Interpreter(Model,OpResolver,Arena,MEMORY_OPERATION_SIZE,Error_Reporter);

	//check
	if(Still_Interpreter.IsOperatorGraphEstablished()){
		
		Still_Interpreter.ReleaseOperatorGraphEstablished();
		Still_Interpreter.ReCreateOperatorGraphEstablished(Model);
	}

	Interpreter = &Still_Interpreter;

#endif
	if(Interpreter == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"Unable to allocate memory for compute pointer");
		return false;

	}

	if(Interpreter->AllocateTensors() != kTfLiteOk){
		
		ErrLog.output(EXCEPTION_ERROR,"Unable to create calculated view");
		return false;
	
	}
	
	Input = Interpreter->input(TF2D_INPUT_2D_INDEX);
	
	if(Input == NULL){
	
		ErrLog.output(EXCEPTION_ERROR,"This model has no input layer");
		return false;
	}
	
	IsModel = true;
	ErrLog.output(EXCEPTION_SUCCESS,"The model and calculation view are established successfully");
	return true;

}
/*>> Write data*/
bool Tf2D::Write2dData(unsigned char* buff2D,int len){
	
	if(buff2D == NULL){
		
		return false;
	}

	if(len <= 0 || len >= MEMORY_OPERATION_SIZE){
	
		return false;
	}
	
	if(Input == NULL){
	
		return false;
	}

	memset(Input->data.int8,0,len);
	memcpy(Input->data.int8,buff2D,len);

	return true;

}
/*>> operation , Returns the most appropriate training set index and score*/
Tf2DResult Tf2D::Operation(int OutPutLayer){
	
	Tf2DResult result = {0};
	
	if(IsModel == false){
		
		return result;

	}

	if(IsAttr == false){
	
		return result;
	}

	if(OutPutLayer <= 0){
		
		return result;
	}
	
	if(Interpreter == NULL){
	
		return result;
	}
	
	if(kTfLiteOk != Interpreter->Invoke()){
		
		return result;
	}
		
	Output = Interpreter->output(TF2D_OUTPUT_2D_INDEX);

	result.score 	 = Output->data.uint8[0];
	result.index     = 0;
	
	for(int i = 0;i < OutPutLayer;++i){
		
		if(Output->data.uint8[i] > result.score){
			
			result.score 	  = Output->data.uint8[i];
			result.index 	  = i;
		}
	}

	return result;

}
/*< set model attr		*/
bool Tf2D::SetModelAttr(unsigned char* data){

	if(data == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"Data is null pointer");
		return false;
	}
	
	SerializeAttr(data);	
	IsAttr = true;

	ErrLog.output(EXCEPTION_SUCCESS,"Set Model Attribute Success");	
	return true;
}
/*< Serialize Attr		*/
void Tf2D::SerializeAttr(unsigned char* data){

	char 	Temp_Str[LAYER_NAME_STR_LEN] 	= {0};
	int  	Temp_Index 		  	=  0 ;
	int  	Real_Index			=  0 ;
	int	Loop_Number			= (LAYER_NAME_NUMBER*LAYER_NAME_STR_LEN);
	
	if(data == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"Data is null pointer");
		return;
	}

	for(int i = 0;i < Loop_Number; ++i){
		
		//Eliminate warning
		if(Temp_Index >= LAYER_NAME_STR_LEN){
		        memset(Temp_Str,0,LAYER_NAME_STR_LEN);
			Temp_Index = 0;
		}	
		
		if(Real_Index >= LAYER_NAME_NUMBER){
			
			Real_Index = 0;
		}
		
		Temp_Str[Temp_Index] = data[i];
		if(Temp_Str[Temp_Index] == EOF || Temp_Str[Temp_Index] == '\0' 
		   || Temp_Str[Temp_Index] == 0){
			
			break;

		}
			
		if(Temp_Str[Temp_Index] == TF2D_DATA_CHECK_FORMAT){
		
			strncpy(ModelAttr.OutPutData[Real_Index++],Temp_Str,Temp_Index);
			Temp_Index = 0;	
			memset(Temp_Str,0,LAYER_NAME_STR_LEN);
			continue;

		}
		
		Temp_Index++;
	}
	
	ModelAttr.OutputLayerNumber = atoi((const char*)ModelAttr.OutPutData[TF2D_OUTPUT_LYER_NUMBER_ADDR]);
	return;

}
/*< Forecast			*/
Tf2DFinal Tf2D::Forecast(unsigned char* data,size_t len){
	
	Tf2DResult result = {0};
	Tf2DFinal  fin    = {0};

	if(IsModel == false){

		ErrLog.output(EXCEPTION_ERROR,"No model created");
		return fin;

	}

	if(IsAttr == false){
		
		ErrLog.output(EXCEPTION_ERROR,"Model properties are not set");
		return fin;

	}

	if(data == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"Data is null pointer");
		return fin;
	}

	if(len <= 0){
	
		ErrLog.output(EXCEPTION_ERROR,"Data Len <= 0");
		return fin;
	}

	Write2dData(data,len);
	result = Operation(ModelAttr.OutputLayerNumber);
	strcpy(fin.Name,ModelAttr.OutPutData[TF2D_OUTPUT_LYER_NUMBER_INDEX + result.index]);

	ErrLog.output(EXCEPTION_SUCCESS,"Forecast success");
	return fin;

}
/*< Get model attr		>*/
struct ModelAttribute* Tf2D::GetModelAttr(){

	return &ModelAttr;

}
