/*-----------------------------------------------------------------
 *| zhihao.zhou@intel.com | 11/08/2021  -  11:21 AM              |
 *-----------------------------------------------------------------
 *| uart.c            | uart server                               |
 *-----------------------------------------------------------------*/

/*>Hand Area			*/
#include <uart/uart.h>

/*>Realization Area		*/
/*>Constructor 			*/
Uart::Uart(){
	
	UartDevice = NULL;
	reset_recv();

}
/*>Dtconstructor		*/
Uart::~Uart(){

}
/*> reset recv 			*/
void Uart::reset_recv(){
	
	Data 		    = NULL;
	Data_Index	    = 0;
	Data_Len	    = 0;
	Recv_Len	    = UINT_MAX;
	Recv_Is_Ok	    = false;

//For Intel OSE
#ifdef UART_CORE_RECV_INTEL_OSE
    Recv_Hand_Ok    = false;
#endif

	ErrLog.output(EXCEPTION_SUCCESS,"Reset data success");

}
/*> call funci			*/
static void uart_fifo_callback(const struct device* dev, void* user_data){
	
	if(dev == NULL || user_data == NULL){

		return;

	}
	
	Uart* 		pUart   = (Uart*)user_data;
	MicroLog* 	pErrLog = pUart->getErrLog();

	if(uart_irq_update(pUart->GetDevice()) != 1){
		
		pErrLog->output(EXCEPTION_ERROR,"The serial port device status cannot be updated");
		return;

	}
	
	//Here, macros are used to define key members.
	//To modify the protocol or use a new protocol
	//please define it according to the protocol specification in config/uart.h
	if(uart_irq_rx_ready(pUart->GetDevice())){

    //Intel's ose FIFO driver will not read data one at a time
    //but will listen to the FIFO data status register
    //It will only fan when there is no data or the data reaches the buffer size
#ifdef UART_CORE_RECV_INTEL_OSE
        int Read_Size = uart_fifo_read(pUart->GetDevice(),(uint8_t*)&pUart->Data[pUart->Data_Index],pUart->Data_Len);
        if(Read_Size > 0){
            
            pUart->Data_Index += Read_Size;
            if(pUart->Data_Index >= AGR_SIZE && pUart->Recv_Hand_Ok == false){

#else
		if(uart_fifo_read(pUart->GetDevice(),(uint8_t*)&pUart->Data[pUart->Data_Index],pUart->Data_Len) > 0){
			
            pUart->Data_Index++;
			if(pUart->Data_Index == AGR_SIZE){

#endif 
				pUart->Recv_Len = AGR_SIZE+((AGR_NAME*)pUart->Data)->AGR_DATA_LEN_NAME;
	
//For Intel OSE
#ifdef UART_CORE_RECV_INTEL_OSE
                pUart->Recv_Hand_Ok = true;
#endif
            }

			if(pUart->Data_Index >= pUart->Recv_Len){
			    
				pUart->Recv_Is_Ok = true;
				uart_irq_rx_disable(pUart->GetDevice());

			}
		}
	}

}
/*>recv	this is 		*/
void Uart::recv(unsigned char* buff,uint32_t len){
	
	if(buff == NULL || len <= 0){
		
		ErrLog.output(EXCEPTION_ERROR,"Buff pointer is null or of length less than or equal to 0");	
		return;

	}

	if(UartDevice == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"The device pointer is null");
		return;

	}
	
	reset_recv();
	Data 		= buff;
	Data_Len 	= len;

	uart_irq_rx_enable(UartDevice);
	while(true){
	    
		if(Recv_Is_Ok == true){
			
			break;
		}

		k_sleep(K_MSEC(RECV_TIME_OUT));

	}
	uart_irq_rx_disable(UartDevice);

	ErrLog.output(EXCEPTION_SUCCESS,"Recv success");

}

/*>init				*/
bool Uart::init(const char* UartDeviceName,\
		uint32_t   Baudrate,\
		uint8_t     Parity,\
		uint8_t   StopBits,\
		uint8_t   DataBits,\
		uint8_t   FlowCtrl){
	
	if(UartDeviceName == NULL){

		ErrLog.output(EXCEPTION_ERROR,"The device name is a null pointer");
		return false;
	}
	
	UartDevice = const_cast<device*>(device_get_binding(UartDeviceName));
	
	if(UartDevice == NULL){

		ErrLog.output(EXCEPTION_ERROR,"Failed to get the device. The device is not registered:%s",UartDeviceName);
		return false;

	}
    
    /*
	struct uart_config Config = {
		.baudrate   =  Baudrate,
		.parity     =  Parity,
		.stop_bits  =  StopBits,
		.data_bits  =  DataBits,
		.flow_ctrl  =  FlowCtrl,
		};

	if(uart_configure(UartDevice,&Config) != 0) {
		
		ErrLog.output(EXCEPTION_ERROR,"Config %s Failed",UartDeviceName);
		return false;

	}
	
    */
	uart_irq_callback_user_data_set(UartDevice,uart_fifo_callback,this);
	ErrLog.output(EXCEPTION_SUCCESS,"%s init succeeded",UartDeviceName);	
	return true;
}

/*> debug model 		*/
void Uart::debug(bool IsDebug){
	
	ErrLog.debug(IsDebug);

}

/*> get device handel		*/
struct device*	Uart::GetDevice(){
	
	if(UartDevice == NULL){
		
		ErrLog.output(EXCEPTION_ERROR,"The device pointer is null");
		return NULL;

	}	

	return UartDevice;
}

/*> get err log ptr 		*/
MicroLog* Uart::getErrLog(){

	return &ErrLog;
}
