/*----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:05 AM |
 *----------------------------------------------------
 *| ags.h                  | Communication protocol  |
 *----------------------------------------------------*/

#ifndef AGS_H
#define AGS_H
#define HANDSHAKE 0
#define HELLO 0
#define FORECAST 3
#define SET_MODEL 1
#define SET_MODEL_ATTR 2
#define RESET_MEMORY    4
#define RC_HELLO "hello word"
#define RC_SET_MODEL "Set Model Success"
#define RC_SET_MODEL_ATTR "Set Model Attr Success"
#define RC_LABEL    "Laybel:"
#define RC_SCORE    "Score:"
#define RC_RESET   "Reset Memory Success"
struct AGR_HAND_V1{
    int sign;
    int data_size;
};

struct AGR_HELLO_V1{
    int greet;
};
struct AGR_IMAGE_V1{
    int format;
    int size;
    unsigned char* image_data;

};
struct AGR_MODEL_V1{
    int format;
    int size;

};

#endif // AGS_H
