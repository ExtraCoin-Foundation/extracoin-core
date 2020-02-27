#ifndef NEWSMODEL_H
#define NEWSMODEL_H

#include "ui/model/abstractmodel.h"
#include "dfs/managers/headers/card_manager.h"
#include <QDebug>

class UiController;

class NewsModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(int newsCount READ newsCount WRITE setNewsCount NOTIFY newsCountChanged)
    Q_PROPERTY(QString errors READ errors WRITE setErrors NOTIFY errorsChanged)

public:
    explicit NewsModel(AbstractModel *parent = nullptr);
    ~NewsModel();

    Q_INVOKABLE void loadLikesPosts(QString dbName);
    Q_INVOKABLE void loadPosts(QStringList subs);
    Q_INVOKABLE void loadUserPosts();
    Q_INVOKABLE void clear();

    QString userId() const;

    Q_INVOKABLE void setUiController(UiController *value);
    Q_INVOKABLE void setWidth(double uiWidth);

    Q_INVOKABLE QVariantMap post(QString userId, QString file);

    enum PostType
    {
        Post,
        Event
    };

    int newsCount() const
    {
        return m_newsCount;
    }

    QString errors() const
    {
        return m_errors;
    }

signals:
    void userIdChanged(QString userId);

    void newsCountChanged(int newsCount);

    void errorsChanged(QString errors);

public slots:
    void loadPost(const QString &fileName, int type = 0);
    void setUserId(const QString &userId);

    void setNewsCount(int newsCount)
    {
        if (m_newsCount == newsCount)
            return;

        m_newsCount = newsCount;
        emit newsCountChanged(m_newsCount);
    }

    void setErrors(QString errors)
    {
        if (m_errors == errors)
            return;

        m_errors = errors;
        emit errorsChanged(m_errors);
    }

private:
    std::vector<std::string> files;
    QString m_userId = "-333";
    UiController *uiController;
    double uiWidth = 0;
    int m_newsCount = 0;
    QString m_errors;
};

#endif // NEWSMODEL_H
