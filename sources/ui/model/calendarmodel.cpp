#include "ui/model/calendarmodel.h"

#include <QDate>
#include <QDebug>

CalendarModel::CalendarModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "day", "month", "year", "date" });
}

int CalendarModel::month() const
{
    return m_month;
}

int CalendarModel::year() const
{
    return m_year;
}

void CalendarModel::setMonth(int month)
{
    if (m_month == month)
        return;

    m_month = month;
    emit monthChanged(m_month);
}

void CalendarModel::setYear(int year)
{
    if (m_year == year)
        return;

    m_year = year;
    emit yearChanged(m_year);
}

void CalendarModel::generateCalendar()
{
    clear();
    QDate date(m_year, m_month, 1);
    int startDays = date.dayOfWeek() - 1;
    int daysInMonth = date.daysInMonth();

    if (!date.isValid() || !daysInMonth)
        return;

    // qDebug() << "+" << date << startDays << daysInMonth;

    append(QVariantMap { { "day", "M" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "T" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "W" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "T" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "F" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "S" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });
    append(QVariantMap { { "day", "S" }, { "date", QDate() }, { "month", m_month }, { "year", -100 } });

    for (int i = 0; i != startDays; ++i)
    {
        append(QVariantMap { { "day", 0 }, { "date", QDate() }, { "month", m_month }, { "year", m_year } });
    }

    for (int i = 1; i <= daysInMonth; ++i)
    {
        append(QVariantMap { { "day", i },
                             { "date", QDateTime(QDate(m_year, m_month, i), QTime(12, 0)) },
                             { "month", m_month },
                             { "year", m_year } });
        // qDebug() << m_month << i;
    }
}
