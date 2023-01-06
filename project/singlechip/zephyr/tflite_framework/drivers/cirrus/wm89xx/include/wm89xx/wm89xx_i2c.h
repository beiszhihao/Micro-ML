/*-----------------------------------------------------------------------
 *| zhihao.zhou@intel.com      | 1/10/2022  -  2:08  PM                 |
 *-----------------------------------------------------------------------
 *| wm89xx_i2c.h               | I2c Driver Head File 			|
 *-----------------------------------------------------------------------*/

/*>Check		*/
#if WM89XX_BUS_I2C
/*>>Struct		*/
extern const struct WM89XX_Bus_Io wm89xx_bus_io_i2c;
/*>>Func		*/
extern static int WM89XX_Bus_Check_I2c(const union WM89XX_Bus* bus);
extern static int WM89XX_Bus_Io_Read_I2c(const union WM89XX_Bus* bus,uint8_t start,uint8_t *buf,int size);
extern static int WM89XX_Bus_Io_Write_I2c(const union WM89XX_Bus* bus,uint8_t start,uint8_t*buf,int size);

/*>End Chek		*/
#endif //WM89XX_BUS_I2C -- END
