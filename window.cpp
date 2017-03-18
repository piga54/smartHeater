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
Window::Window():b(0.0)
{

        QPushButton *Button1 = new QPushButton("&20 C°");
        QPushButton *Button2 = new QPushButton("&40 C°");
        QPushButton *Button3 = new QPushButton("&60 C°");
        QPushButton *quitButton = new QPushButton("&Quit");

        QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
        connect(Button1,SIGNAL(clicked()),SLOT(startProcess1()));
        connect(Button2,SIGNAL(clicked()),SLOT(startProcess2()));
	connect(Button3,SIGNAL(clicked()),SLOT(startProcess3()));
        // set up the initial plot data
        for( int index=0; index<plotDataSize; ++index )
        {
                xData[index] = index;
                //yData[index] = gain * sin( M_PI * index/50 );
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
        vLayout = new QVBoxLayout;

        vLayout->addWidget(Button1);
        vLayout->addWidget(Button2);
        vLayout->addWidget(Button3);
        vLayout->addWidget(quitButton);

        // plot to the left of knob and thermometer
        hLayout = new QHBoxLayout;
        hLayout->addLayout(vLayout);
        hLayout->addWidget(plot);

        setLayout(hLayout);
        // This is a demo for a thread which can be
        // used to read from the ADC asynchronously.
        // At the moment it doesn't do anything else than
        // running in an endless loop and which prints out "tick"
        // every second.
//      adcreader = new ADCreader();
//      adcreader->start();
}

Window::~Window() {
        // tells the thread to no longer run its endless loop
//      adcreader->quit();
        // wait until the run method has terminated
//      adcreader->wait();
//      delete adcreader;
}


void Window::timerEvent( QTimerEvent * )
{
        double a;
        double inVal =test(&a);

        wiringPiSetup () ;
        pinMode (1, OUTPUT) ;
        // add the new input to the plot
        memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
        yData[plotDataSize-1] = inVal;
        curve->setSamples(xData, yData, plotDataSize);
        plot->replot();

	printf("%.3f C\n",inVal);

if (inVal<=b){
digitalWrite(1,1);
}
else{
digitalWrite(1,0);
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

