#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt includes
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
//includes for json parsing
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QWebEngineView> // this is for the login webview

#include <QString>
#include <QLoggingCategory> // this is for disabling the ssl certificate warnings
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QString>
#include <iostream>
using namespace std;
QString client_id="262658486767-qddccp22dhrh6uk682g7uq26itntf0vo.apps.googleusercontent.com";
QString client_secret="prYAYIMZSdOb-f9An-82JIUH";
QString device_code;
QString user_code;
QString grant_type = "http://oauth.net/grant_type/device/1.0";
QString access_token;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");  // just to ignore the logged ssl warnings

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest networkRequest( QUrl( QString("https://accounts.google.com/o/oauth2/device/code")));
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString content = QString("client_id=%1&scope=%2")
            .arg("262658486767-qddccp22dhrh6uk682g7uq26itntf0vo.apps.googleusercontent.com")
            .arg("https://www.googleapis.com/auth/calendar.readonly");

    QNetworkReply *addr_reply = mgr.post(networkRequest, content.toUtf8());
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //qDebug() << "Success " <<addr_reply->readAll() << "\n";
        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);

        device_code = document.object().value("device_code").toString();
        user_code = document.object().value("user_code").toString();
        ui->label_code->setText(user_code);
        qDebug() << device_code << "\n" << user_code;
    }
    else {
        //failure
        qDebug() << "Failure" <<addr_reply->errorString();
    }
    eventLoop.quit();

    // Web view Portion

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    ui->label_web->setAttribute( Qt::WA_DeleteOnClose );      // delete object on closing or exiting
    ui->label_web->load(QUrl("https://www.google.com/device"));
    ui->label_web->setZoomFactor(0.85);
    ui->label_web->resize(440, 370);
    ui->label_web->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_token_clicked()
{
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");  // just to ignore the logged ssl warnings

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest networkRequest( QUrl( QString("https://www.googleapis.com/oauth2/v4/token")));
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString content = QString("client_id=%1&client_secret=%2&code=%3&grant_type=%4")
            .arg(client_id)
            .arg(client_secret)
            .arg(device_code)
            .arg(grant_type);

    QNetworkReply *addr_reply = mgr.post(networkRequest, content.toUtf8());
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //qDebug() << "Success " <<addr_reply->readAll() << "\n";
        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);

        access_token = document.object().value("access_token").toString();
        qDebug() << "Access Token: " << access_token<< "\n";
    }
    else {
        //failure
        qDebug() << "Failure" <<addr_reply->errorString();
    }
    eventLoop.quit();
}

void MainWindow::on_pushButton_event_clicked()
{
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");  // just to ignore the logged ssl warnings

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest networkRequest( QUrl( QString("https://www.googleapis.com/calendar/v3/calendars/primary/events?timeMax=2018-08-11T23%3A59%3A59%2B06%3A00&timeMin=2018-08-05T00%3A00%3A00%2B06%3A00&fields=items%2Fsummary")));

    QString prepared_access_token = "Bearer " + access_token;
    networkRequest.setRawHeader("Authorization", prepared_access_token.toUtf8());

    QNetworkReply *addr_reply = mgr.get(networkRequest);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //qDebug() << "Success " <<addr_reply->readAll() << "\n";
        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        QJsonObject items_object = document.object();
        QJsonValue items_value = items_object.value("items");
        QJsonArray items_array = items_value.toArray();
        QString event_name;

        if(!items_array.isEmpty()){
            event_name = items_array[0].toObject().value("summary").toString();
        }
        else {
            event_name = "No matched event found.";
        }

        qDebug() << "Event Name: " << event_name;
        ui->label_event->setText("Event Name: " + event_name);
    }
    else {
        //failure
        qDebug() << "Failure" <<addr_reply->errorString();
    }
    eventLoop.quit();
}
