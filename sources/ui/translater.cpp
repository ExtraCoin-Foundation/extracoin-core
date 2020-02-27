#include "ui/translater.h"

#include <QLocale>
#include <QFile>
#include <QDebug>

Translater::Translater(QQmlApplicationEngine &engine, QObject *parent)
    : QObject(parent)
    , engine(engine)
{
    qApp->installTranslator(&translater);

#ifdef QT_DEBUG
    m_systemLang = QLocale().name().left(2);
#else
    m_systemLang = "en";
#endif
    systemLangChanged(m_systemLang);
    // setLang(m_systemLang);
}

void Translater::selectLanguage(const QString &lang)
{
    translater.load(QString(":/ExtraCoin_%1").arg(lang), ".");
    engine.retranslate();
}

QString Translater::lang() const
{
    return m_lang;
}

QString Translater::systemLang() const
{
    return m_systemLang;
}

void Translater::setLang(QString lang)
{
    if (lang == "system" || lang.isEmpty())
    {
        setLang(m_systemLang);
        return;
    }

    QString langFile = QString(":/ExtraCoin_%1.qm").arg(lang);
    bool langFileExists = QFile::exists(langFile);
    if (!langFileExists)
    {
        setLang("en");
        return;
    }

    if (m_lang == lang)
        return;

    lang = "en"; // always english
    m_lang = lang;
    translater.load(QString(":/ExtraCoin_%1").arg(m_lang), ".");
    engine.retranslate();
    emit langChanged(m_lang);
}
