#include "leanitem.h"

#include <QtMath>

/****************************************************************************
** The LeanItem class is responsible for managing data in each cell of the
** QTableWidget, also known generically as a QTableWidgetItem. The
** functionResult() method in particular determines which functions or
** operators to call based on the QString returned by the function() method.
****************************************************************************/

/** Copyright (C) 2016 The Qt Company Ltd. **/
LeanItem::LeanItem()
        : QTableWidgetItem(), isResolving(false)
{
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
LeanItem::LeanItem(const QString &text)
        : QTableWidgetItem(text), isResolving(false)
{
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
QTableWidgetItem *LeanItem::clone() const
{
    LeanItem *item = new LeanItem();
    *item = *this;
    return item;
}

// Determines how data is represented in each cell.
QVariant LeanItem::data(int role) const
{
    if (role == Qt::EditRole || role == Qt::StatusTipRole)
        return function();

    if (role == Qt::DisplayRole)
        return display();

    QString t = display().toString();
    bool isNumber = false;
    int number = t.toDouble(&isNumber);

    if (role == Qt::TextColorRole)
    {
        if (!isNumber)
            return QVariant::fromValue(QColor(Qt::black));
        else if (number < 0)
            return QVariant::fromValue(QColor(Qt::red));
        return QVariant::fromValue(QColor(Qt::blue));
    }

     if (role == Qt::TextAlignmentRole)
         if (!t.isEmpty() && (t.at(0).isNumber() || t.at(0) == '-'))
             return (int)(Qt::AlignRight | Qt::AlignVCenter);

     return QTableWidgetItem::data(role);
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanItem::setData(int role, const QVariant &value)
{
    QTableWidgetItem::setData(role, value);
    if (tableWidget())
        tableWidget()->viewport()->update();
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
QVariant LeanItem::display() const
{
    // avoid circular dependencies
    if (isResolving)
        return QVariant();

    isResolving = true;
    QVariant result = functionResult(function(), tableWidget(), this);
    isResolving = false;
    return result;
}

// Where LeanSheets' functions and operators roam.
QVariant LeanItem::functionResult(const QString &function,
                                         const QTableWidget *widget,
                                         const QTableWidgetItem *self)
{
    // check if the string is actually a function or not
    QStringList list = function.split(' ');
    if (list.isEmpty() || !widget)
        return function; // it is a normal string

    // What we'll return.
    QVariant result;

    // If an operator is called:
    if (list.value(1) == "+" || list.value(1) == "-" || list.value(1) == "*" || list.value(1) == "/" || list.value(1) == "^")
    {
        int opRow = 0;
        int opCol  = 0;

        // Collects data based on the row and column of the cell entered.
        decode_pos(list.value(0), &opRow, &opCol);
        const QTableWidgetItem *left = widget->item(opRow, opCol);
        decode_pos(list.value(2), &opRow, &opCol);
        const QTableWidgetItem *right = widget->item(opRow, opCol);

        double leftHand = 0;
        double rightHand = 0;

        QString temp = list.value(0);
        leftHand = left ? left->text().toDouble() : temp.toDouble();
        temp = list.value(2);
        rightHand = right ? right->text().toDouble() : temp.toDouble();

        // Methods for each operator:
        if (list.value(1) == "+")
            result = leftHand + rightHand;
        else if (list.value(1) == "-")
            result = leftHand - rightHand;
        else if (list.value(1) == "*")
            result = leftHand * rightHand;
        else if (list.value(1) == "/" && rightHand != 0)
            result = leftHand / rightHand;
        else if (list.value(1) == "^")
            result = qPow(leftHand, rightHand);

        return result;
    }

    QString splitFunction = list.value(0).toLower();

    int singleRow = -1;
    int singleCol = -1;
    int firstRow = -1;
    int firstCol = -1;
    int lastRow = -1;
    int lastCol = -1;

    // Stores consecutive rows and columns into QVectors below.
    QVector<int> numRows;
    QVector<int> numCols;

    for (int pos = 1; pos < list.count(); pos++)
    {
        decode_pos(list.value(pos), &singleRow, &singleCol);
        numRows.append(singleRow);
        numCols.append(singleCol);
    }

    if (!numRows.empty() && !numCols.empty())
    {
        qSort(numRows.begin(), numRows.end());
        qSort(numCols.begin(), numCols.end());
        firstRow = numRows.first();
        firstCol = numCols.first();
        lastRow = numRows.last();
        lastCol = numCols.last();
    }

    // Methods for 'sum=' and 'product='
    if (splitFunction == "sum=" || splitFunction == "product=")
    {
        double sum = 0;
        double prod = 1;

        for (int row = firstRow; row <= lastRow; ++row)
        {
            for (int col = firstCol; col <= lastCol; ++col)
            {
                const QTableWidgetItem *tableItem = widget->item(row, col);
                if (tableItem && tableItem != self)
                {
                    if (splitFunction == "sum=")
                        sum += tableItem->text().toDouble();
                    else
                        prod *= tableItem->text().toDouble();
                }
            }
        }

        if (splitFunction == "sum=")
            result = sum;
        else
            result = prod;
    }
    // Method for 'sqrt=' function.
    else if (splitFunction == "sqrt=")
    {
        int sqrRow = 0;
        int sqrCol = 0;

        decode_pos(list.value(1), &sqrRow, &sqrCol);
        const QTableWidgetItem *sqrItem = widget->item(sqrRow, sqrCol);
        double sqrResult = 0;
        QString sqrTemp = list.value(1);
        sqrResult = sqrItem ? sqrItem->text().toDouble() : sqrTemp.toDouble();

        result = qSqrt(sqrResult);
    }
    // Methods for 'median=', 'min=', and 'max=' functions.
    else if (splitFunction == "median=" || splitFunction == "min=" || splitFunction == "max=")
    {
        QVector<int> medStore;
        for (int row = firstRow; row <= lastRow; ++row)
        {
            for (int col = firstCol; col <= lastCol; ++col)
            {
                const QTableWidgetItem *tableItem = widget->item(row, col);
                if (tableItem && tableItem != self)
                    medStore.append(tableItem->text().toDouble());
            }
        }

        qSort(medStore);

        if (splitFunction == "median=")
        {
            if (medStore.size() % 2)
                result = medStore.at(medStore.count() / 2);
            else
                result = (medStore.at(medStore.count() / 2) + medStore.at((medStore.count() / 2) + 1))/2;
        }
        else if (splitFunction == "min=")
            result = medStore.first();
        else if (splitFunction == "max=")
            result = medStore.last();
    }
    // Method for 'average=' function.
    else if (splitFunction == "average=")
    {
        double avgSum = 0;
        double avgCount = 0;

        for (int row = firstRow; row <= lastRow; ++row)
        {
            for (int col = firstCol; col <= lastCol; ++col)
            {
                const QTableWidgetItem *tableItem = widget->item(row, col);
                if (tableItem && tableItem != self)
                {
                    avgSum += tableItem->text().toDouble();
                    avgCount++;
                }
            }
        }

        result = avgSum / avgCount;
    }
    // Method for 'stdev=' function.
    else if (splitFunction == "stdev=") {
        double stdSum = 0;
        double avgSum = 0;
        double stdAvg = 0;
        int stdCount = 0;

        for (int row = firstRow; row <= lastRow; ++row)
        {
            for (int col = firstCol; col <= lastCol; ++col)
            {
                const QTableWidgetItem *tableItem = widget->item(row, col);
                if (tableItem && tableItem != self)
                {
                    avgSum += tableItem->text().toDouble();
                    stdCount++;
                }
            }
        }
        stdAvg = avgSum / stdCount;

        for (int row = firstRow; row <= lastRow; ++row)
        {
            for (int col = firstCol; col <= lastCol; ++col)
            {
                const QTableWidgetItem *tableItem = widget->item(row, col);
                if (tableItem && tableItem != self)
                    stdSum += qPow(tableItem->text().toDouble() - stdAvg, 2);
            }
        }

        result = qSqrt(stdSum / (stdCount - 1));
    }
    else
        result = function;

    return result;
}
