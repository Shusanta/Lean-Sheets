#include "leandelegate.h"

/****************************************************************************
** The LeanDelegate class is derived from the QAbstractItemDelegate Class,
** which contains virtual functions designed to be reimplemented based on
** how data is stored in a model. In this case, QStrings are the main
** intermediary for data manipulated in the LeanItem class.
****************************************************************************/

LeanDelegate::LeanDelegate(QObject *parent)
        : QItemDelegate(parent) {}

/** Copyright (C) 2016 The Qt Company Ltd. **/
QWidget *LeanDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &,
                                          const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);

    QStringList allStrings;
    for (int i = 1; i<index.model()->rowCount(); i++)
    {
        QString strItem(index.model()->data(index.sibling(i, index.column()),
            Qt::EditRole).toString());

        if (!allStrings.contains(strItem))
            allStrings.append(strItem);
    }

    QCompleter *autoComplete = new QCompleter(allStrings);
    editor->setCompleter(autoComplete);
    connect(editor, &QLineEdit::editingFinished, this, &LeanDelegate::commitAndCloseEditor);
    return editor;
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit)
    {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit)
    {
        model->setData(index, edit->text());
        return;
    }
}
