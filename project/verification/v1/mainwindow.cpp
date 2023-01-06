/*----------------------------------------------------
 *| zhihaox.zhou@intel.com | 11/10/2021  -  11:02 AM |
 *----------------------------------------------------
 *| mainwindow.cpp         | The business layer      |
 *----------------------------------------------------*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
bool hello = false;
bool verification = false;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //init path
    init_path();
    //init
    init_serial_port();
    //image
    init_image();
    //qss
    init_qss();
    //agr
    init_agr();
    //time
    init_timeout();

    //set basic
    m_set_model_and_config = false;
    m_model                = false;
    m_model_attr           = false;
    m_image                = false;
    m_image_isok           = false;
    m_reset                = false;

}

void MainWindow::init_image(){

    m_img = new QImage;
    m_reimg = new QImage;

    if(m_img == nullptr || m_reimg == nullptr){
        log("Error Image");
    }

}

void MainWindow::init_path(){

    m_this_path = QCoreApplication::applicationDirPath();

}

void MainWindow::send_model(){

    AGR_HAND_V1 agr;
    QByteArray file;
    QByteArray send_buff;
    file = read_file(ui->model_path->text());
    agr.sign = SET_MODEL;
    agr.data_size = file.size();
    send_buff = pack_file(agr,file);
    send(send_buff);
    this->log("Send Model Attr");
    m_sendmodel->stop();
}

void MainWindow::init_timeout(){
     m_timeout = new QTimer(this);
     connect(m_timeout, SIGNAL(timeout()), this, SLOT(handleTimeout()));

     m_dy_time = new QTimer(this);
     connect(m_dy_time, SIGNAL(timeout()), this, SLOT(DyTime()));

     m_sendmodel = new QTimer(this);
     connect(m_sendmodel, SIGNAL(timeout()), this, SLOT(send_model()));
}

void MainWindow::set_qss(QString path){
    QFile qss(path);
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}
void MainWindow::init_qss(){
    m_skin << SKIN_WHITE << SKIN_BLACK;
    qDebug() << m_this_path+m_skin[0];
    set_qss(m_this_path+m_skin[0]);
}
void MainWindow::init_series(){
    //run state
    m_run_state_series = new QLineSeries();
    m_run_state_series->append(0, 6);
    m_run_state_series->append(2, 4);
    m_run_state_series->append(3, 8);
    m_run_state_series->append(7, 4);
    m_run_state_series->append(10, 5);


    m_run_state_chart = new QChart();
    m_run_state_chart->legend()->hide();
    m_run_state_chart->addSeries(m_run_state_series);
    m_run_state_chart->createDefaultAxes();
    m_run_state_chart->setTitle("Computational Power");
    m_run_state_chart->setAnimationOptions(QChart::AllAnimations);
    m_run_state_chart->setDropShadowEnabled(true);
    m_run_state_chart->setAutoFillBackground(true);
    m_run_state_chart->setTheme(QChart::ChartThemeBlueCerulean);

    ui->graphicsView->setChart(m_run_state_chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    //Occupancy Ratio
    m_occ_series = new QPieSeries();
    m_occ_series->append("TensorFlow", 0.4)->setColor(QColor(0, 200, 200));
    m_occ_series->append("Zephyr", 0.1)->setColor(QColor(200, 150, 0));
    m_occ_series->append("Available", 0.7)->setColor(QColor(0, 0, 0));
    m_occ_series->setLabelsVisible();

    m_occ_chart = new QChart();
    m_occ_chart->addSeries(m_occ_series);
    m_occ_chart->setTitle("Occupancy Ratio");
    m_occ_chart->legend()->hide();
    m_occ_chart->setAnimationOptions(QChart::AllAnimations);
    m_occ_chart->setDropShadowEnabled(true);
    m_occ_chart->setAutoFillBackground(true);
    m_occ_chart->setTheme(QChart::ChartThemeBlueCerulean);


    ui->graphicsView_3->setChart(m_occ_chart);
    ui->graphicsView_3->setRenderHint(QPainter::Antialiasing);

    //ram
    m_bar_ai_device = new QBarSet("Zephyr");
    m_bar_tensorflow = new QBarSet("TensorFlow");
    *m_bar_ai_device << 13;
    *m_bar_tensorflow << 21;
    m_bar_tensorflow->setLabelColor(QColor(0,0,255));
    m_bar_tensorflow->setLabelColor(QColor(0,0,255));

    m_bar_series = new QBarSeries();
    m_bar_series->append(m_bar_ai_device);
    m_bar_series->append(m_bar_tensorflow);
    m_bar_series->setVisible(true);
    m_bar_series->setLabelsVisible(true);

    m_bar_chart = new QChart();
    m_bar_chart->setTheme(QChart::ChartThemeBlueCerulean);
    m_bar_chart->setDropShadowEnabled(true);
    m_bar_chart->addSeries(m_bar_series);
    m_bar_chart->setAutoFillBackground(true);
    m_bar_chart->setAnimationOptions(QChart::AllAnimations);

    m_bar_chart->setTitleBrush(QBrush(QColor(0,0,255)));
    m_bar_chart->setTitleFont(QFont("微软雅黑"));
    m_bar_chart->setTitle("Ram");

    m_bar_axisX = new QBarCategoryAxis;
    m_bar_axisX->append("Ⅰ");
    m_bar_axisX->append("Ⅱ");
    m_bar_axisX->append("Ⅲ");
    m_bar_axisX->append("Ⅳ");
    m_bar_axisX->append("Ⅴ");
    m_bar_axisX->setLabelsColor(QColor(7,28,96));

    m_bar_axisY = new QValueAxis;
    m_bar_axisY->setRange(0,100);
    m_bar_axisY->setTitleText("Consume");
    m_bar_axisY->setLabelFormat("%d%");

    m_bar_chart->setAxisX(m_bar_axisX,m_bar_series);
    m_bar_chart->setAxisY(m_bar_axisY,m_bar_series);
    m_bar_chart->legend()->setVisible(true);
    m_bar_chart->legend()->setAlignment(Qt::AlignBottom);
    m_bar_chart->legend()->setBackgroundVisible(true);
    m_bar_chart->legend()->setAutoFillBackground(true);
    m_bar_chart->legend()->setColor(QColor(222,233,251));
    m_bar_chart->legend()->setLabelColor(QColor(0,100,255));
    m_bar_chart->legend()->setMaximumHeight(50);

    ui->graphicsView_2->setChart(m_bar_chart);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    m_run_state_chart->setTheme(QChart::ChartThemeLight);
    m_occ_chart->setTheme(QChart::ChartThemeLight);
    m_bar_chart->setTheme(QChart::ChartThemeLight);

}
void MainWindow::init_agr(){
    ui->agr->addItem("V1");
}
void MainWindow::init_serial_port(){
    ui->serial_port_comboBox->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){

        ui->serial_port_comboBox->addItem(info.portName());
    }

    QObject::connect(&m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);

}
unsigned char* MainWindow::agr_start(void* handel,void* data,size_t hand_len,size_t data_len){
    if(handel == NULL || data == NULL || hand_len <= 0 || data_len <= 0){
        return NULL;
    }

    size_t len = hand_len + data_len;

    unsigned char* pack = (unsigned char*)malloc(len);
    if(pack == NULL){
        return NULL;
    }

    memset((unsigned char*)pack,0,len);

    memcpy((unsigned char*)pack,handel,hand_len);
    memcpy((unsigned char*)pack+hand_len,data,data_len);

    return pack;
}
void MainWindow::agr_end(unsigned char* pack){
        if(pack == NULL){
            return;
        }
        free(pack);
        pack = NULL;
}

void MainWindow::agr_hello(){
    hello = false;

    send_sign(HELLO);

    wait_start(1000);



}

void MainWindow::send_sign(int sign){

    AGR_HAND_V1 hand = {0};
    hand.sign = sign;
    hand.data_size = sizeof(AGR_HELLO_V1);
    AGR_HELLO_V1 data = {0};

    size_t len = sizeof(struct AGR_HAND_V1)+sizeof(struct AGR_HELLO_V1);
    unsigned char* pack = agr_start((void*)&hand,(void*)&data,sizeof(struct AGR_HAND_V1),sizeof(struct AGR_HELLO_V1));
    QByteArray send_buff = QByteArray::fromRawData((char *)pack,len);;
    m_serial.write(send_buff);
    agr_end(pack);

}

void MainWindow::wait_end(){
    m_timeout->stop();
    m_wait.close();
    ui->work->setMaximum(100);
    ui->work->setValue(100);

}
void MainWindow::wait_start(unsigned int timeout){
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    m_wait.show();
    m_wait.move(globalPos.x() + 200, globalPos.y() + 100);
    ui->work->setValue(0);
    //time out
    m_timeout->start(timeout);
    //dy
    // y = x/k
    dy(timeout/100,100);
 }

void MainWindow::serial_open_ok(){
    ui->conect_port->setText("Close");
    ui->image_seting->setEnabled(true);
    ui->serial_port_comboBox->setEnabled(false);
    ui->serial_port_baud_rete->setEnabled(false);
    this->log("Open serial port successfully");
    hello = true;
    //series
    init_series();
    ui->skin->setEnabled(true);

}

void MainWindow::handleTimeout(){
    if(hello == true){
        wait_end();
    }else{

        serial_oepn_err();
        wait_end();
    }

    if(m_set_model_and_config == true && (m_model_attr == false || m_model == false || m_reset == false)){

        this->log("Set Config and Model Failed");
        wait_end();
    }

    if(m_image == true){
        if(m_image_isok == false){
            this->log("Upload Image Failed");
            wait_end();
        }else{
            this->log("Upload Image Success");
            wait_end();
        }
    }


}

void MainWindow::DyTime(){

    ui->work->setValue(ui->work->value()+1);
}

void MainWindow::dy(int timeout,int courr){

    ui->work->setMaximum(courr);
    m_dy_time->start(timeout);

}

void MainWindow::serial_oepn_err(){
    this->log("This is not a recognizable AI device");
    m_serial.close();
    hello = false;
}
void MainWindow::readData(){

    m_buffer += m_serial.readAll();

    if(m_buffer[m_buffer.length()-1] == '\n' || m_buffer[m_buffer.length()-1] == '\r'){
    qDebug() << m_buffer;
        if(hello == false){

            QString str = QString(m_buffer);
            if(str.contains(RC_HELLO,Qt::CaseSensitive)){
                        serial_open_ok();
                        m_buffer.clear();
                        wait_end();

             }

        }

        if(m_reset == true){
            QString str = QString(m_buffer);
            if(str.contains(RC_RESET,Qt::CaseSensitive)){
                this->log("Set Memory Success");
                m_set_model_and_config = true;
                send_file(ui->model_path->text(),SET_MODEL);
            }

        }

        if(m_set_model_and_config == true){

            QString str = QString(m_buffer);
            if(str.contains(RC_SET_MODEL,Qt::CaseSensitive)){

                this->log("Set Model Success");
                m_model = true;
                send_file(ui->config_txt->text(),SET_MODEL_ATTR);
                this->log("Set Model Attr");


             }

            if(str.contains(RC_SET_MODEL_ATTR,Qt::CaseSensitive)){

                m_model_attr = true;
            }

            if(m_model_attr == true && m_model == true && m_reset == true){

                this->log("Set Config and Model success");
                m_set_model_and_config = false;
                m_reset                = false;
                m_buffer.clear();
                wait_end();
            }

        }

        if(m_image == true){
            QString str = QString(m_buffer);
            if(str.contains(RC_SCORE,Qt::CaseSensitive)){
                m_image_isok = true;
                QImage result = m_img->scaled(800, 600).scaled(200, 180, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                ui->re_image->setPixmap(QPixmap::fromImage(result));
                std::string picture_name = str.toStdString();
                int nPos_1 = picture_name.find("<");
                int nPos_2 = picture_name.find_last_of(">");
                std::string name_name=picture_name.substr(nPos_1+1,nPos_2-nPos_1-1);
                unsigned int score = atoi(name_name.data());
                if(score > 100){ score = 100; }
                nPos_1 = picture_name.find("(");
                nPos_2 = picture_name.find_last_of(")");
                name_name=picture_name.substr(nPos_1+1,nPos_2-nPos_1-1);
                ui->re_ec->setText(QString(tr("%1: %2%").arg(name_name.data()).arg(score)));
                wait_end();
            }

        }

        this->ai_log(QString(m_buffer));
        m_buffer = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::log(QString info){
     QDateTime cdt = QDateTime::currentDateTime();
     ui->log->append("[" + cdt.toString() + "] " + info);
}
void MainWindow::ai_log(QString str){
    QDateTime cdt = QDateTime::currentDateTime();
    ui->ai_log->append("[" + cdt.toString() + "] " + str);
}
void MainWindow::on_conect_port_clicked()
{
    if(ui->conect_port->text() == "Connect"){
        m_serial.setPortName(ui->serial_port_comboBox->currentText());
        m_serial.setBaudRate(ui->serial_port_baud_rete->text().toInt());
        m_serial.setDataBits(QSerialPort::Data8);
        m_serial.setParity(QSerialPort::NoParity);
        m_serial.setStopBits(QSerialPort::OneStop);
        m_serial.setFlowControl(QSerialPort::NoFlowControl);

        if(!m_serial.open(QIODevice::ReadWrite)){
            this->log("Failed to open serial port");
            ui->image_seting->setEnabled(false);
            ui->serial_port_comboBox->setEnabled(true);
            ui->serial_port_baud_rete->setEnabled(false);
            return;
        }else{
            agr_hello();
        }
    }else if (ui->conect_port->text() == "Close"){
            m_serial.close();
            this->log("Close serial port");
            ui->image_seting->setEnabled(false);
            ui->serial_port_comboBox->setEnabled(true);
            ui->serial_port_baud_rete->setEnabled(true);
            ui->conect_port->setText("Connect");
            ui->skin->setEnabled(false);
    }
}

QString MainWindow::get_file_path(QString format){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    HOME_DIR,
                                                    tr(format.toStdString().c_str()));
    return fileName;
}
void MainWindow::on_open_image_clicked()
{
    QString fileName = get_file_path("Images (*.png *.jpg *.jpeg)");
    ui->image_path->setText(fileName);
    if(!fileName.isEmpty())
    {

        if(! ( m_img->load(fileName) ) )
        {
             this->log("This is not a recognizable image format");
             return;
        }
        QImage result = m_img->scaled(800, 600).scaled(200, 180, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->image->setPixmap(QPixmap::fromImage(result));
        QFileInfo info(fileName);
        m_ing_info = info;
        if (info.exists()){
            ui->img_name->setText(info.fileName());
            ui->img_size->setText(tr("%1 Kb").arg(info.size() >> 10));
            ui->img_file->setText(info.fileName());
        }
        this->log("Loading image succeeded");
    }
}

void MainWindow::on_open_model_clicked()
{
    QString fileName = get_file_path("File (*.oneAPI)");
    ui->model_path->setText(fileName);
    QFileInfo info(fileName);
    ui->model_name->setText(info.fileName());
    ui->model_size->setText(tr("%1 Kb").arg(info.size()>>10));

}

void MainWindow::on_checkBox_clicked()
{

}

void MainWindow::on_img_upload_clicked()
{
    if(ui->image_path->text() == NULL){
        this->log("Image file is NULL");
        return;
    }

    //start
    wait_start(10000);
    clear_tmp();
    img2yuv_96x96(IMG2PATH(m_ing_info),SEND_YUV_TMP_FILE);

    /*
    //read file and pack
    QByteArray file = read_file(SEND_YUV_TMP_FILE);

    //pack file
    AGR_HAND_V1 agr;
    agr.sign = IMAGE;
    agr.data_size = file.size();
    QByteArray send_buff = pack_file(agr,file);

    //send file
    send(send_buff);
    */
    m_image = true;
    m_image_isok = false;
    send_file(SEND_YUV_TMP_FILE,FORECAST);


}
void MainWindow::send(QByteArray& arry){
        verification = true;
        m_serial.write(arry);
}

QByteArray MainWindow::pack_file(AGR_HAND_V1 arg1,QByteArray& file){

    AGR_HAND_V1 m_temp = arg1;
    unsigned char* data_buff = agr_start((void*)&m_temp,(void*)file.data(),sizeof(struct AGR_HAND_V1),file.size());
    QByteArray send_buff = QByteArray::fromRawData((char *)data_buff,sizeof(struct AGR_HAND_V1)+file.size());
    agr_end(data_buff);
    return send_buff;

}
void MainWindow::on_pushButton_clicked()
{
    if(ui->config_txt->text() == NULL || ui->model_path->text() == NULL){
        this->log("Config file and Model file is NULL");
        return;
    }

    /*
     AGR_HAND_V1 agr;
     QByteArray file;
     QByteArray send_buff;

     m_model_attr = false;
     m_model = false;
     ui->label_3->setText("set config and model");
     //set timeout
     m_set_model_and_config = true;
     wait_start(45000);

     //send model
     file = read_file(ui->model_path->text());
     agr.sign = SET_MODEL;
     agr.data_size = file.size();
     send_buff = pack_file(agr,file);
     send(send_buff);
     */
    m_model_attr = false;
    m_model = false;
    m_reset = true;
    ui->label_3->setText("set config and model");
    wait_start(45000);
    send_sign(RESET_MEMORY);


}

void MainWindow::on_up_clicked()
{
    init_serial_port();
}


void MainWindow::clear_tmp(){
        QDir dir(TMP_DIR);
        dir.setFilter(QDir::Files);
        int fileCount = dir.count();
        for (int i = 0; i < fileCount; i++)
            dir.remove(dir[i]);
}

void MainWindow::ffmpeg(QString src_file,QString arg,QString dest_file){
    QString arg_str(tr("%1 -i %2 %3 %4").arg(FFMPEG_PATH).arg(src_file).arg(arg).arg(dest_file));
    cmd(arg_str);
}

void MainWindow::cmd(QString arg){
    system(arg.toStdString().c_str());
}

void MainWindow::img2yuv_96x96(QString src_file,QString dest_file){
    ffmpeg(src_file,FFMPEG_YUV_96x96,dest_file);
}

QByteArray MainWindow::read_file(QString file_path){

    QFile file(file_path);
    file.open(QIODevice::ReadOnly);
    QByteArray arry;
    while(!file.atEnd()){
        arry.append(file.readLine(READ_SIZE));
    }
    return arry;

}

void MainWindow::send_file(QString file_path,int arg){
    QFile file(file_path);
    AGR_HAND_V1 hand = {0};
    hand.sign = arg;
    int dataLen = 0;
    QByteArray binByteArray;
    qDebug() << file_path;
        if(!file.open(QIODevice::ReadOnly)) {
            qDebug()<<"Can't open the file!"<<endl;
        }

        while(!file.atEnd())
          {
              //QByteArray buf = file.readLine(512);
              binByteArray.append(file.readLine(512));
          }
        dataLen = file.size();

    hand.data_size = dataLen;
    int send_len = sizeof(AGR_HAND_V1)+dataLen;
    char* data = (char*)malloc(send_len);
    memset(data,0,send_len);
    memcpy(data,&hand,sizeof(AGR_HAND_V1));
    memcpy(data+sizeof(AGR_HAND_V1),binByteArray.data(),dataLen);
    QByteArray send_buff = QByteArray::fromRawData((char *)data,send_len);
    m_serial.write(send_buff);
    free(data);


}
void MainWindow::on_set_config_clicked()
{
    QString fileName = get_file_path("File (*.conf)");
    ui->config_txt->setText(fileName);


}

void MainWindow::on_skin_clicked()
{
    if(ui->skin->isChecked()){
        set_qss(m_this_path+m_skin[1]);
        m_run_state_chart->setTheme(QChart::ChartThemeBlueCerulean);
        m_occ_chart->setTheme(QChart::ChartThemeBlueCerulean);
        m_bar_chart->setTheme(QChart::ChartThemeBlueCerulean);
    }else{
        set_qss(m_this_path+m_skin[0]);
        m_run_state_chart->setTheme(QChart::ChartThemeLight);
        m_occ_chart->setTheme(QChart::ChartThemeLight);
        m_bar_chart->setTheme(QChart::ChartThemeLight);
    }

}
