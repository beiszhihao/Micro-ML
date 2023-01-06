# Project Introduction
This is the lower computer program, which depends on the zephyr compilation system: West

The project uses modular programming and encapsulates common API functions. The kernel directory is the starting point of everything. You can find the call rules in the kernel / main.cc file

This project needs to be used with its own host computer

Of course, it can not be matched, because this project uses serial communication. You only need to find a software that supports serial communication and communicate through the communication protocol written in the code.

# Directory System
## Catalog Rules
```bash
├── boards
│   └── mimxrt1060_evk.overlay
├── CMakeLists.txt
├── prj.conf
└── src
    ├── include
    │   ├── auxiliary
    │   │   └── basic.h
    │   ├── config
    │   │   ├── image.h
    │   │   ├── micro_log.h
    │   │   ├── system.h
    │   │   ├── tfmicro.h
    │   │   └── uart.h
    │   ├── micro_log
    │   │   └── micro_log.h
    │   ├── tfmicro
    │   │   ├── tf2d.h
    │   │   ├── tfmicro.h
    │   │   └── tools.h
    │   └── uart
    │       └── uart.h
    ├── kernel
    │   └── main.cc
    ├── lib
    │   └── tf-micro
    │       ├── micro_interpreter.cc_modify
    │       └── micro_interpreter.h_modify
    └── modules
        ├── micro_log
        │   └── micro_log.cc
        ├── tfmicro
        │   ├── tf2d.cc
        │   └── tfmicro.cc
        └── uart
            └── uart.cc

15 directories, 21 files
```

## Main Directory
|  Directory Name   | Effect  |
|  ----  | ----  |
| include  | Common and module header files |
| kernel  | Main startup function |
| lib  |  Transplanted third-party implementation library |
| modules | Module functions implemented |

## Directory introduction
|  Directory Name   | Effect  |
|  ----  | ----  |
| boards | Adaptation file for MCU zephyr |
| include/config | System and module properties profile |
| include/micro_log | Header file of log module |
| include/tfmicro | Implementation of tfmicro module |
| include/uart | Header file of serial port module |
| modules/micro_log | Implementation of log module |
| modules/tfmicro | Implementation of tfmicro module |
| modules/uart | Implementaion of uart module |

## Role of main documents
|  File Name   | Effect  |
|  ----  | ----  |
| boards/mimxrt1060_evk.overlay| NXP mimxrt1060 Adaptive DTS description file  |
| lib/tf-micro/micro_interpreter.cc_modify | The revised interpreter tool adds code suitable for reconstruction |
| modules/tfmicro/tfmicro.cc | Tfmicro based tools, all AI related operations should inherit it |
| modules/tfmicro/tf2d.cc | Inherit the operator module for 2D image recognition implemented by tfmicro |
| kernel/main.cc  | The starting point of everything |
| modules/micro_log/micro_log.cc | Log system |
| modules/uart/uart.cc  | Uart system |

# Build
## 1. Install Zephyr
First, you need to configure zephyr on your computer, and the version > = 2.7.99 (because this is the author's)

Zephyr Doc:[zephyr](https://docs.zephyrproject.org/latest/)

## 2. Clone Warehouse
First clone the repository
```bash
git clone https://github.com/intel-sandbox/zhihao_Micro-ML.git
```
Then enter the project directory
```bash
cd zhihao_Micro-ML/lower_machine/
```
## 3. Replace interpreter tool
Put the modified interpreter tool into the zephyr directory. The directory here is modified according to the actual zephyr directory of your computer
It is recommended to note zephyr's original micro first_ Interpreter.h and micro_ interpreter.cc
```bash
cp ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter.h ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter_back.h
cp ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter.cc ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter_back.cc 
cp src/lib/tf-micro/micro_interpreter.h_modify ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter.h
cp src/lib/tf-micro/micro_interpreter.cc_modify ~/zephyrproject/modules/lib/tflite-micro/tensorflow/lite/micro/micro_interpreter.cc
```
## 4. Compile
Enter the zephyr directory
Change according to your actual directory
```bash
cd ~/zephyrproject/zephyr
```
Using zephyr's west build tool Jin
```bash
west build -p auto -b your_board ~/zhihao_Micro-ML/lower_machine
```
Construction results
```bash
[252/252] Linking CXX executable zephyr/zephyr.elf
Memory region         Used Size  Region Size  %age Used
           SDRAM:          0 GB        32 MB      0.00%
           FLASH:       79504 B         8 MB      0.95%
            SRAM:      625408 B       768 KB     79.52%
            ITCM:          0 GB       128 KB      0.00%
            DTCM:          0 GB       128 KB      0.00%
        IDT_LIST:          0 GB         2 KB      0.00%
```
Then you can find the zephyr.bin file in the build/zephyr folder. This file is the final generated. Bin file, which can be burned to your MCU
After burning and restarting MCU, you can see the following output through the serial port
```bash
UART_1 init succeeded
Tf2d initialized successfully
Reset data success
```
## 5. Debug
Debug debugging mode is enabled in the code. Various debugging information will be printed when the module is executed
```bash
Module Name: TF2D
      State: SUCCESS
  File Name: tf2d.cc
  Func Name: Create_Destructor
  Code Line: 122
     Messge: The model and calculation view are established successfully
```
You can find the relevant open code in the setup function of kernel/main.cc
```c++
bool setup(){

        UartDevice.debug(true);
        if(UartDevice.init(DEVICE_UART_NAME,DEVICE_UART_BAUDRATE) == false){

                ErrLog.output(EXCEPTION_ERROR,"Failed to initialize UART device");
                return false;

        }

        TfMl.debug(true);
        if(TfMl.init() == false){

                ErrLog.output(EXCEPTION_ERROR,"Failed to initialize TF2D Model");
                return false;
        }

        return true;

}
```

# Business Code
The loop of kernel/main.cc is the event code, which is responsible for reading data from the serial port and parsing the protocol to call different event events
```c++
bool loop(){

        memset(Data_Buffer,0,KERNEL_DATA_BUFF_SIZE);
        UartDevice.recv(Data_Buffer,KERNEL_DATA_BUFF_SIZE);
        Agr = (AGR_NAME*)Data_Buffer;
        //Do not use switch
        //because switch will do memory alignment, and dozens of bytes will be added in case of too many conditions
        //which is unreasonable on micro MCU, so use if statement
        if(Agr->AGR_SIGN_NAME == AGR_HELLO){

                agr_hello_func();
                return true;

        }else if(Agr->AGR_SIGN_NAME == AGR_SET_MODEL){

                if(agr_set_model_func(BASIC_ADDR_OFFSET(Data_Buffer,AGR_SIZE)) == false){

                        ErrLog.output(EXCEPTION_FAIL,"Failed to set model");
                        return false;

                }else{

                        return true;

                }

        }else if(Agr->AGR_SIGN_NAME == AGR_SET_MODEL_ATTR){

                if(agr_set_model_attr_func(BASIC_ADDR_OFFSET(Data_Buffer,AGR_SIZE)) == false){

                        ErrLog.output(EXCEPTION_FAIL,"Failed to set model attr");
                        return false;

                }else{

                        return true;

                }

        }else if(Agr->AGR_SIGN_NAME == AGR_FORECAST){


                if(agr_forecast_func(BASIC_ADDR_OFFSET(Data_Buffer,AGR_SIZE),
                                     Agr->AGR_DATA_LEN_NAME) == false){


                        ErrLog.output(EXCEPTION_FAIL,"Failed to forecast data");
                        return false;

                }else{


                        return true;

                }
        }

        return true;
}
```
Because of good encapsulation, you will see more clearly when reading these codes, and the call of event event event is also very clear
```c++
bool agr_set_model_func(unsigned char* ModelBuff){

        if(ModelBuff == NULL){

                ErrLog.output(EXCEPTION_ERROR,"Model buffer is null pointer");
                return false;
        }

        if(TfMl.Create_Destructor(ModelBuff) == false){

                ErrLog.output(EXCEPTION_FAIL,"Failed to construct model");
                return false;
        }

        return true;

}

bool agr_set_model_attr_func(unsigned char* AttrData){

        if(AttrData == NULL){

                ErrLog.output(EXCEPTION_ERROR,"AttrData is null pointer");
                return false;
        }

        if(TfMl.SetModelAttr(AttrData) == false){

                ErrLog.output(EXCEPTION_FAIL,"Failed to Set model attribute");
                return false;
        }

        return true;
}
bool agr_forecast_func(unsigned char* Data,size_t Len){

        if(Data == NULL){

                ErrLog.output(EXCEPTION_ERROR,"Data is null pointer");
                return false;
        }

        if(Len <= 0){

                ErrLog.output(EXCEPTION_ERROR,"Len <= 0");
                return false;
        }

        //Tf2d module implements two methods. The first method obtains the full
        //connection layer with the highest score, and the second method directly outputs labels
        //TODO 1
        /*
        Write2dData(Data,Len);
        Tf2DResult result = TfMl.Operation(TfMl.GetModelAttr()->OutputLayerNumber);
        printk("Full connection layer Index:%d\n"
                "Score:%d\n",result.index,result.score);
        */
        //TODO 2
        printk("Laybel:%s\n",TfMl.Forecast(Data,Len).Name);

        return true;

}
```
Because it runs on MCU, the key codes are defined as global variables and have more memory in the global area
```c++
namespace {

        Uart                                    UartDevice;                                     /*< Uart Module >*/
        MicroLog                                ErrLog;                                         /*< Log Module  >*/
        Tf2D                                    TfMl;                                           /*< TinyMl      >*/
        unsigned char                           Data_Buffer[KERNEL_DATA_BUFF_SIZE];             /*< Buffer      >*/
        /*< Communication protocol using v1 >*/
        AGR_NAME*                               Agr;                                            /*< V1 agr >*/

}
```
You can see which modules are included in the header file
```c++
#include <tfmicro/tools.h>      /*< Contains all tfmicro tool libraries                 >*/
#include <tfmicro/tf2d.h>       /*< 2D AI for image processing                          >*/
#include <uart/uart.h>          /*< Serial port module                                  >*/
#include <micro_log/micro_log.h>/*< Log tool                                            >*/
#include <config/image.h>       /*< Contains the configuration required by the image    >*/
#include <config/system.h>      /*< Config system-related properties                    >*/
#include <auxiliary/basic.h>    /*< Common functions         
```
