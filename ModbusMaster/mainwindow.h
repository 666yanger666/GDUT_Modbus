#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>

class MasterDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPlainTextEdit *m_logWindow;

private:
    QWidget* m_mainWidget;
    QVBoxLayout* m_vMainLayout;
    MasterDialog* m_MDialog;
    QLabel* m_copyright;
};

#endif // MAINWINDOW_H
