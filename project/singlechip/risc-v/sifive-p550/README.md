# Environmental dependence
The board requires system support above `Ubuntu 18.04`, You also need to install `Python`

# Transplantation method
Download the firmware to any directory on the target board

Execution:
```bash
python ./tf_server.py
```

# Command support：
* SEND_MODEL_CMD 
* SEND_INPUT_CMD
* SEND_RUN_CMDS
* SEND_INPUT_JPG_CMD
* GET_RESULT_CMDS
* GET_CPUINFO_CMDS

# Communication protocol - V2
The total package size is `1024` for each transmission
``` c++
struct protocol {
	uint8_t cmd;
	uint8_t end;
	short f_size;
	uint32_t t_size;
	char data[0]
}
```

