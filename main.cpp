#include <window.h>

#include <QApplication>
#include <QtGui>
int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        // create the window

//QFrame *frame = new QFrame;
//frame->setObjectName("myframe");
//frame->setStyleSheet("QFrame#myframe{border-image:uml(./water_background_by_baggs.png)}");
//frame->show();

        Window window;
        window.showMaximized();

//window.setObjectName("mywindow");
//window.setStyleSheet("Window#mywindow{border-image:uml(./water_background_by_baggs.png)}");
//window.showMaximized();


        // call the window.timerEvent function every 40 ms
        window.startTimer(40);

        // execute the application
        return app.exec();
}
