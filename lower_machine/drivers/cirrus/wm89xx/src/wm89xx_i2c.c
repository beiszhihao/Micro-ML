/*-----------------------------------------------------------------------
 *| zhihao.zhou@intel.com  | 1/10/2022  -  2:05  PM                     |
 *-----------------------------------------------------------------------
 *| wm89xx_i2c.c           | I2c Driver 				|
 *-----------------------------------------------------------------------*/

/*>Head 													   */
#include <wm89xx/wm89xx.h>							/*< Head File 			  >*/
#include <wm89xx/wm89xx_i2c.h>							/*< I2C Head File		  >*/

/*>Define													   */
#ifdef WM89XX_BUS_I2C								/*< Verify that buszon is defined >*/

/*>Func														   */
/*>>Check Bus Io												   */
static int WM89XX_Bus_Check_I2c(const union WM89XX_Bus* bus){
	
	return 0;

}
/*>>I2C Read													   */
static int WM89XX_Bus_Io_Read_I2c(const union WM89XX_Bus* bus,uint8_t start,uint8_t *buf,int size){

	return 0;

}
/*>>I2C Write													   */
static int WM89XX_Bus_Io_Write_I2c(const union WM89XX_Bus* bus,uint8_t start,uint8_t*buf,int size){

	return 0;

}

/*>I2C Struct													   */
const struct WM89XX_Bus_Io wm89xx_bus_io_i2c = {
	.check = WM89XX_Bus_Check_I2c,
	.read = WM89XX_Bus_Io_Read_I2c,
	.write = WM89XX_Bus_Io_Write_I2c,
};

/*>End Define													   */
#endif //--WM89XX_BUS_I2C
