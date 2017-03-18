#include "window.h"
#include <cmath>
#include <QtGui>
#include <QFont>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#define BUFSIZE 128
#define PIN RPI_GPIO_P1_12
double test (double *a)
{
double temp;
int fd;
int ret;
char buf[BUFSIZE];
char tempBuf[5];

fd = open("/sys/bus/w1/devices/28-001522141bee/w1_slave", O_RDONLY);
if(-1 == fd){
perror("open device file error");
return 1;
}

while(1){
ret = read(fd, buf, BUFSIZE);
if(0 == ret){
break;
}
if(-1 == ret){
if(errno == EINTR){
continue;
}
perror("read()");
close(fd);
return 1;
}
}

for(std::size_t i=0;i<sizeof(buf);i++){
if(buf[i] == 116){ //instead of 116 it was 't'
for(std::size_t j=0;j<sizeof(tempBuf);j++){
tempBuf[j] = buf[i+2+j];
}
}
}

temp = (double)atoi(tempBuf) / 1000;
*a = temp;
double result = *a;
return result;
}

//Window::Window() : gain(5), count(0)
Window::Window():b(0.0), offHeaterControl(0)
{
        createButtonsGroup();
        createMonitoringHeater();
        createControlHeater();
//        reading  = new QTextEdit;
//        reading ->setText("Tempe C");


        // set up the initial plot data
        for( int index=0; index<plotDataSize; ++index )
        {
                xData[index] = index;
                yData[index] = 10;
        }

        curve = new QwtPlotCurve;
        plot = new QwtPlot;
        // make a plot curve from the data and attach it to the plot
        curve->setSamples(xData, yData, plotDataSize);
        curve->attach(plot);

        plot->replot();
        plot->show();

        plot->setTitle(QString::fromUtf8("Smart Water Heater"));
        plot->setAxisTitle(QwtPlot::xBottom,QString::fromUtf8("time"));
        plot->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8("temperature"));

        // set up the layout - knob above thermometer
       /* vLayout1 = new QVBoxLayout;
        vLayout1->addWidget(Button1);
        vLayout1->addWidget(Button2);
        vLayout1->addWidget(Button3);
        vLayout1->addWidget(reading);
        vLayout1->addWidget(quitButton);
*/
        //set up the main layout
        mainLayout = new QHBoxLayout;
        //mainLayout->addLayout(vLayout1);
        mainLayout->addWidget(heaterGroup);
        mainLayout->addWidget(plot);
        setLayout(mainLayout);


        // plot to the left of knob and thermometer

       // setLayout(hLayout);
}
void Window::createButtonsGroup()
{
        buttonsGroup = new QGroupBox(tr("Buttons"));
        QVBoxLayout *layout = new QVBoxLayout;
        QPushButton *Button1 = new QPushButton(tr("20\260C"));
        QPushButton *Button2 = new QPushButton(tr("40\260C"));
        QPushButton *Button3 = new QPushButton(tr("60\260C"));

        layout->addWidget(Button1);
        layout->addWidget(Button2);
        layout->addWidget(Button3);
        buttonsGroup->setLayout(layout);

        connect(Button1,SIGNAL(clicked()),SLOT(startProcess1()));
        connect(Button2,SIGNAL(clicked()),SLOT(startProcess2()));
        connect(Button3,SIGNAL(clicked()),SLOT(startProcess3()));
}
void Window::createControlHeater()
{
       heaterGroup = new QGroupBox(tr("Heater Control"));
       QHBoxLayout *layout = new QHBoxLayout;
       layout->addWidget(buttonsGroup);
       layout->addWidget(monitoringGroup);
       heaterGroup->setLayout(layout);
}
void Window::createMonitoringHeater()
{
       monitoringGroup = new QGroupBox(tr("Heater Monitoring"));
       QVBoxLayout *layout = new QVBoxLayout;
       reading = new QLineEdit;
       layout->addWidget(reading);

       heaterLed = new QPushButton(tr("OFF"));
       heaterLed->setEnabled(false);
       heaterLed->setStyleSheet("background-color: rgb(100,0,0)");
       layout->addWidget(heaterLed);

       heaterOff = new QPushButton(tr("TURN OFF"));
       layout->addWidget(heaterOff);

       monitoringGroup->setLayout(layout);
       connect(heaterOff,SIGNAL(clicked()),SLOT(emergencyOff()));
}

Window::~Window() {
        // tells        connect(Button3,SIGNAL(clicked()),SLOT(startPro
//      adcreader->quit();
        // wait until the run method has terminated
//      adcreader->wait();
//      delete adcreader;
}


void Window::timerEvent( QTimerEvent * )
{
        double a;
        double inVal =test(&a);

        QString s = QString::number(inVal);
        wiringPiSetup () ;
        pinMode (1, OUTPUT) ;
        // add the new input to the plot
        memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
        yData[plotDataSize-1] = inVal;
        reading->setText(s);
        curve->setSamples(xData, yData, plotDataSize);
        plot->replot();
        reading->show();
	printf("%.3f C\n",inVal);

if (inVal<=b){
digitalWrite(1,1);
heaterLed->setStyleSheet("background-color: rgb(255,0,0)");
heaterLed->setText("ON");
}
else{
digitalWrite(1,0);
heaterLed->setStyleSheet("background-color: rgb(100,0,0)");
heaterLed->setText("OFF");
offHeaterControl = 0;
}


//	printf("%f \n", b );

        // set the thermometer value
//      thermo->setValue( inVal + 10 );
}


void Window::startProcess1()
{
this->b=b;
b=20.0;
}

void Window::startProcess2()
{
this->b=b;
b=40.0;
}

void Window::startProcess3()
{
this->b=b;
b=60.0;
}

void Window::emergencyOff()
{
this->offHeaterControl=offHeaterControl;
offHeaterControl=1;
b=0.0;
}

