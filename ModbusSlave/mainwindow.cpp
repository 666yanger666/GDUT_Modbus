#include "mainwindow.h"
#include "slavedialog.h"

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setMaximumSize(400,500);
    this->setMinimumSize(400,500);
    this->setWindowIcon(QIcon(":/slave.ico"));

    m_logWindow = new QPlainTextEdit;
    m_logWindow->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_logWindow->setStyleSheet("font-size:18px;");
    m_logWindow->setReadOnly(true);

    m_copyright = new QLabel("QQ: 619039452 MADE BY YOUNG");
    m_copyright->setStyleSheet("font-size:13px;font-weight:580;");

    m_vMainLayout = new QVBoxLayout;
    m_MDialog = new SlaveDialog(this);
    m_vMainLayout->addWidget(m_MDialog,1);
    m_vMainLayout->addWidget(m_logWindow,1);
    m_vMainLayout->addWidget(m_copyright,0,Qt::AlignBottom|Qt::AlignCenter);
    m_mainWidget = new QWidget;
    m_mainWidget->setLayout(m_vMainLayout);
    setCentralWidget(m_mainWidget);
}

MainWindow::~MainWindow()
{

}
