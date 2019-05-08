#include "googletranslator.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrlQuery>


GoogleTranslator::GoogleTranslator()/* : Translator()*/
{
    canDetectLang = true;
    canTranslate = true;
    host = "translate.googleapis.com";
    path = "/translate_a/single";
    scheme = "https";
}

QNetworkReply *GoogleTranslator::setQueryToGoogleTranlator(const QString &str) const
{
//----- Create URL
    QUrl myurl;
    myurl.setScheme(scheme); //https also applicable
    myurl.setHost(host/*"ws.detectlanguage.com"*/); // https://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=uk&dt=t&q=Tiu%20frazo%20estas%20skribita%20en%20Esperanto.
    myurl.setPath(path/*"/0.2/detect"*/);

//----- Create URL query
    QUrlQuery querystr;
    querystr.addQueryItem("client", "gtx");
    querystr.addQueryItem("sl", "auto");
    querystr.addQueryItem("tl", "uk");
    querystr.addQueryItem("dt", "t");
    querystr.addQueryItem("q", str);
    //querystr.addQueryItem("key", "demo");

    myurl.setQuery(querystr); // Add query to URL

    qDebug() << "\n-------------- Translation";
    qDebug() << str;
    qDebug() << myurl.toString();

//----- Create network request
    QNetworkRequest request;
    request.setUrl(myurl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

//----- Create network access manager
    QNetworkAccessManager * restclient;
    restclient = new QNetworkAccessManager();
    connect(restclient, &QNetworkAccessManager::destroyed, [] () {qDebug() << "\n-------------- Translation manager deleted";});

//----- Create and read reply
    QNetworkReply * reply = restclient->get(request);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this] (QNetworkReply::NetworkError code) {
                emit this->errorOccurred( code );
    });
    connect(reply, &QNetworkReply::destroyed, [] () {qDebug() << "\n-------------- Translation reply deleted";});
    return reply;
}

void GoogleTranslator::translate(const QString &input) const
{
    QNetworkReply * reply = setQueryToGoogleTranlator(input);

    connect(reply, &QNetworkReply::finished, [reply, this] () {

    //----- Read data from reply and close reply with manager
        QByteArray arr = reply->readAll();
        reply->close();
        reply->manager()->clearAccessCache();
        reply->manager()->deleteLater();
        reply->deleteLater();

    //----- Parse server answer
        QJsonDocument doc = QJsonDocument::fromJson(arr);
        if (doc.isEmpty() && !doc.isArray()) {
            emit translated(QVector<Translation>());
            return;
        }

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty()) {
            emit translated(QVector<Translation>());
            return;
        }
        emit translated( { Translation( jArr.at(6).toDouble(), jArr.at(2).toString(), jArr.at(0).toArray().at(0).toArray().at(0).toString() ) } );
    });
}

void GoogleTranslator::detectLanguage(const QString &input) const
{
    QNetworkReply * reply = setQueryToGoogleTranlator(input);

    connect(reply, &QNetworkReply::finished, [reply, this] () {

    //----- Read data from reply and close reply with manager
        QByteArray arr = reply->readAll();
        reply->close();
        reply->manager()->clearAccessCache();
        reply->manager()->deleteLater();
        reply->deleteLater();

    //----- Parse server answer
        QJsonDocument doc = QJsonDocument::fromJson(arr);
        if (doc.isEmpty() && !doc.isArray()) {
            emit detected(QMap<QString, double>());
            return;
        }

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty()) {
            emit detected(QMap<QString, double>());
            return;
        }

        QMap<QString, double> answ;
        answ.insert(jArr.at(2).toString(), 1);
        emit detected(answ);
    });
}
