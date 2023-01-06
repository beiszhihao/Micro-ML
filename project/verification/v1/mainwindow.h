/*----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:02 AM |
 *----------------------------------------------------
 *| mainwindow.h           | The business layer      |
 *----------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define SKIN_WHITE              "/skin/white.qss"
#define SKIN_BLACK              "/skin/black.qss"
#define TMP_DIR                 m_this_path+"/temp"
#define HOME_DIR                ""
#define FFMPEG_PATH             m_this_path+"/bin/ffmpeg.exe"
#define FFMPEG_YUV_96x96        "-vf scale=96:96 -pix_fmt gray"
#define IMG2PATH(a)             a.path() + "/" + a.fileName()
#define SEND_YUV_TMP_FILE       m_this_path+"/temp/send.yuv"
#define READ_SIZE               512
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QtCharts>
#include <QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QTimer>
#include "waiting.h"

#include "ags.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void init_serial_port();
    ~MainWindow();
    void log(QString info);
    QString get_file_path(QString format);
    void init_series();
    void set_qss(QString path);
    void init_qss();
    void init_agr();
    void init_path();
    void init_image();
    void agr_hello();
    void init_timeout();
    unsigned char* agr_start(void* handel,void* data,size_t hand_len,size_t data_len);
    void agr_end(unsigned char* pack);
    void ai_log(QString str);
    void serial_open_ok();
    void serial_oepn_err();
    void wait_start(unsigned int timeout);
    void wait_end();
    void clear_tmp();
    void ffmpeg(QString src_file,QString arg,QString dest_file);
    void cmd(QString arg);
    void img2yuv_96x96(QString src_file,QString dest_file);
    QByteArray read_file(QString file_path);
    QByteArray pack_file(AGR_HAND_V1 arg1,QByteArray& file);
    void send(QByteArray& arry);
    void dy(int timeout,int courr);
    void send_file(QString file_path,int arg);
    void send_sign(int sign);
private slots:
    void on_conect_port_clicked();

    void on_open_image_clicked();

    void on_open_model_clicked();

    void on_checkBox_clicked();

    void readData();

    void handleTimeout();
    void DyTime();
    void send_model();

    void on_img_upload_clicked();

    void on_pushButton_clicked();

    void on_up_clicked();

    void on_set_config_clicked();

    void on_skin_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort    m_serial;
    QImage*        m_img;
    QImage*        m_reimg;
    QLineSeries*   m_run_state_series;
    QChart*        m_run_state_chart;
    QChartView*    m_occ_view;
    QChart*        m_occ_chart;
    QPieSeries*    m_occ_series;
    QChart*        m_bar_chart;
    QBarSet*       m_bar_ai_device;
    QBarSet*       m_bar_tensorflow;
    QBarSeries*    m_bar_series;
    QBarCategoryAxis* m_bar_axisX;
    QValueAxis*       m_bar_axisY;
    QTimer*        m_timeout;
    QTimer*        m_dy_time;
    QTimer*        m_sendmodel;
    waiting        m_wait;
    QFileInfo      m_ing_info;
    int            temp_i;
    QString        m_this_path;
    QStringList    m_skin;
    QByteArray     m_buffer;
    bool           m_set_model_and_config;
    bool           m_model;
    bool           m_model_attr;
    bool           m_image;
    bool           m_image_isok;
    bool           m_reset;

};

#endif // MAINWINDOW_H
