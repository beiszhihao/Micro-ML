#ifdef __linux__
#include <termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#ifdef WIN32
#include "getopt.h"
#include <windows.h>
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>

#define IMAGE_HEIGHT            56
#define IMAGE_WIDTH             56
#define IMAGE_BYTE              2
#define IMAGE_SIZE              (IMAGE_HEIGHT * IMAGE_WIDTH) * IMAGE_BYTE
#define USART_READ_TIMEOUT      1000
#define USART_READ_START	0x334455
#define USART_READ_CRC          0x554433
#define RGB888_RED      	0x00ff0000
#define RGB888_GREEN    	0x0000ff00
#define RGB888_BLUE     	0x000000ff

struct usart_data {
	uint32_t st;
	uint8_t  data[IMAGE_SIZE];
	uint32_t crc;
};

using namespace cv;
using namespace std;
const char *srcstr = "i:w:h:f:d:b:cs";

#ifdef __linux__
int uart_set(int fd, int baudrate, int bits, char parity, int stop, char flow)
{
    struct termios termios_uart;
    int ret = 0;
    speed_t uart_speed = 0;

    memset(&termios_uart, 0, sizeof(termios_uart));
    ret = tcgetattr(fd, &termios_uart);
    if (ret == -1) {
        printf("tcgetattr failed\n");
        return -1;
    }
    
    switch (baudrate) {
        case 0:      uart_speed = B0;      break;
        case 50:     uart_speed = B50;     break;
        case 75:     uart_speed = B75;     break;
        case 110:    uart_speed = B110;    break;
        case 134:    uart_speed = B134;    break;
        case 150:    uart_speed = B150;    break;
        case 200:    uart_speed = B200;    break;
        case 300:    uart_speed = B300;    break;
        case 600:    uart_speed = B600;    break;
        case 1200:   uart_speed = B1200;   break;
        case 1800:   uart_speed = B1800;   break;
        case 2400:   uart_speed = B2400;   break;
        case 4800:   uart_speed = B4800;   break;
        case 9600:   uart_speed = B9600;   break;
        case 19200:  uart_speed = B19200;  break;
        case 38400:  uart_speed = B38400;  break;
        case 57600:  uart_speed = B57600;  break;
        case 115200: uart_speed = B115200; break;
        case 230400: uart_speed = B230400; break;
        default: printf("Baud rate not supported\n"); return -1;
    }
    cfsetspeed(&termios_uart, uart_speed);

    switch (bits) {
        case 5:
            termios_uart.c_cflag &= ~CSIZE;
            termios_uart.c_cflag |= CS5;
        break;

        case 6:
            termios_uart.c_cflag &= ~CSIZE;
            termios_uart.c_cflag |= CS6;
        break;

        case 7:
            termios_uart.c_cflag &= ~CSIZE;
            termios_uart.c_cflag |= CS7;
        break;

        case 8:
            termios_uart.c_cflag &= ~CSIZE;
            termios_uart.c_cflag |= CS8;
        break;

        default:
            printf("Data bits not supported\n");
            return -1;
    }

    switch (parity) {
        case 'n':
        case 'N':
            termios_uart.c_cflag &= ~PARENB;
            termios_uart.c_iflag &= ~INPCK;
        break;

        case 'o':
        case 'O':
            termios_uart.c_cflag |= PARENB;
            termios_uart.c_cflag |= PARODD;
            termios_uart.c_iflag |= INPCK;
            termios_uart.c_iflag |= ISTRIP;
        break;

        case 'e':
        case 'E':
            termios_uart.c_cflag |= PARENB;
            termios_uart.c_cflag &= ~PARODD;
            termios_uart.c_iflag |= INPCK;
            termios_uart.c_iflag |= ISTRIP;
        break;

        default:
            printf("Parity not supported\n");
            return -1;
    }

    switch (stop) {
        case 1: termios_uart.c_cflag &= ~CSTOPB; break;
        case 2: termios_uart.c_cflag |= CSTOPB;  break;
        default: printf("Stop bits not supported\n");
    }

    switch (flow) {
        case 'n':
        case 'N':
            termios_uart.c_cflag &= ~CRTSCTS;
            termios_uart.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;

        case 'h':
        case 'H':
            termios_uart.c_cflag |= CRTSCTS;
            termios_uart.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;

        case 's':
        case 'S':
            termios_uart.c_cflag &= ~CRTSCTS;
            termios_uart.c_iflag |= (IXON | IXOFF | IXANY);
        break;

        default:
            printf("Flow control parameter error\n");
            return -1;
    }

    termios_uart.c_cflag |= CLOCAL;
    termios_uart.c_cflag |= CREAD;

    termios_uart.c_oflag &= ~OPOST;

    termios_uart.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    termios_uart.c_cc[VTIME] = 1;
    termios_uart.c_cc[VMIN]  = 1;

    tcflush(fd, TCIFLUSH);

    ret = tcsetattr(fd, TCSANOW, &termios_uart);
    if (ret == -1) {
        printf("tcsetattr failed\n");
    }

    return ret;
}
#endif
#ifdef __linux__
void send_usart_data(int fd, Mat *img, int width, int height, struct usart_data *send_data, string format) {
#endif
#ifdef WIN32
void send_usart_data(HANDLE hCom, Mat *img, int width, int height, struct usart_data *send_data, string format) {
#endif		
			resize(*img, *img, Size(width, height));
                        memset(send_data, 0, sizeof(struct usart_data));
                        if (format == "rgb565") {

                                cvtColor(*img, *img, COLOR_BGR2BGR565);
                        }
                        send_data->st = USART_READ_START;
                        send_data->crc = USART_READ_CRC;
                        memcpy(send_data->data, img->data, IMAGE_SIZE);
#ifdef __linux__
                        write(fd, (char*)send_data, sizeof(struct usart_data));
#endif

#ifdef WIN32
                        WriteFile(hCom, (LPCVOID)send_data, sizeof(struct usart_data), NULL, NULL);
#endif

}

void sear_camera() {
	VideoCapture capture;
	int i = 0;

	while (true) {

		capture.open(i);
		if(!capture.isOpened()) {
		
			return;
		}

		printf("camera%d\n", i++);
	}

}

int main(int argc, char *argv[]) {
#ifdef __linux__
	int fd;
#endif
#ifdef WIN32
	HANDLE hCom;
	DCB dcb;
#endif
	int ch;
	int width;
	int height;
	int baud = 115200;
	bool camera = false;
	string input_file;
	string format;
	string dev;
	Mat img;
	VideoCapture capture;
	struct usart_data send_data;
	
	while ((ch = getopt(argc, argv, srcstr)) != -1) {
		
		switch (ch) {
			case 'i':
			     	input_file = optarg;
			break;
			case 'w':
				width = atoi(optarg);
			break;
			case 'h':
				height = atoi(optarg);
			break;
			case 'f':
				format = optarg;
			break;
			case 'd':
				dev = optarg;
			break;
			case 'b':
				baud = atoi(optarg);
			break;
			case 'c':
				camera = true;
			break;
			case 's':
				sear_camera();
				return 0;
			break;
		}
	
	}
#ifdef __linux__
	fd = open(dev.c_str(), O_RDWR | O_NOCTTY );
	if (fd < 0) {
		
		cout << "Unable to open device: " << dev << endl;
		return -1;
	}

	if (uart_set(fd, baud, 8, 'n', 1, 'n') == -1) {
		
		cout << "Failed to configure the serial port: " << dev << " " << baud << endl;
		return -1;
	}
#endif
#ifdef WIN32
	hCom = CreateFile(dev.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if (hCom == (HANDLE)-1) {
		
		 cout << "Unable to open device: " << dev << endl;
		 return -1;
	}
	SetupComm(hCom, sizeof(struct usart_data), 1024);
	GetCommState(hCom, &dcb);
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONE5STOPBITS;
	SetCommState(hCom, &dcb);
#endif
	if (!camera) {
		
		img = cv::imread(input_file);
		if (img.data == NULL) {
		
			cout << "Unable to open image file: " << input_file << endl;
			return -1;
		}

	} else {
		
		capture.open(atoi(input_file.c_str()));
		if(!capture.isOpened()) {
			
			cout << "Failed to open the camera " << input_file << endl;
			return -1;
		}
	}
	
	while (true) {

		if(camera) {

			capture >> img;
			if(img.empty()) {

				cout << "Failed to read the picture from the camera " << input_file << endl;
				return -1;
			}
		}
		
		send_usart_data(
#ifdef __linux__
				fd, 
#endif
#ifdef WIN32
				hCom, 
#endif
				&img, width, height, &send_data, format);

		if (!camera) {
			
			break;
		}

#ifdef __linux__
		usleep(1000);
#endif
#ifdef WIN32
		Sleep(1000);
#endif

	}

#ifdef __linux
	close(fd);
#endif
#ifdef WIN32
	CloseHandle(hCom);
#endif

	cout << "Send image data done" << endl;

	return 0;
}
