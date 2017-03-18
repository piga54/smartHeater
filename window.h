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
public slots:
      void setLimit(double limit);

// internal variables for the window class
private slots:
        void startProcess1();
        void startProcess2();
	void startProcess3();
        void emergencyOff();
//        void setLimit();


private:
        void createButtonsGroup();
        void createControlHeater();
        void createMonitoringHeater();
        QwtKnob      *knob;
        QwtThermo    *thermo;
        QwtPlot      *plot;
        QwtPlotCurve *curve;
        QLabel       *reading;
        QLabel       *manual;
        QPushButton  *heaterLed;
        QPushButton  *heaterOff;

        //Qt Groups
        QGroupBox *buttonsGroup;
        QGroupBox *heaterGroup;
        QGroupBox *monitoringGroup;
        // layout elements
//        QVBoxLayout  *vLayout1;  // vertical layout1
  //      QVBoxLayout  *vLayout2;  // vertical layout2
    //    QHBoxLayout  *hLayout;  // horizontal layout
        QHBoxLayout  *mainLayout;  // horizontal layout

        static const int plotDataSize = 100;

        // data arrays for the plot
        double xData[plotDataSize];
        double yData[plotDataSize];

	double b;
        bool offHeaterControl;
        double limit;

};

#endif // WINDOW_H
