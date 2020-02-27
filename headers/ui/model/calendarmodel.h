#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include "ui/model/abstractmodel.h"

struct Day
{
    int day = 0;
    int weekDay = 0;
};

class CalendarModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(int month READ month WRITE setMonth NOTIFY monthChanged)
    Q_PROPERTY(int year READ year WRITE setYear NOTIFY yearChanged)

public:
    explicit CalendarModel(AbstractModel *parent = nullptr);

    int month() const;
    int year() const;

    /**
     * @brief Create calendar based on model year and month values
     */
    Q_INVOKABLE void generateCalendar();

public slots:
    void setMonth(int month);
    void setYear(int year);

signals:
    void monthChanged(int month);
    void yearChanged(int year);

private:
    int m_month = 0;
    int m_year = 0;
};

#endif // CALENDARMODEL_H
