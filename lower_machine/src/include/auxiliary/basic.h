/*-------------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/12/2021  -  5:21 PM     |
 *-------------------------------------------------------
 *| basic.h                | Basic auxiliary functions  |
 *------------------------------------------------------*/

/*> Guard Area				*/
#ifndef __BASIC_H__
#define __BASIC_H__

/*> basic Common macro functions	*/
#define BASIC_ADDR_OFFSET(ptr,offset) ptr+offset			/*< Address offset	>*/
#define BASIC_STATIC_ADDR_OFFSET(ptr,offset,type) (type*)&ptr[offset]	/*< Static addr offset	>*/

/* End Guard Area			*/
#endif //__BAISC_H__ -- END
