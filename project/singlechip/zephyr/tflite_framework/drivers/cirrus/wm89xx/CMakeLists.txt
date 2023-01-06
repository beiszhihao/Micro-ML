#-----------------------------------------------------------------------
#| zhihao.zhou@intel.com  | 1/08/2022  -  12:27  PM                    |
#-----------------------------------------------------------------------
#| CMakeFile.txt          | Compile wm89xx drive  		       |
#-----------------------------------------------------------------------

#Get Source File
FILE(GLOB WM89xx_Source_File include/*.h src/*.c)

include_directories(include)

#Init Zephyr Lib
zephyr_library()

#Set Compile
zephyr_library_sources(${WM89xx_Source_File})
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
