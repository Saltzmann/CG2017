#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->myglwid, SIGNAL(sendSpeedFactor(int)),
                     ui->sBXSpeed, SLOT(setValue(int)));
    QObject::connect(ui->myglwid, SIGNAL(sendFPSValue(int)),
                     ui->lcdFPSCounter, SLOT(display(int)));
    QObject::connect(ui->dSBlinMod, SIGNAL(valueChanged(double)),
                     ui->myglwid, SLOT(recieveLinModUpdate(double)));
    QObject::connect(ui->dSBexpMod, SIGNAL(valueChanged(double)),
                     ui->myglwid, SLOT(recieveExpModUpdate(double)));

    //grabMouse();
    //setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
