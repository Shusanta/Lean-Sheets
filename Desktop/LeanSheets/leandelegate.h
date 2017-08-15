#ifndef LEANDELEGATE_H
#define LEANDELEGATE_H

#include "leansheets.h"

#include <QItemDelegate>
#include <QtWidgets>

class LeanDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    LeanDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private slots:
    void commitAndCloseEditor();
};

#endif // LEANDELEGATE_H
