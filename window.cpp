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

Window::Window():b(0.0), offHeaterControl(0), limit(0.0)
{
        createButtonsA();
        createButtonsB();
        createButtonsGroup();
        createManualControlGroup();
        createMonitoringGroup();
        createPanelGroup();

        // set up the initial plot data
        for( int index=0; index<plotDataSize; ++index )
        {
                xData[index] = index;
                yData[index] = 20;
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
        plot->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8("temperature C"));

        //set up the main layout
        mainLayout = new QHBoxLayout;
        mainLayout->addWidget(panelGroup);
        mainLayout->addWidget(plot);
        setLayout(mainLayout);
}

void Window::createButtonsA()
{
        buttonsA =  new QGroupBox();
        QVBoxLayout *layout = new QVBoxLayout;
        QPushButton *ButtonOff = new QPushButton(tr("OFF"));
        QPushButton *Button1 = new QPushButton(tr("20\260C"));

        layout->addWidget(ButtonOff);
        layout->addWidget(Button1);

        buttonsA->setLayout(layout);

        connect(ButtonOff,SIGNAL(clicked()),SLOT(emergencyOff()));
        connect(Button1,SIGNAL(clicked()),SLOT(startProcess1()));
}

void Window::createButtonsB()
{
        buttonsB = new QGroupBox();
        QVBoxLayout *layout = new QVBoxLayout;
        QPushButton *Button2 = new QPushButton(tr("40\260C"));
        QPushButton *Button3 = new QPushButton(tr("60\260C"));

        layout->addWidget(Button2);
        layout->addWidget(Button3);

        buttonsB->setLayout(layout);

        connect(Button2,SIGNAL(clicked()),SLOT(startProcess2()));
        connect(Button3,SIGNAL(clicked()),SLOT(startProcess3()));
}

void Window::createButtonsGroup()
{
        buttonsGroup = new QGroupBox(tr("Automatic Control"));
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(buttonsA);
        layout->addWidget(buttonsB);

        buttonsGroup->setLayout(layout);
}

void Window::createManualControlGroup()
{
        manualControlGroup = new QGroupBox(tr("Manual Control"));
        QVBoxLayout *layout = new QVBoxLayout;

        knob = new QwtKnob;
        knob->setRange(0,80);
        knob->setValue(limit);
//        knob->setKnobWidth(5);
        layout->addWidget(knob);

        manual = new QLabel;
        manual->setStyleSheet("background-color: rgb(255,255,255)");
        layout->addWidget(manual);
        manualControlGroup->setLayout(layout);

        connect(knob, SIGNAL(valueChanged(double)), SLOT(setLimit(double)));
}

void Window::createMonitoringGroup()
{
        monitoringGroup = new QGroupBox(tr("Monitor"));
        QVBoxLayout *layout = new QVBoxLayout;
        reading = new QLabel;
        reading->setStyleSheet("background-color: rgb(255,255,255)");
//        layout->addWidget(reading);

        heaterLed = new QPushButton(tr("OFF"));
        heaterLed->setEnabled(false);
        heaterLed->setStyleSheet("background-color: rgb(100,0,0)");
  //      layout->addWidget(heaterLed);

        thermo = new QwtThermo;
        thermo->setFillBrush( QBrush(Qt::red) );
        thermo->setRange(0, 100);
        thermo->show();
        layout->addWidget(thermo);

        monitoringGroup->setLayout(layout);
}

void Window::createPanelGroup()
{
        panelGroup = new QGroupBox(tr("Panel"));
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(buttonsGroup);
        layout->addWidget(manualControlGroup);
        layout->addWidget(monitoringGroup);

        panelGroup->setLayout(layout);
}

Window::~Window() {
//
}


void Window::timerEvent( QTimerEvent * )
{
        double a;
        double inVal =test(&a);

        QString s = QString::number(inVal);
        s = s + "\260C";
        QString h = QString::number(limit);
        h = h + "\260C";
        manual->setText(h);
        wiringPiSetup () ;
        pinMode (1, OUTPUT) ;
        // add the new input to the plot
        memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
        yData[plotDataSize-1] = inVal;
        reading->setText(s);
        curve->setSamples(xData, yData, plotDataSize);
        plot->replot();
        reading->show();
        thermo->setValue(inVal);
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

}

void Window::startProcess1()
{
this->b=b;
b=20.0;
limit=0;
}

void Window::startProcess2()
{
this->b=b;
b=40.0;
limit=0;
}

void Window::startProcess3()
{
this->b=b;
b=60.0;
limit=0;
}


void Window::emergencyOff()
{
this->offHeaterControl=offHeaterControl;
offHeaterControl=1;
b=0.0;
limit=b;
}

void Window::setLimit(double limit)
{
this->limit=limit;
b=limit;
}
