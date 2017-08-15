#ifndef LEANITEM_H
#define LEANITEM_H

#include "leansheets.h"

#include <QTableWidgetItem>

class LeanItem : public QTableWidgetItem
{
public:
    LeanItem();
    LeanItem(const QString &text);

    QTableWidgetItem *clone() const override;

    QVariant data(int role) const override;
    void setData(int role, const QVariant &value) override;
    QVariant display() const;

    /** Copyright (C) 2016 The Qt Company Ltd. **/
    inline QString function() const
    {
        return QTableWidgetItem::data(Qt::DisplayRole).toString();
    }

    static QVariant functionResult(const QString &formula,
                                   const QTableWidget *widget,
                                   const QTableWidgetItem *self = 0);


private:
    mutable bool isResolving;
};

#endif // LEANITEM_H
