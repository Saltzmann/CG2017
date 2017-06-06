#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Connected "Scrollen zum Speed einstellen"
    QObject::connect(ui->myglwid, SIGNAL(sendSpeedFactor(int)),
                     ui->sBXSpeed, SLOT(setValue(int)));
    //Connected "Anzeige FPS Counter"
    QObject::connect(ui->myglwid, SIGNAL(sendFPSValue(int)),
                     ui->lcdFPSCounter, SLOT(display(int)));
    //Connected "Linearer Beleuchtungsmodifikator"
    QObject::connect(ui->dSBlinMod, SIGNAL(valueChanged(double)),
                     ui->myglwid, SLOT(recieveLinModUpdate(double)));
    //Connected "Exponentieller Beleuchtungsmodifikator"
    QObject::connect(ui->dSBexpMod, SIGNAL(valueChanged(double)),
                     ui->myglwid, SLOT(recieveExpModUpdate(double)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
