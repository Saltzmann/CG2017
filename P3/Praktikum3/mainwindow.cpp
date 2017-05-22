#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->myglwid, SIGNAL(sendSpeedFactor(int)),
                     ui->sbxScaleZ, SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
