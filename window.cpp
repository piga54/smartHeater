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
        createAutomaticControlGroup();
        createManualControlGroup();
        createMonitoringGroup();
        createControlHeater();
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
	plot->setStyleSheet("QWidget {border-image: url(./pic/water_background_by_baggs.png) }");

//        plot->setTitle(QString("<h2><i>Smart </i>"
//"<font color=red>Water Heater</font></h2>"));
        plot->setAxisTitle(QwtPlot::xBottom,QString::fromUtf8("time"));
        plot->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8("temperature C"));
//	plot->setAxisScale(QwtPlot::xBottom, 0, 60);

        //set up the main layout
        mainLayout = new QHBoxLayout;
	mainLayout->addWidget(plot);
        mainLayout->addWidget(panelGroup);
        setLayout(mainLayout);

	//set background
//	plot->setStyleSheet("QWidget {border-image: url(./pic/water_background_by_baggs.png) }");

}

void Window::createAutomaticControlGroup()
{
        automaticControlGroup = new QGroupBox(tr("Auto Control"));
        QVBoxLayout *layout = new QVBoxLayout;
        QPushButton *ButtonOff = new QPushButton(tr("TURN OFF"));
        QPushButton *Button1 = new QPushButton(tr("20\260C"));
        QPushButton *Button2 = new QPushButton(tr("40\260C"));
        QPushButton *Button3 = new QPushButton(tr("60\260C"));

        layout->addWidget(ButtonOff);
        layout->addWidget(Button1);
        layout->addWidget(Button2);
        layout->addWidget(Button3);
        automaticControlGroup->setLayout(layout);

        connect(ButtonOff,SIGNAL(clicked()),SLOT(emergencyOff()));
        connect(Button1,SIGNAL(clicked()),SLOT(startProcess1()));
        connect(Button2,SIGNAL(clicked()),SLOT(startProcess2()));
        connect(Button3,SIGNAL(clicked()),SLOT(startProcess3()));

	Button1->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	Button2->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	Button3->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	ButtonOff->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");

	automaticControlGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");

}

void Window::createManualControlGroup()
{
        manualControlGroup = new QGroupBox(tr("Manual Control"));
        QVBoxLayout *layout = new QVBoxLayout;
	manualControlGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");
        knob = new QwtKnob;
        knob->setRange(0,80);
        knob->setKnobWidth(30);
        //knob->setMarkerStyle(2);
        knob->setValue(limit);
        layout->addWidget(knob);

        manual = new QLabel;
       // manual->setStyleSheet("background-color: rgb(255,255,255)");
        layout->addWidget(manual);
        manualControlGroup->setLayout(layout);

        connect(knob, SIGNAL(valueChanged(double)), SLOT(setLimit(double)));

	//set background
//	manualControlGroup->setStyleSheet("border-image: url(./pic/13042073-Drop-of-water-logo-Stock-Vector.jpg) ");
}


void Window::createMonitoringGroup()
{
       monitoringGroup = new QGroupBox(tr("Monitor"));
       QVBoxLayout *layout = new QVBoxLayout;
//	monitoringGroup->setStyleSheet("QWidget {border-image: url(./pic/white-blue.jpg)} ");


	space =new QLabel;
	layout->addWidget(space);

       heaterLed = new QPushButton(tr("OFF"));
       heaterLed->setEnabled(false);
       heaterLed->setStyleSheet("background-color: rgb(100,0,0)");
       layout->addWidget(heaterLed);

       reading = new QLabel;
//       reading->setStyleSheet("background-color: rgb(255,255,255)");
       layout->addWidget(reading);


       thermo = new QwtThermo;
       thermo->setFillBrush( QBrush(Qt::red) );
       thermo->setRange(0, 100);
       thermo->show();
       layout->addWidget(thermo);

       monitoringGroup->setLayout(layout);

	monitoringGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");
}

void Window::createControlHeater()
{
        controlHeater = new QGroupBox;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(automaticControlGroup);
        layout->addWidget(manualControlGroup);
//	layout->addWidget(controlHeater);
        controlHeater->setLayout(layout);

}

void Window::createPanelGroup()
{
       panelGroup = new QGroupBox(tr("Smart Water Heater"));
       QHBoxLayout *layout = new QHBoxLayout;
       layout->addWidget(controlHeater);
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
knob->setValue(0);
}

void Window::setLimit(double limit)
{
this->limit=limit;
b=limit;
}
