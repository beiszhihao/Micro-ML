/*-----------------------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/08/2021  -  11:16 AM              |
 *-----------------------------------------------------------------
 *| uart.h            | uart server                               |
 *-----------------------------------------------------------------*/

/*>Guard Area		*/
#ifndef __UART_H__
#define __UART_H__

/*> Hand Area		*/
#include <micro_log/micro_log.h>	/*< log		>*/
#include <config/uart.h>		/*< config uart >*/
#include <device.h>
#include <drivers/uart.h>

/*> Class Area		*/
class Uart{
	
	public:
		Uart() noexcept;
		~Uart();
		bool init(const char* UartDeviceName,\
				uint32_t    Baudrate,\
				uint8_t     Parity = UART_CFG_PARITY_NONE,\
				uint8_t   StopBits = UART_CFG_STOP_BITS_1,\
				uint8_t   DataBits = UART_CFG_DATA_BITS_8,\
				uint8_t   FlowCtrl = UART_CFG_FLOW_CTRL_NONE);
		void 			debug(bool IsDebug);
		struct device* 		GetDevice();
		void 			reset_recv();
		void 			recv(unsigned char* buff,uint32_t len);
		MicroLog* 		getErrLog();

		/*< var >*/
		unsigned char* Data;		    /*< Data 	>*/
		uint32_t       Data_Index;	    /*< Data index  >*/
		uint32_t       Data_Len;	    /*< Data len	    >*/
		uint32_t       Recv_Len;	    /*< Recv Len	    >*/
		bool	       Recv_Is_Ok;	    /*< Recv OK	        >*/

//For Intel OSE
#ifdef UART_CORE_RECV_INTEL_OSE
        bool           Recv_Hand_Ok;    /*< Recv Hand Ok    >*/
#endif
	private:
		struct device* UartDevice;	/*< Uart Device >*/
		MicroLog       ErrLog;		/*< Error log	>*/
};

/*>End Guard Area	*/
#endif //__UART_H__ -- END

