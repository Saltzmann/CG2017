#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QObject::connect(ui->sldRotationZ, SIGNAL(valueChanged(int),
    //                 ui->myglwid, SLOT(receiveRotationZ(int));
}

MainWindow::~MainWindow()
{
    delete ui;
}
