#ifndef EXPOUNDER_H
#define EXPOUNDER_H

#include <QObject>
#include <QString>

class Expounder : public QObject
{
    Q_OBJECT

public:
    struct Explanation {
        QString brief = "";
        QString idea = "";
        QString link = "";

        Explanation() = default;
        Explanation(const QString &idea, const QString &brief, const QString &link) :
            brief(brief), idea(idea), link(link) {}
    };

protected:
    QString host = "";
    QString path = "";
    QString scheme = ""; // http, https

public:
    Expounder() = default;
    virtual ~Expounder() = default;

    virtual void findExplanation(const QString &input) const = 0;

signals:
    void errorOccurred(int code) const;
    void explanationFound(QVector<Explanation> explanations) const;

};

#endif // EXPOUNDER_H
