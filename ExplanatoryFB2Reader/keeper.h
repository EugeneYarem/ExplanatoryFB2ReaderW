#ifndef KEEPER_H
#define KEEPER_H

#include <QObject>
#include <QString>

class Keeper : public QObject
{
    Q_OBJECT

public:
    struct Bookmark {
        int position = 0;
        int percent = 0;
        QString name = "";

        Bookmark() = default;
        Bookmark(const QString &name, int percent, int position) :
            position(position), percent(percent), name(name) {}
    };

private:
    bool itMustCashing = false;
    QString currentBookFileName;
    QString settingDirPath;

public:
    Keeper();

    bool saveBookmark(const QString &bookmarkName, int percent, int currentPosition) const;
    bool isMustCashing() const;
    bool saveBook(const QString &bookName, const QString &book);
    QVector<Bookmark> readBookmarks(const QString &bookName) const;

public slots:
    void setCashing(bool mustCashing = true);

};

#endif // KEEPER_H
