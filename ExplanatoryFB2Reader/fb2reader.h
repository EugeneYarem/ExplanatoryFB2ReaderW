#ifndef FB2READER_H
#define FB2READER_H

#include <QObject>
#include <QString>
#include <QMap>

class FB2Reader : public QObject
{
    Q_OBJECT

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

    QString chapterIdPrefix = "ExplanatoryFB2ReaderChapterId@=";
    QString tokenIdPrefix = "ExplanatoryFB2ReaderId@=";

public:
    FB2Reader() = default;

    const QMap<unsigned int, ChapterData> & getContent();
    const QString & book() const;
    int findPositionByChapterId(const QString &plainText, unsigned int index);
    int getNumberBookCharacters();    
    QString getBookName() const;
    QString getChapterName(unsigned int index) const;
    QString getFirstChapterName() const;    
    void setNumberBookCharacters(int number);

private:
    void clearBeforeParsing();
    void removeLineBreaksFromContent();

public slots:
    void parseBook(const QString &fileName);

};

#endif // FB2READER_H
