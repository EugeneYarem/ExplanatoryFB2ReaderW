#ifndef FB2READER_H
#define FB2READER_H

#include <QObject>
#include <QString>
#include <QMap>

class FB2Reader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString book READ book NOTIFY bookChanged)

public:
    struct ChapterData {
        int posInPlainText = 0;
        QString name;

        ChapterData() = default;
        ChapterData(QString name) : name(name) {}
    };

private:
    int numberBookCharacters = 0;
    QMap<unsigned int, ChapterData> m_content;
    QString m_book = "";
    QString m_bookData = "";

    QString chapterIdPrefix = "ExplanatoryFB2ReaderChapterId@=";
    QString tokenIdPrefix = "ExplanatoryFB2ReaderId@=";  

    //QStringList m_content;

public:
    FB2Reader() = default;

    const QMap<unsigned int, ChapterData> & getContent();
    const QString & book() const;
    int getNumberBookCharacters();
    int findPositionByChapterId(const QString & plainText, unsigned int index);
    QString getBookName() const;
    //QString getChapterIdentifier(int index) const;
    QString getChapterName(unsigned int index) const;
    QString getFirstChapterName() const;    
    void setNumberBookCharacters(int number);

private:
    void removeLineBreaksFromContent();

public slots:
    void parseBook(const QString & fileName);

signals:
    void bookChanged();

};

#endif // FB2READER_H
