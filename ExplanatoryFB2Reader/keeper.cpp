#include "keeper.h"
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QSettings>


Keeper::Keeper()
{
    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
    settingDirPath = QDir::toNativeSeparators(env.value("USERPROFILE") + "/AppData/Local/Explanatory FB2-Reader/");

    QSettings settings("EugeneLAB", "Explanatory FB2-Reader");
    itMustCashing = settings.value("mustCash").toBool();
}

bool Keeper::saveBookmark(const QString &bookmarkName, int percent, int currentPosition) const
{
    if (!QFile::exists(settingDirPath))
        QDir().mkdir(settingDirPath);

    QFile file(settingDirPath + currentBookFileName + ".bookmarks");
    if ( !file.open(QIODevice::ReadWrite | QIODevice::Text) )
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonDocument newD;
    QJsonArray newA;
    QJsonObject bookmark( { QPair<QString, QJsonValue>("name", bookmarkName),
                            QPair<QString, QJsonValue>("currentPosition", currentPosition),
                            QPair<QString, QJsonValue>("percent", percent)
                          } );
    if (doc.isEmpty() && !doc.isArray())
        doc = newD;

    QJsonArray jArr = doc.array();
    if (jArr.isEmpty())
        jArr = newA;

    jArr.append(bookmark);
    doc.setArray(jArr);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool Keeper::isMustCashing() const
{
    return itMustCashing;
}

bool Keeper::saveBook(const QString &bookName, const QString &book)
{
    currentBookFileName = bookName;

    if (itMustCashing) {
        if (!QFile::exists(settingDirPath))
            QDir().mkdir(settingDirPath);

        QFile file(settingDirPath + bookName + ".html");
        if ( !file.open(QIODevice::WriteOnly | QIODevice::Text) )
            return false;

        QTextStream out(&file);
        out << book;
        file.close();
    }
    return true;
}

QVector<Keeper::Bookmark> Keeper::readBookmarks(const QString &bookName) const
{
    if (!QFile::exists(settingDirPath))
        return {};

    QFile file(settingDirPath + bookName + ".bookmarks");
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
        return {};

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isEmpty() && !doc.isArray())
        return {};

    QJsonArray jArr = doc.array();
    if (jArr.isEmpty())
        return {};

    QVector<Bookmark> answ;

    QJsonArray::iterator i = jArr.begin();
    while (i != jArr.end()) {
        QJsonObject obj = i->toObject();
        Bookmark bM(obj["name"].toString(), obj["percent"].toInt(), obj["currentPosition"].toInt());
        answ.append(bM);
        ++i;
    }

    return answ;
}

void Keeper::setCashing(bool mustCashing)
{
     QSettings settings("EugeneLAB", "Explanatory FB2-Reader");
     settings.setValue("mustCash", mustCashing);
     itMustCashing = mustCashing;
}
