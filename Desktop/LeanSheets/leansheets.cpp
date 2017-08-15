#include "leansheets.h"
#include "leandelegate.h"
#include "leanitem.h"

#define ALPHA 26

/****************************************************************************
** The LeanSheets class encapsulates the data used to run the
** graphical interface of LeanSheets. Much attention should be paid to
** the QTableWidget 'table' which is the basis of spreadsheet functionality.
** The majority of the functions in this class represent operations
** defined in the various menus of the program.
****************************************************************************/

LeanSheet::LeanSheet(int rows, int cols, QWidget *parent)
        : QMainWindow(parent)
{
    curFile = nullptr;

    addToolBar(toolBar = new QToolBar());
    formulaInput = new QLineEdit();

    cellLabel = new QLabel(toolBar);
    cellLabel->setMinimumSize(80, 0);

    toolBar->addWidget(cellLabel);
    toolBar->addWidget(formulaInput);

    table = new QTableWidget(rows, cols, this);
    table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    // Names each column starting with 'A'
    for (int c = 0; c < cols; ++c)
    {
        QString character(QChar('A' + c));
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    }

    table->setItemPrototype(table->item(rows - 1, cols - 1));
    table->setItemDelegate(new LeanDelegate());

    createActions();
    setupMenuBar();
    setupContents();
    setCentralWidget(table);

    // Connects functions which allow the user to manipulate cells.
    statusBar();
    connect(table, &QTableWidget::currentItemChanged,
            this, &LeanSheet::updateStatus);
    connect(table, &QTableWidget::currentItemChanged,
            this, &LeanSheet::updateLineEdit);
    connect(table, &QTableWidget::itemChanged,
            this, &LeanSheet::updateStatus);
    connect(formulaInput, &QLineEdit::returnPressed, this, &LeanSheet::returnPressed);
    connect(table, &QTableWidget::itemChanged,
            this, &LeanSheet::updateLineEdit);

    setWindowTitle(tr("LeanSheets"));
    // If "Logo.png" does not appear, resource directory may be misconfigured.
    setWindowIcon(QIcon(":/Logo/Logo.png"));
}

void LeanSheet::createActions()
{
    // Connects File actions
    openAction = new QAction(tr("Open"), this);
    connect(openAction, &QAction::triggered, this, &LeanSheet::openFile);

    saveAction = new QAction(tr("Save"), this);
    connect(saveAction, &QAction::triggered, this, &LeanSheet::saveFile);

    saveAsAction = new QAction(tr("Save As"), this);
    connect(saveAsAction, &QAction::triggered, this, &LeanSheet::saveAs);

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    // Connects Insert Actions
    rowInsert = new QAction(tr("Row"), this);
    connect(rowInsert, &QAction::triggered, this, &LeanSheet::insertRow);

    colInsert = new QAction(tr("Column"), this);
    connect(colInsert, &QAction::triggered, this, &LeanSheet::insertCol);

    // Connects Edit Actions
    cutAction = new QAction(tr("Cut"), this);
    cutAction->setShortcut(QKeySequence(QKeySequence::Cut));
    connect(cutAction, &QAction::triggered, this, &LeanSheet::cut);

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence(QKeySequence::Copy));
    connect(copyAction, &QAction::triggered, this, &LeanSheet::copy);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence(QKeySequence::Paste));
    connect(pasteAction, &QAction::triggered, this, &LeanSheet::paste);

    // Connects Help Actions
    functionList = new QAction(tr("Functions"), this);
    functionList->setShortcut(QKeySequence(QKeySequence::Find));
    connect(functionList, &QAction::triggered, this, &LeanSheet::showFunctions);

    operatorList = new QAction(tr("Operators"), this);
    operatorList->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    connect(operatorList, &QAction::triggered, this, &LeanSheet::showOperators);

    aboutLeanSheets = new QAction(tr("About Leansheets"), this);
    connect(aboutLeanSheets, &QAction::triggered, this, &LeanSheet::showAbout);
}

void LeanSheet::setupMenuBar()
{
    // Sets up File operations
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exitAction);

    // Sets up Edit operations
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);

    // Sets up Insert operations
    QMenu *insertMenu = menuBar()->addMenu(tr("&Insert"));
    insertMenu->addSeparator();
    insertMenu->addAction(rowInsert);
    insertMenu->addAction(colInsert);

    // Sets up Help operations
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addSeparator();
    helpMenu->addAction(functionList);
    helpMenu->addAction(operatorList);
    helpMenu->addAction(aboutLeanSheets);
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanSheet::updateStatus(QTableWidgetItem *item)
{
    if (item && item == table->currentItem())
    {
        statusBar()->showMessage(item->data(Qt::StatusTipRole).toString(), 1000);
        cellLabel->setText(tr("Cell: (%1)").arg(encode_pos(table->row(item), table->column(item))));
    }
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanSheet::updateLineEdit(QTableWidgetItem *item)
{
    if (item != table->currentItem())
        return;
    if (item)
        formulaInput->setText(item->data(Qt::EditRole).toString());
    else
        formulaInput->clear();
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
void LeanSheet::returnPressed()
{
    QString text = formulaInput->text();
    int row = table->currentRow();
    int col = table->currentColumn();
    QTableWidgetItem *item = table->item(row, col);
    if (!item)
        table->setItem(row, col, new LeanItem(text));
    else
        item->setData(Qt::EditRole, text);
    table->viewport()->update();
}

// Sets the cells to empty QStrings.
void LeanSheet::clear()
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int col = 0; col < table->columnCount(); col++)
        {
            QTableWidgetItem *cur = table->item(row, col);
            cur->setText("");
        }
    }
}

// Opens files chosen by the user.
void LeanSheet::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open LeanSheet"), "", tr("LeanSheet (*.lean);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    else
    {
        curFile = new QFile(fileName);
        if (!curFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::information(this, tr("Unable to open this lean"), curFile->errorString());
            return;
        }

        clear();
        QTextStream input(curFile);

        for (int numLine = 0; !input.atEnd(); numLine++)
        {
            if (numLine >= table->rowCount())
                insertRow();

            QString curLine = input.readLine();
            QStringList data = curLine.split(",");

            for (int size = 0; size < ALPHA && data.size() > table->columnCount(); size++)
                insertCol();

            for (int index = 0; index < ALPHA && index < data.size(); index++)
                table->setItem(numLine, index, new LeanItem(data.value(index)));

            input.flush();
        }
        curFile->close();
    }
}

// Saves data to a new or opened file.
void LeanSheet::saveFile()
{
    if (!curFile || !curFile->open(QIODevice::WriteOnly))
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save LeanSheet"), "", tr("LeanSheet (*.lean);;All Files (*)"));
        if (fileName.isEmpty())
            return;
        else
        {
            curFile = new QFile(fileName);
            if (!curFile->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::information(this, tr("Unable to save this lean"), curFile->errorString());
                return;
            }
        }
    }

    QTextStream output(curFile);
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int col = 0; col < table->columnCount(); col++)
        {
            if (col == table->columnCount() - 1)
                output << table->item(row,col)->text();
            else
                output << table->item(row,col)->text() << ",";
        }
        output << "\n";
    }
    curFile->close();
}

// Sets curFile as an unopened file.
void LeanSheet::saveAs()
{
    if (curFile)
        delete curFile;
    curFile = nullptr;
    saveFile();
}

// Inserts a new row into the sheet.
void LeanSheet::insertRow()
{
    table->insertRow(table->rowCount());
    for (int index = 0; index < table->columnCount(); index++)
        table->setItem(table->rowCount() - 1, index, new LeanItem());
}

// Inserts a new column into the sheet.
void LeanSheet::insertCol()
{
    int colSize = table->columnCount();
    QString character(QChar('A' + colSize));

    if (colSize < ALPHA)
    {
    table->insertColumn(colSize);
    table->setHorizontalHeaderItem(colSize, new QTableWidgetItem(character));

    for (int index = 0; index < table->rowCount(); index++)
        table->setItem(index, colSize, new LeanItem());
    }
}

// Sets selected cells to empty QStrings.
void LeanSheet::cut()
{
    foreach (QTableWidgetItem *cur, table->selectedItems())
        cur->setText("");
}

// Stores selected cells into QVector 'copied'.
void LeanSheet::copy()
{
    if (!copied.empty())
        copied.clear();
    foreach (QTableWidgetItem *cur, table->selectedItems())
        copied.append(cur->text());
}

// Stores contents of copied into selected cells.
void LeanSheet::paste()
{
    int index = 0;
    foreach (QTableWidgetItem *cur, table->selectedItems())
    {
        if (!copied.isEmpty() && index < copied.size())
        {
            cur->setText(copied.value(index));
            index++;
        }
        else
            cur->setText("");
    }
}

void LeanSheet::setupContents()
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int col = 0; col < ALPHA && col < table->columnCount(); col++)
            table->setItem(row, col, new LeanItem());
    }
}

/* Below are functions dedicated to the Help menu */

const char *functionText =
        "<HTML>"
        "<p><b>List of Functions:"
        "</b></p>"
        "<ul>"
        "<li><b>sum=</b> Cell/Number Cell/Number</li>"
        "<p>Computes the sum of consecutive cells, a cell and a number, or two numbers."
        "</p>"
        "<li><b>product=</b> Cell/Number Cell/Number</li>"
        "<p>Computes the product of consecutive cells, a cell and a number, or two numbers."
        "</p>"
        "<li><b>sqrt=</b> Cell/Number</li>"
        "<p>Computes the square root of any cell or number."
        "</p>"
        "<li><b>median=</b> Cell Cell</li>"
        "<p>Finds the median value of consecutive cells."
        "</p>"
        "<li><b>min=</b> Cell Cell</li>"
        "<p>Finds the minimum value of consecutive cells."
        "</p>"
        "<li><b>max=</b> Cell Cell</li>"
        "<p>Finds the maximum value of consecutive cells."
        "</p>"
        "<li><b>averagev=</b> Cell Cell</li>"
        "<p>Finds the average of consecutive cells."
        "</p>"
        "<li><b>stdev=</b> Cell Cell</li>"
        "<p>Finds the sample standard deviation of consecutive cells."
        "</p>"
        "</HTML>";

const char *operatorText =
        "<HTML>"
        "<p><b>List of Operators:"
        "</b></p>"
        "<ul>"
        "<li><b>+</b> -> Cell/Number + Cell/Number</li>"
        "<p>Adds any two cells or numbers."
        "</p>"
        "<li><b>-</b> -> Cell/Number - Cell/Number</li>"
        "<p>Subtracts any two cells or numbers."
        "</p>"
        "<li><b>*</b> -> Cell/Number * Cell/Number</li>"
        "<p>Multiplies any two cells or numbers."
        "</p>"
        "<li><b>/</b> -> Cell/Number / Cell/Number</li>"
        "<p>Divides any two cells or numbers."
        "</p>"
        "<li><b>^</b> -> Cell/Number ^ Cell/Number</li>"
        "<p>Raises any cell or number to the power of any cell or number."
        "</p>"
        "</HTML>";

const char *aboutText =
        "<HTML>"
        "<p><b>Welcome To LeanSheets!"
        "</b></p>"
        "<p><b>To save files:</b> click on the <b>File</b> menu and select <b>Save</b>. "
        "This will allow you to create a new file or save your data "
        "in the file previously opened."
        "</p>"
        "<p><b>To save to another file:</b> under <b>File</b> select <b>Save As</b>. "
        "This will allow you to store your data in a completely separate "
        "file from the one previously opened."
        "</p>"
        "<p><b>To open a file:</b> under <b>File</b> select <b>Open</b>. "
        "This will allow you to open a *.lean or correctly parsed "
        "*.txt and *.csv file."
        "</p>"
        "<p>The <b>Edit</b> menu allows you to cut, copy, and paste "
        "single or multiple cells anywhere in the sheet. "
        "<b>Shortcuts:</b> Copy - CTRL+C, Cut - CTRL+X, Paste - CTRL+V"
        "</p>"
        "<p>The <b>Insert</b> menu allows you to insert a new "
        "row or column into the sheet."
        "</p>"
        "<p>To access information regarding <b>Functions</b> or <b>Operators</b> "
        "click the <b>Help</b> menu or select the following "
        "<b>Shortcuts:</b> Functions - CTRL+F, Operators - CTRL+E"
        "</p>"
        "</HTML>";

void LeanSheet::showFunctions()
{
    QMessageBox::about(this, "Function List", functionText);
}

void LeanSheet::showOperators()
{
    QMessageBox::about(this, "Operator List", operatorText);
}

void LeanSheet::showAbout()
{
    QMessageBox::about(this, "About LeanSheets", aboutText);
}

/* Non-member functions */

// Case-insensitive decoder.
void decode_pos(const QString &pos, int *row, int *col)
{
    if (pos.isEmpty())
    {
        *col = -1;
        *row = -1;
    }
    else
    {
        QChar firstChar = pos.at(0).toUpper();
        *col = firstChar.toLatin1() - 'A';
        *row = pos.right(pos.size() - 1).toInt() - 1;
    }
}

/** Copyright (C) 2016 The Qt Company Ltd. **/
QString encode_pos(int row, int col)
{
    return QString(col + 'A') + QString::number(row + 1);
}
