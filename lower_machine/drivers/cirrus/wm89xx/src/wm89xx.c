/*-----------------------------------------------------------------------
 *| zhihao.zhou@intel.com  | 1/08/2022  -  11:55  AM                    |
 *-----------------------------------------------------------------------
 *| wm89xx.c               | Driver of multimedia coder for wm89 series |
 *-----------------------------------------------------------------------*/

/*>Config Drivers							 */
#ifdef CONFIG_WM89XX

/*> Hand Area								 */
#include <wm89xx/wm89xx.h>			/*< Wm89xx hand file 	>*/
#include <wm89xx/wm89xx_i2c.h>			/*< I2c Driver		>*/

/*> Check Area                                                           */
#if DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0
#warning "WM89xx series coder is not defined in DTS"
#endif

/*> Driver Attribute							 */
#define INIT_PRIO 35

/*>Func									 */
static int wm89xx_init(const struct device* dev){
	
	return 0;
}

/*>Api									*/
struct WM89XX_Api wm89xx_api;

/*>Driver Register							*/
#define WM89XX_CONFIG_I2C(inst)						\
	{								\
		.bus.i2c = I2C_DT_SPEC_INST_GET(inst), 			\
		.bus_io = &wm89xx_bus_io_i2c,				\
	}

#define WM89XX_DEFINE(inst)						\
	static struct WM89XX_Data wm89xx_data_##inst; 			\
	static const struct WM89XX_Config wm89xx_config_##inst = 	\
					  WM89XX_CONFIG_I2C(inst); 	\
	DEVICE_DT_INST_DEFINE(inst,					\
			      wm89xx_init,				\
			      NULL,					\
			      &wm89xx_data_##inst,			\
			      &wm89xx_config_##inst,			\
			      POST_KERNEL,				\
			      INIT_PRIO,				\
			      &wm89xx_api);				

DT_INST_FOREACH_STATUS_OKAY(WM89XX_DEFINE)

/*>End Config Drivers							 */
#endif //CONFIG_WM89XX -- END
