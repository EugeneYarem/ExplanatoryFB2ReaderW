#ifndef KEEPER_H
#define KEEPER_H

#include <QObject>
#include <QString>

class Keeper : public QObject
{
    Q_OBJECT

    bool itMustCashing = false;
    QString currentBookFileName;
    QString settingDirPath;

public:
    Keeper();

    bool saveBookmark(const QString &bookmarkName, int percent, int currentPosition) const;
    bool isMustCashing() const;
    bool saveBook(const QString &bookName, const QString &book);

public slots:
    void setCashing(bool mustCashing = true);

};

#endif // KEEPER_H
