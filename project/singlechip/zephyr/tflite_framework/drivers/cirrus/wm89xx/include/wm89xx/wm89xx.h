/*-----------------------------------------------------------------------
 *| zhihao.zhou@intel.com  | 1/08/2022  -  11:55  AM                    |
 *-----------------------------------------------------------------------
 *| wm89xx.h               | Driver of multimedia coder for wm89 series |
 *-----------------------------------------------------------------------*/

/*>Config Drivers               							 */
#ifdef CONFIG_WM89XX

/*>Guard Area										 */
#ifndef __WM89XX_H__
#define __WM89XX_H__

/*> Hand File										 */
#include <zephyr/types.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/i2c.h>						/*< I2C Drivers	>*/

/*> DTS Attribute               							  */
#define DT_DRV_COMPAT           cirrus_wm89xx    			/*< Drivers Name >*/
#define WM89XX_BUS_I2C		DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)	/*< I2C BUS	 >*/

/*> Typedef Struct AREA									    */
/*>> Type										    */
union WM89XX_Bus{
	
#if WM89XX_BUS_I2C
	struct i2c_dt_spec i2c;
#endif

};
/*>> Func										    */
typedef int (*WM89XX_Bus_Check_Func)(const union WM89XX_Bus* bus);
typedef int (*WM89XX_Io_Read_Func)(const union WM89XX_Bus* bus,uint8_t start,uint8_t *buf,int size);
typedef int (*WM89XX_Io_Write_Func)(const union WM89XX_Bus* bus,uint8_t start,uint8_t *buf,int size);
/*>> Data										     */
struct WM89XX_Data{

	int Id;								/*< Device Id	    >*/	

};
/*>> Io											     */
struct WM89XX_Bus_Io{
	WM89XX_Bus_Check_Func check;
	WM89XX_Io_Read_Func read;
	WM89XX_Io_Write_Func write;
};
/*>> Config										     */
struct WM89XX_Config{
	union WM89XX_Bus bus;
	const struct WM89XX_Bus_Io *bus_io;
};
/*>> Api										     */
struct WM89XX_Api{
	
	int id;
};

/*>End Guard Area									     */
#endif 	//__CONFIG_WM89XX_H__ 	-- END

/*>End Config Drivers									     */
#endif	//CONFIG_WM89XX	-- END
