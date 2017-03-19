#include "window.h"
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

//test function gets the data from the sensor
double test (double *a)
{
double temp;            //Here is stored the temperature as a double
int    openFile;	//It is needed to open the sensor
int    reader;
char   buffer[BUFSIZE];
char   stringTemp[5];   //Temperature is stored in a String

openFile = open("/sys/bus/w1/devices/28-001522141bee/w1_slave", O_RDONLY);
if(-1 == openFile){
perror("open device file error");
return 1;
}

//Infinite loop which breaks after reading the sensor or failing in accessing it
while(1){
reader = read(openFile, buffer, BUFSIZE);
if(0 == reader){
break;
}
if(-1 == reader){
if(errno == EINTR){
continue;
}
perror("read()");
close(openFile);
return 1;
}
}

//Storing the temperature in a String
for(std::size_t i=0;i<sizeof(buffer);i++){
if(buffer[i] == 116){ 
for(std::size_t j=0;j<sizeof(stringTemp);j++){
stringTemp[j] = buffer[i+2+j];
}
}
}


temp = (double)atoi(stringTemp) / 1000; //Conversion from char to double
*a = temp;                              //Pointer for using temperature in other function
double result = *a;
return result;
}

Window::Window():b(0.0), c(0.0), offHeaterControl(0), limit(0.0)
{
	//These functions creates all the GUI elements except the main Layout
        createAutomaticControlGroup();
        createManualControlGroup();
        createMonitoringGroup();
        createControlHeater();
        createPanelGroup();

        //Set up the initial plot data
        for( int index=0; index<plotDataSize; ++index )
        {
                xData[index] = index;
                yData[index] = 20;
        }

        curve = new QwtPlotCurve;
        plot = new QwtPlot;
        //Make a plot curve from the data and attach it to the plot
        curve->setSamples(xData, yData, plotDataSize);
        curve->attach(plot);

        plot->replot();
        plot->show();
	plot->setStyleSheet("QWidget {border-image: url(./pic/water_background_by_baggs.png) }");

        plot->setAxisTitle(QwtPlot::xBottom,QString::fromUtf8("time"));
        plot->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8("temperature C"));

        //Set up the main layout
        mainLayout = new QHBoxLayout;	     //horizontal layout
	mainLayout->addWidget(plot);         //set up the plot as first slot
        mainLayout->addWidget(panelGroup);   //set up the panelGroup as second slot
        setLayout(mainLayout);
}

void Window::createAutomaticControlGroup()
{
        automaticControlGroup = new QGroupBox(tr("Auto Control"));
	//Group background
        automaticControlGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");

        QVBoxLayout *layout = new QVBoxLayout;
        QPushButton *ButtonOff = new QPushButton(tr("TURN OFF"));
        QPushButton *Button1 = new QPushButton(tr("20\260C"));
        QPushButton *Button2 = new QPushButton(tr("40\260C"));
        QPushButton *Button3 = new QPushButton(tr("60\260C"));

	//Adding components to the layout
        layout->addWidget(ButtonOff);
        layout->addWidget(Button1);
        layout->addWidget(Button2);
        layout->addWidget(Button3);
        automaticControlGroup->setLayout(layout);

	//Functionality of Buttons
        connect(ButtonOff,SIGNAL(clicked()),SLOT(emergencyOff()));
        connect(Button1,SIGNAL(clicked()),SLOT(startProcess1()));
        connect(Button2,SIGNAL(clicked()),SLOT(startProcess2()));
        connect(Button3,SIGNAL(clicked()),SLOT(startProcess3()));

	//Buttons Design
	Button1->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	Button2->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	Button3->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
	ButtonOff->setStyleSheet("QWidget {border-image: url(./pic/1322493767359492846blue-button-no-text-hi-hi.png) }");
}

void Window::createManualControlGroup()
{
        manualControlGroup = new QGroupBox(tr("Manual Control"));
        QVBoxLayout *layout = new QVBoxLayout;
	//Group Layout
	manualControlGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");

	//Knob creation
        knob = new QwtKnob;
        knob->setRange(0,80);		//Knob range.
        knob->setKnobWidth(30);		//For reducing knob size
        knob->setValue(limit);		//Set up the knob value for the initialization
        layout->addWidget(knob);	//Add the knob to the layout

	//Knob label creation. THe user can see the real knob's value
        manual = new QLabel;
        layout->addWidget(manual);
        manualControlGroup->setLayout(layout);

	//Knob functionality. setLimit function is called whenever the knob is manipulated
        connect(knob, SIGNAL(valueChanged(double)), SLOT(setLimit(double)));
}


void Window::createMonitoringGroup()
{
        monitoringGroup = new QGroupBox(tr("Monitor"));
	//Group background
        monitoringGroup->setStyleSheet("QGroupBox {border-image: url(./pic/white-blue.jpg)} ");
        QVBoxLayout *layout = new QVBoxLayout;

	//To have a small space between labels
	space =new QLabel;
	layout->addWidget(space);

	//heaterLed is a Push Button that is used as a LED indicator whether the heater is ON or OFF
        heaterLed = new QPushButton(tr("OFF"));
        heaterLed->setEnabled(false);
        heaterLed->setStyleSheet("background-color: rgb(100,0,0)");
        layout->addWidget(heaterLed);

	//This label takes the temperature's value and show it in the GUI
        reading = new QLabel;
        layout->addWidget(reading);

	//A thermometer is implemented in the monitoringGroup
        thermo = new QwtThermo;
        thermo->setFillBrush( QBrush(Qt::red) );
        thermo->setRange(0, 100);	//Thermometer's range
        thermo->show();
        layout->addWidget(thermo);

        monitoringGroup->setLayout(layout);
}

void Window::createControlHeater()
{
        controlHeater = new QGroupBox;
        QVBoxLayout *layout = new QVBoxLayout;		//Vertical Box Layout
        layout->addWidget(automaticControlGroup);	//First slot of the Box
        layout->addWidget(manualControlGroup);		//Second slot of the Box
        controlHeater->setLayout(layout);
}

void Window::createPanelGroup()
{
       panelGroup = new QGroupBox(tr("Smart Water Heater"));
       QHBoxLayout *layout = new QHBoxLayout;		//Horizontal Box Layout
       layout->addWidget(controlHeater);		//First slot of the Box
       layout->addWidget(monitoringGroup);		//Second slot of the Box
       panelGroup->setLayout(layout);
}

void Window::timerEvent( QTimerEvent * )
{
        double a;
        double inVal =test(&a);			//inVal takes the temperature's value from the test function

        QString s = QString::number(inVal);	//Conversion from double to QString. Current temperature label
        s = s + "\260C";
        QString h = QString::number(limit);	//Conversion from double to QString. Knob label
        h = h + "\260C";
        manual->setText(h);			//Display knob's value in the GUI

	//Set up the GPIO for Heater Control
        wiringPiSetup () ;			//Set up WiringPI library
        pinMode (1, OUTPUT) ;			//Set up GPIO 18 as OUTPUT

        // add the new input to the plot
        memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
        yData[plotDataSize-1] = inVal;

        reading->setText(s);			//Display current temperature
        curve->setSamples(xData, yData, plotDataSize);
        plot->replot();
	thermo->setValue(inVal);		//Display current temperature in thermometer
	printf("%.3f C\n",inVal);		//Print current temperature in terminal

//Turn ON Heater according the current limit -5
if (inVal<b-5){
digitalWrite(1,1);			                     //Turn on the Heater
heaterLed->setStyleSheet("background-color: rgb(255,0,0)");  //Turn on heaterLed
heaterLed->setText("ON");
}

//Turn OFF the Heater if the temperature is greater than current limit -5 or
//Temperature is greater than difference
if((inVal>=b-5) || (inVal>=c)){
digitalWrite(1,0);                              //Turn off the Heater
heaterLed->setStyleSheet("background-color: rgb(100,0,0)");  //Turn off heaterL$
heaterLed->setText("OFF");
offHeaterControl = 0;                           //Reset the control signal
c=b;						//difference is set equal to  dont affect the turning off behaviour
}

}

//Button1 function
void Window::startProcess1()
{
this->b=b;
//Set temperature's limit (Button1)
b=20.0;
limit=0;
//This value will be a temperature's limit depending on how far is the current
//Temperature from the desire temperature
c=20;

//Set point for taking current temperature's value
double a;
double smartL =test(&a);
double rest;

//Cases for temeprature's limit
rest = b-smartL;
if (rest<=5){
c=b-2.5;
}
else if(rest>5 && rest<=10){
c=b-7.5;
}
else{
c=b-12.5;
}
}

//Button2 function
void Window::startProcess2()
{
this->b=b;
b=40.0;
limit=0;
c=40;
double a;
double smartL =test(&a);                 //inVal takes the temperature's$
double rest;

rest = b-smartL;
if (rest<=5){
c=b-2.5;
}
else if(rest>5 && rest<=10){
c=b-7.5;
}
else{
c=b-12.5;
}

}

//Button3 function
void Window::startProcess3()
{
this->b=b;
b=60.0;
limit=0;
c=60;

double a;
double smartL =test(&a);                 //inVal takes the temperature's$
double rest;

rest = b-smartL;
if (rest<=5){
c=b-2.5;
}
else if(rest>5 && rest<=10){
c=b-7.5;
}
else{
c=b-12.5;
}

}

//This function is called for turning off the Heater no matter what
void Window::emergencyOff()
{
this->offHeaterControl=offHeaterControl;
offHeaterControl=1;	//The control signal is activated
b=0.0;
limit=b;
c=limit;
knob->setValue(0);	//Knob is reset
}

//Function for setting a new limit according to the knob
void Window::setLimit(double limit)
{
this->limit=limit;
b=limit-5;

}
