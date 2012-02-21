#include "translationservice.h"
#include <QApplication>
#include <QLibraryInfo>

QTranslator TranslationService::_qtTranslator;
QTranslator TranslationService::_appTranslator;

void TranslationService::loadDefault()
{
    if (!_qtTranslator.isEmpty())
    {
        qApp->removeTranslator(&_qtTranslator);
    }

    if (!_appTranslator.isEmpty())
    {
        qApp->removeTranslator(&_appTranslator);
    }
}

void TranslationService::loadEnglish()
{
    loadDefault();
}

void TranslationService::loadRussian()
{
    loadDefault();

    _qtTranslator.load("qt_ru", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&_qtTranslator);

    _appTranslator.load(":/qblokus2_ru");
    qApp->installTranslator(&_appTranslator);
}
