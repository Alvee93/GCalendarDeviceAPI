#include "gcalendar.h"

using namespace std;
QString client_id="_CLIENT_ID_goes_here_";
QString client_secret="_CLIENT_SECRET_goes_here_";
QString device_code;
QString user_code;
QString grant_type = "http://oauth.net/grant_type/device/1.0";
QString access_token;

GCalendar::GCalendar()
{

}

QString GCalendar::gLogin()
{
    // just to ignore the logged ssl warnings
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl device_url = "https://accounts.google.com/o/oauth2/device/code";
    QNetworkRequest networkRequest(device_url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString scope_url =  "https://www.googleapis.com/auth/calendar.readonly";
    QString content = QString("client_id=%1&scope=%2")
            .arg(client_id)
            .arg(scope_url);

    QNetworkReply *addr_reply = mgr.post(networkRequest, content.toUtf8());
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //success WORKS WELL, however enabling this dont show the parsed json values!
        //qDebug() << "Success " <<addr_reply->readAll() << "\n";
        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);

        device_code = document.object().value("device_code").toString();
        user_code = document.object().value("user_code").toString();

        qDebug() << device_code << "\n" << user_code;
    }
    else {
        //failure
        qDebug() << "Failure" <<addr_reply->errorString();
    }
    eventLoop.quit();
    return user_code;
}

void GCalendar::getAccessToken()
{
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");  // just to ignore the logged ssl warnings

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl token_url = "https://www.googleapis.com/oauth2/v4/token";
    QNetworkRequest networkRequest(token_url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString content = QString("client_id=%1&client_secret=%2&code=%3&grant_type=%4")
            .arg(client_id)
            .arg(client_secret)
            .arg(device_code)
            .arg(grant_type);

    QNetworkReply *addr_reply = mgr.post(networkRequest, content.toUtf8());
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //success WORKS WELL somehow enabling this dont show the parsed json values!!!
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

QString GCalendar::getEvent()
{
    QString event_name;
    // just to ignore the logged ssl warnings
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl api_endpoint = "https://www.googleapis.com/calendar/v3/calendars/primary/events?timeMax=2018-08-11T23%3A59%3A59%2B06%3A00&timeMin=2018-08-05T00%3A00%3A00%2B06%3A00&fields=items%2Fsummary";
    QNetworkRequest networkRequest(api_endpoint);

    QString prepared_access_token = "Bearer " + access_token;
    networkRequest.setRawHeader("Authorization", prepared_access_token.toUtf8());

    QNetworkReply *addr_reply = mgr.get(networkRequest);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        //success WORKS WELL somehow enabling this dont show the parsed json values!!!
        //qDebug() << "Success " <<addr_reply->readAll() << "\n";
        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        QJsonObject items_object = document.object();
        QJsonValue items_value = items_object.value("items");
        QJsonArray items_array = items_value.toArray();

        if(!items_array.isEmpty()){
            event_name = items_array[0].toObject().value("summary").toString();
        }
        else {
            event_name = "No matched event found.";
        }

        qDebug() << "Event Name: " << event_name;
    }
    else {
        //failure
        qDebug() << "Failure" <<addr_reply->errorString();
    }
    eventLoop.quit();
    return event_name;
}
