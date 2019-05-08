#include "wikipediaexpounder.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrlQuery>


WikipediaExpounder::WikipediaExpounder(const QString &language) : Expounder ()
{
    host = language + ".wikipedia.org";
    path = "/w/api.php";
    scheme = "http";
}

void WikipediaExpounder::findExplanation(const QString &str) const
{
//----- Create URL
    QUrl myurl;
    myurl.setScheme(scheme); //https also applicable
    myurl.setHost(host);
    myurl.setPath(path);

//----- Create URL query
    QUrlQuery querystr;
    querystr.addQueryItem("action", "opensearch");
    querystr.addQueryItem("search", str);
    querystr.addQueryItem("prop", "info");
    querystr.addQueryItem("format", "json");
    querystr.addQueryItem("inprop", "url");

    myurl.setQuery(querystr); // Add query to URL

    qDebug() << "\n-------------- Definition";
    qDebug() << str;
    qDebug() << myurl.toString();

//----- Create network request
    QNetworkRequest request;
    request.setUrl(myurl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

//----- Create network access manager
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager();
    connect(restclient, &QNetworkAccessManager::destroyed, [] () { qDebug() << "\n-------------- Definition manager deleted"; });

//----- Create and read reply
    QNetworkReply *reply = restclient->get(request);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this] (QNetworkReply::NetworkError code) {
                emit this->errorOccurred( code );
    });
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
            emit explanationFound( {} );
            return;
        }

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty()) {
            emit explanationFound( {} );
            return;
        }

        QVector<QJsonArray> columns;

        QJsonArray::iterator i = jArr.begin();
        int curColumn = 0;
        while (i != jArr.end() && curColumn < 3) {
            if (i->isArray()) {
                columns.push_back(i->toArray());
                ++curColumn;
            }
            ++i;
        }

        QVector<Explanation> answ;
        for (int j = 0; j < columns.at(0).count(); ++j) {
            answ.append(Explanation(columns.at(0).at(j).toString(),
                                    columns.at(1).at(j).toString(),
                                    columns.at(2).at(j).toString()));

        }

        emit explanationFound(answ);
    });

    connect(reply, &QNetworkReply::destroyed, [] () { qDebug() << "\n-------------- Definition reply deleted"; });
}
