#include "keeper.h"
#include <QDebug>
#include <QDir>
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

    QFile file(settingDirPath + currentBookFileName + ".json");
    if ( !file.open(QIODevice::Append | QIODevice::Text) )
        return false;

    /*QTextStream out(&file);
    out << book;
    file.close();*/
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

void Keeper::setCashing(bool mustCashing)
{
     QSettings settings("EugeneLAB", "Explanatory FB2-Reader");
     settings.setValue("mustCash", mustCashing);
     itMustCashing = mustCashing;
}
