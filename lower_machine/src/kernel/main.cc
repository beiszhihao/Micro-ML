/*------------------------------------------------
 *| zhihao.zhou@intel.com | 11/01/2021  -  8:23  |
 *------------------------------------------------
 *| main.cc                | Core control        |
 *------------------------------------------------*/

/*> Hand Area */
#include <tfmicro/tools.h>	/*< Contains all tfmicro tool libraries 		>*/
#include <tfmicro/tf2d.h>	/*< 2D AI for image processing				>*/
#include <uart/uart.h>		/*< Serial port module					>*/
#include <micro_log/micro_log.h>/*< Log tool						>*/
#include <config/image.h>	/*< Contains the configuration required by the image 	>*/
#include <config/system.h>	/*< Config system-related properties 			>*/
#include <auxiliary/basic.h>	/*< Common functions					>*/
#include <zephyr.h>
#include <sys/printk.h>
#include <sys/reboot.h>
#include <logging/log.h>
#include <device.h>
#include <soc.h>
#include <stdlib.h>

/*> call func 	*/
extern void agr_hello_func();
extern bool agr_set_model_func(unsigned char* ModelBuff);
extern bool agr_set_model_attr_func(unsigned char* AttrData);
extern bool agr_forecast_func(unsigned char* Data,size_t Len);
extern bool agr_reset_memory_func();

namespace {

	Uart 					UartDevice;					/*< Uart Module >*/
	MicroLog				ErrLog;						/*< Log Module	>*/
	Tf2D					TfMl;						/*< TinyMl	>*/	
	unsigned char				Data_Buffer[KERNEL_DATA_BUFF_SIZE];		/*< Buffer	>*/
	unsigned int 				Model_Basic;					/*< Model Baisc >*/
	/*< Communication protocol using v1 >*/
	AGR_NAME*				Agr;						/*< V1 agr >*/

}

bool setup(){
	
	UartDevice.debug(true);
	if(UartDevice.init(DEVICE_UART_NAME,DEVICE_UART_BAUDRATE) == false){

		ErrLog.output(EXCEPTION_ERROR,"Failed to initialize UART device");	
		return false;

	}
	
	TfMl.debug(true);	
	if(TfMl.init() == false){
		
		ErrLog.output(EXCEPTION_ERROR,"Failed to initialize TF2D Model");
		return false;
	}

	return true;

}

bool loop(){
       	
	//memset(BASIC_ADDR_OFFSET(Data_Buffer,Model_Basic),0,KERNEL_DATA_BUFF_SIZE - Model_Basic);
	//UartDevice.recv(BASIC_ADDR_OFFSET(Data_Buffer,Model_Basic),KERNEL_DATA_BUFF_SIZE - Model_Basic);
	//Agr = (AGR_NAME*)BASIC_ADDR_OFFSET(Data_Buffer,Model_Basic);
	//UartDevice.recv((unsigned char*)&Data_Buffer[Model_Basic],KERNEL_DATA_BUFF_SIZE);
	//Agr = (AGR_NAME*)&Data_Buffer[Model_Basic];
	UartDevice.recv(BASIC_STATIC_ADDR_OFFSET(Data_Buffer,Model_Basic,unsigned char),KERNEL_DATA_BUFF_SIZE - Model_Basic);
	Agr = BASIC_STATIC_ADDR_OFFSET(Data_Buffer,Model_Basic,AGR_NAME);
	//Do not use switch
	//because switch will do memory alignment, and dozens of bytes will be added in case of too many conditions
	//which is unreasonable on micro MCU, so use if statement
	if(Agr->AGR_SIGN_NAME == AGR_HELLO){
		
		agr_hello_func();
		return true;
	
	}else if(Agr->AGR_SIGN_NAME == AGR_SET_MODEL){
	    
		int TempDataLen = Agr->AGR_DATA_LEN_NAME;
		if(agr_set_model_func(BASIC_STATIC_ADDR_OFFSET(Data_Buffer,(Model_Basic + AGR_SIZE),unsigned char)) == false){
			
			ErrLog.output(EXCEPTION_FAIL,"Failed to set model");
			return false;

		}else{
			Model_Basic = (((TempDataLen + AGR_SIZE)) + KERNEL_DATA_ALIG);
			return true;

		}
	
	}else if(Agr->AGR_SIGN_NAME == AGR_SET_MODEL_ATTR){
		
		if(agr_set_model_attr_func(BASIC_STATIC_ADDR_OFFSET(Data_Buffer,(Model_Basic + AGR_SIZE),unsigned char)) == false){
		
			ErrLog.output(EXCEPTION_FAIL,"Failed to set model attr");
			return false;

		}else{
			
			return true;

		}

	}else if(Agr->AGR_SIGN_NAME == AGR_FORECAST){
	
		
		if(agr_forecast_func(BASIC_STATIC_ADDR_OFFSET(Data_Buffer,(Model_Basic + AGR_SIZE),unsigned char),
				     Agr->AGR_DATA_LEN_NAME) == false){
		
			
			ErrLog.output(EXCEPTION_FAIL,"Failed to forecast data");
			return false;

		}else{
		
			
			return true;

		}

	}else if(Agr->AGR_SIGN_NAME == AGR_RESET_MEMORY){

            if(agr_reset_memory_func() == false){
                    
                    ErrLog.output(EXCEPTION_FAIL,"Failed to Reset Memory");
                    return false;

            }else{

                    return true;
            }

        }
	
	return true;	
}

int main(int argc, char *argv[]){
	
	if(setup() == false){
		
		ErrLog.output(EXCEPTION_ERROR,"Setup failed");
		return -1;

	}
	
		
	while(true){

		if(loop() == false){
			
			ErrLog.output(EXCEPTION_NORMAL,"Normal business exit");
			break;
		}
	}

	return 0; 
}

void agr_hello_func(){
	
	printk("hello word\n");

}

bool agr_set_model_func(unsigned char* ModelBuff){

	if(ModelBuff == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"Model buffer is null pointer");
		return false;
	}
	
	if(TfMl.Create_Destructor(ModelBuff) == false){
	
		ErrLog.output(EXCEPTION_FAIL,"Failed to construct model");
		return false;
	}

	printk("Set Model Success\n");
	return true;

}

bool agr_set_model_attr_func(unsigned char* AttrData){

	if(AttrData == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"AttrData is null pointer");
		return false;
	}

	if(TfMl.SetModelAttr(AttrData) == false){
		
		ErrLog.output(EXCEPTION_FAIL,"Failed to Set model attribute");
		return false;
	}
	
	printk("Set Model Attr Success\n");
	return true;
}

bool agr_forecast_func(unsigned char* Data,size_t Len){
	
	if(Data == NULL){
	
		ErrLog.output(EXCEPTION_ERROR,"Data is null pointer");
		return false;
	}

	if(Len <= 0){
		
		ErrLog.output(EXCEPTION_ERROR,"Len <= 0");
		return false;
	}
	
	//Tf2d module implements two methods. The first method obtains the full 
	//connection layer with the highest score, and the second method directly outputs labels
	//TODO 1
	TfMl.Write2dData(Data,Len);
	Tf2DResult result = TfMl.Operation(TfMl.GetModelAttr()->OutputLayerNumber);
	printk("Full connection layer Index:%d\n"
		   "Score:<%d>Label(%s)\n",result.index,result.score,
           TfMl.GetModelAttr()->OutPutData[TF2D_OUTPUT_LYER_NUMBER_INDEX + result.index]);
	//TODO 2
    /*
	printk("Laybel:<%s>\n",TfMl.Forecast(Data,Len).Name);
    */
	return true;

}
/*< reset memory >*/
bool agr_reset_memory_func(){
     
            Model_Basic = KERNEL_MODEL_BUFF_ADDR;
            printk("Reset Memory Success\n");
            return true;
}
