/*-----------------------------------------------------------------
 *| zhihao.a.zhou@outlook.com | 11/05/2021  -  14:26 PM           |
 *-----------------------------------------------------------------
 *| micro_log.h               | log server		 	  |
 *-----------------------------------------------------------------*/
/*> Guard Area 		*/
#ifndef __MICRO_LOG_H__
#define __MICRO_LOG_H__

/*> Hand Area 		*/
#include <config/micro_log.h>		/*< Config Log >*/
#include <zephyr.h>
#include <sys/printk.h>
#include <stdarg.h>
#include <string.h>

/*> Class Area		*/
class MicroLog{

	public:
		MicroLog() noexcept;
		~MicroLog();
		void debug(bool Open);
		void output(int Grade,const char* Format,...);
	private:
		char buff[BUFF_SIZE];
		void __print(const char* Format);
		bool IsDebug;

};

/*> End Guard Area 	*/
#endif //__MICRO_LOG_H__ -- END
