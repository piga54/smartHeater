#ifndef WINDOW_H
#define WINDOW_H

//#include <qwt/qwt_thermo.h>
//#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#include <QBoxLayout>
//#include <QTextEdit>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <qwidget.h>
#include <qfont.h>

// #include "adcreader.h"

// class definition 'Window'
class Window : public QWidget
{
        // must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
        Q_OBJECT

public:
        Window(); // default constructor - called when a Window is declared without arguments

        ~Window();

        void timerEvent( QTimerEvent * );
//public slots:
//      void setGain(double gain);

// internal variables for the window class
private slots:
        void startProcess1();
        void startProcess2();
	void startProcess3();
        void emergencyOff();


private:
        void createButtonsGroup();
        void createControlHeater();
        void createMonitoringHeater();
//      QwtKnob      *knob;
//      QwtThermo    *thermo;
        QwtPlot      *plot;
        QwtPlotCurve *curve;
        QLineEdit    *reading;
        QPushButton  *heaterLed;
        QPushButton  *heaterOff;

        //Qt Groups
        QGroupBox *buttonsGroup;
        QGroupBox *heaterGroup;
        QGroupBox *monitoringGroup;
        // layout elements
        QVBoxLayout  *vLayout1;  // vertical layout1
        QVBoxLayout  *vLayout2;  // vertical layout2
        QHBoxLayout  *hLayout;  // horizontal layout
        QHBoxLayout  *mainLayout;  // horizontal layout
//        QHBoxLayout  *controlHeater  //horizontal layout
        static const int plotDataSize = 100;

        // data arrays for the plot
        double xData[plotDataSize];
        double yData[plotDataSize];

//      double gain;
//      int count;
	double b;
        bool offHeaterControl;

//      ADCreader *adcreader;
};

#endif // WINDOW_H
