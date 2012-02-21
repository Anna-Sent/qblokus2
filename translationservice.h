#ifndef TRANSLATIONSERVICE_H
#define TRANSLATIONSERVICE_H

#include <QTranslator>

class TranslationService
{
private:
    static QTranslator _qtTranslator;
    static QTranslator _appTranslator;
    static void loadDefault();
public:
    static void loadRussian();
    static void loadEnglish();
};

#endif // TRANSLATIONSERVICE_H
