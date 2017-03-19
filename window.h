#ifndef WINDOW_H
#define WINDOW_H

#include <qwt/qwt_thermo.h>
#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <QBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <qwidget.h>
#include <qfont.h>

class Window : public QWidget
{
        // must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
        Q_OBJECT

public:
        Window(); // default constructor - called when a Window is declared without arguments

        void timerEvent( QTimerEvent * );
public slots:
	void setLimit(double limit);  //This function allows to set a new limit by modifying the knob (Manual COntrol)

// internal variables for the window class
private slots:
        void startProcess1();  //Set the temperature limit to 20 C (Button1)
        void startProcess2();  //Set the temperature limit to 40 C (Button2)
	void startProcess3();  //Set the temperature limit to 60 C (Button3)
        void emergencyOff();   //This function turns off the Heater no matter what (ButtonOff)

private:
	//These functions are for creating all the components in the GUI. The components are divided by groups(Qt Groups)
        void createAutomaticControlGroup();  //In this function there are the buttons for an automatic control
        void createManualControlGroup();     //The knob with its label are created here.
        void createMonitoringGroup();        //The heater LED, the thermometer and its label are made in this function
        void createControlHeater();          //This function makes the automaticGroup and manualGroup inside of controlHeater
        void createPanelGroup();             //Thee panelGroup is created with this function and contains the ControlHeater
					     //and the plot.

	//Declaring pointers
        QwtKnob      *knob;
        QwtThermo    *thermo;
        QwtPlot      *plot;
        QwtPlotCurve *curve;
        QLabel       *reading;
	QLabel       *space;
        QLabel       *manual;
        QPushButton  *heaterLed;
        QPushButton  *heaterOff;

        //Qt Groups. Each group is created inside of its own function
        QGroupBox *automaticControlGroup;
        QGroupBox *manualControlGroup;
        QGroupBox *monitoringGroup;
        QGroupBox *controlHeater;
        QGroupBox *panelGroup;

        //The main Layout which will contain all the GUI elements
        QHBoxLayout  *mainLayout;  // horizontal layout

        static const int plotDataSize = 60;

        // data arrays for the plot
        double xData[plotDataSize];
        double yData[plotDataSize];

	double b,c;                 //Global variable for setting the temperature limit by buttons
        bool offHeaterControl;    //Global variable used as a flag for turning off the Heater
        double limit; 		  //Global variable for setting the temperature limit by the knob
	bool flag;
};

#endif // WINDOW_H
