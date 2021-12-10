/*-----------------------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/05/2021  -  14:26 PM              |
 *-----------------------------------------------------------------
 *| micro_log.cc           | log server			          |
 *-----------------------------------------------------------------*/
/*> Hand Area          */
#include <micro_log/micro_log.h>

/*> constructed		*/
MicroLog::MicroLog(){
	
	memset(buff,0,BUFF_SIZE);
	IsDebug = false;
}

/*> Open Debug model 	*/
void MicroLog::debug(bool Open){

	IsDebug = Open;
}

/*> deconstructed	*/
MicroLog::~MicroLog(){

}

/*> output log 		*/
void MicroLog::__print(const char* Format){
	
	if(Format == NULL){
		
		return;

	}

	if(IsDebug == false){
	
		return;

	}	

	printk("%s\n",Format);

}

/*     Effect> Format output log
 * Parameter1> Format log
 * Parameter2> Deformable parameter
 *     Return> No return value
 */
void MicroLog::output(int Grade,const char* Format,...){
	
	if(Format == NULL){

		return;
	
	}
	
	if(Grade >= EXCEPTION_MAX){
	
		return;
	}	
	
	memset(buff,0,BUFF_SIZE);
	
	va_list arg;
	va_start(arg,Format);
	vsnprintk(buff,BUFF_SIZE,Format,arg);
	va_end(arg);

	__print(buff);

}
