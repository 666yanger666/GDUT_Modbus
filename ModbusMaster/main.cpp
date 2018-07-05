#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QComboBox {border-radius:2px; width:70px; height:32px; border:1px solid #ccc; font-size:18px;} \
                    QComboBox:hover {border-radius:3px; height:28px; border:1px solid #4f9fcf;} \
                    QComboBox::pressed {border-radius:3px; height:28px; border:1px solid #4f9fcf;} \
                    QLabel {font-size:20px;} \
                    QPushButton#refresh {font-weight:800; border-radius:3px; width:70px; height:32px; border:1px solid #ccc;} \
                    QPushButton#refresh:hover {border-radius:3px; height:28px; border:1px solid #4f9fcf;} \
                    QPushButton#connectOrDisconnect {font-weight:580; border-radius:3px; width:100px; height:28px; border:1px solid #ccc;} \
                    QPushButton#connectOrDisconnect:hover {border-radius:3px; height:28px; border:1px solid #4f9fcf;} \
                    QPushButton#startOrStop {border:0;} \
                    QPushButton#startOrStop {color:black; font-size:10pt; font-weight:580;} \
                    QPushButton#startOrStop:hover {width:50px; height:44px; background-color: #ec971f; border-right:3px solid gray; border-bottom:3px solid gray;} \
                    QPushButton#startOrStop {width:60px; height:50px; background-color: #f0ad4e; border-right:3px solid gray; border-bottom:3px solid gray;} \
                    QPushButton#startOrStop:pressed {border-left:4px solid gray; border-top:4px solid gray; border-right:0 solid gray; border-bottom:0 solid gray; height:66px; background-color:#f0ad4e;} \
                    QPushButton#lamp {border-left:0px; border-top:0px; border-right:0; border-bottom:0;} \
                    QPushButton#lamp {width:100px; height:100px; color:black; font-size:15pt; font-weight:580;} \
                    QPushButton#lamp {border-radius:50px; background-color:red;} \
                    ");
    MainWindow w;
    w.setWindowTitle(QStringLiteral("Modbus Master"));
    w.show();

    return a.exec();
}
