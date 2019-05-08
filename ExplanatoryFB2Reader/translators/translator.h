#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QMap>
#include <QObject>
#include <QString>

class Translator : public QObject
{
    Q_OBJECT

public:
    struct Translation {
        double probability = 0;
        QString language = "";
        QString translation = "";

        //Translation() = default;
        Translation(double probability, const QString &language, const QString &translation) :
            probability(probability), language(language), translation(translation) {}
    };

protected:
    bool canDetectLang = false;
    bool canTranslate = false;
    QString host = "";
    QString path = "";
    QString scheme = ""; // http, https

public:
    Translator();
    virtual ~Translator() = default;

    bool itCanTranslate() const;
    bool itCanDetectLang() const;
    virtual void detectLanguage(const QString &input) const = 0;
    virtual void translate(const QString &input) const = 0;

signals:
    void detected(QMap<QString, double> langAndProbability) const;
    void errorOccurred(int code) const;
    void translated(QVector<Translation> translations) const;

};

#endif // TRANSLATOR_H
