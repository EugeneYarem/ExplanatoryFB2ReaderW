#include "translator.h"


Translator::Translator() : QObject()
{}

bool Translator::itCanTranslate() const
{
    return canTranslate;
}

bool Translator::itCanDetectLang() const
{
    return canDetectLang;
}
