#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWebEngineView> // this is for the login webview

using namespace std;

GCalendar gc_obj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_login_clicked()
{
    QString user_code = gc_obj.gLogin();
    ui->label_code->setText(user_code);

    // Web view Portion
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QWebEngineView *mView_2 = new QWebEngineView();
    ui->label_web->setAttribute( Qt::WA_DeleteOnClose );      // delete object on closing or exiting
    ui->label_web->load(QUrl("https://www.google.com/device"));
    ui->label_web->setZoomFactor(0.85);
    ui->label_web->resize(440, 370);
    ui->label_web->show();
}

void MainWindow::on_pushButton_token_clicked()
{
    gc_obj.getAccessToken();    // first need to call this for the system to get the access token
    QString event_name = gc_obj.getEvent();
    ui->label_event->setText(event_name);
}

void MainWindow::on_pushButton_event_clicked()
{
    //QString event_name = gc_obj.getEvent();
    //ui->label_event->setText(event_name);
}
