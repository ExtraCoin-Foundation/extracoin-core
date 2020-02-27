#ifndef TRANSLATER_H
#define TRANSLATER_H

#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>

class Translater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lang READ lang WRITE setLang NOTIFY langChanged)
    Q_PROPERTY(QString systemLang READ systemLang NOTIFY systemLangChanged)

public:
    explicit Translater(QQmlApplicationEngine &engine, QObject *parent = nullptr);

    Q_INVOKABLE void selectLanguage(const QString &lang);
    QString lang() const;

    QString systemLang() const;

public slots:
    void setLang(QString lang);

signals:
    void langChanged(QString lang);
    void systemLangChanged(QString systemLang);

private:
    QQmlApplicationEngine &engine;
    QTranslator translater;
    QString m_lang;
    QString m_systemLang;
};

#endif // TRANSLATER_H
