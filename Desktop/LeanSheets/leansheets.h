#ifndef LEANSHEETS_H
#define LEANSHEETS_H

#include <QMainWindow>
#include <QFile>

class QAction;
class QLabel;
class QLineEdit;
class QToolBar;
class QTableWidgetItem;
class QTableWidget;

class LeanSheet : public QMainWindow
{
    Q_OBJECT

public:

    LeanSheet(int rows, int cols, QWidget *parent = 0);

public slots:
    void updateStatus(QTableWidgetItem *item);
    void updateLineEdit(QTableWidgetItem *item);
    void returnPressed();

    void openFile();
    void saveAs();
    void saveFile();

    void insertRow();
    void insertCol();

    void cut();
    void copy();
    void paste();

    void showAbout();
    void showFunctions();
    void showOperators();

protected:
    void setupContents();
    void clear();
    void setupMenuBar();
    void createActions();

private:
    QToolBar *toolBar;

    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QAction *rowInsert;
    QAction *colInsert;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;

    QAction *functionList;
    QAction *operatorList;
    QAction *aboutLeanSheets;

    QFile *curFile;
    QVector<QString> copied;

    QLabel *cellLabel;
    QTableWidget *table;
    QLineEdit *formulaInput;

};

void decode_pos(const QString &pos, int *row, int *col);
QString encode_pos(int row, int col);

#endif // LEANSHEETS_H
