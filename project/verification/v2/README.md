## Build Applications
The application supports Windows and Linux. If you need to install `opencv` under ubnutu:
```bash
sudo apt install libopencv-video-dev
```
There is no need to install under Windows. I have transplanted the opencv library on Windows in lib and configured it in cmake

compile
```bash
&: mkdir build & cd build
&: cmake .. & cmake --build .
-- Selecting Windows SDK version 10.0.19041.0 to target Windows 10.0.22000.
-- Configuring done
-- Generating done
-- Build files have been written to: upper/build
Microsoft (R) Build Engine version 16.11.2+f32259642 for .NET Framework
Copyright (C) Microsoft Corporation. All rights reserved.

  upper.vcxproj -> upper\bin\Debug\upper.exe
```
After compilation, the executable program will be generated in the source code `bin` directory

## Usage
Let firmware do AI prediction through application:
```bash
upper -i face1.jpg -w 56 -h 56 -f rgb565 -d /dev/ttyACM0 -b 115200
```
Of course, you can also use the camera to predict AI
```bash
upper -i 0 -w 56 -h 56 -f rgb 565 -d /dev/ttyACM0 -b 115200
```
If you want to check how many cameras are on the current computer, you can use `-s` to check
```bash
upper -s
```
