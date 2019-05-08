#ifndef GOOGLETRANSLATOR_H
#define GOOGLETRANSLATOR_H

#include "translator.h"

class QNetworkReply;

class GoogleTranslator : public Translator
{
public:
    GoogleTranslator();

    QNetworkReply *setQueryToGoogleTranlator(const QString &textToTranslate) const;
    void detectLanguage(const QString &input) const override;
    void translate(const QString &input) const override;

};

#endif // GOOGLETRANSLATOR_H
