/****************************************************************************
**
** Legal Clause:
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
** LeanSheets
** A simpler spreadsheet.
** By Edgar Perez and Shusanta Bhattarai
**
** For more information about the user interface, click the "About LeanSheets" button under the Help menu.
**
** The mission during the development of LeanSheets centered on providing a simple interface for users to
** store and analyze their data. The program also aims to provide flexibility in how the user's data is managed
** between different workflows. The *.lean extension represents a standard text file, which allows LeanSheets
** to be cross compatible with correctly parsed *.csv and *.txt files. Users also have the option of saving their
** work in *.csv and *.txt files of their choice. The decision to have Qt as our application framework was based
** on its cross-platform nature and it's relatively simple nomenclature of various front end features. Without their
** tremendous documentation, much of the core functionality of LeanSheets would not have been feasible.
** Any functions which were not modified in the making of LeanSheets will contain Qt's respective copyright
** over their definition. Thank you for choosing LeanSheets as your spreadsheet application.
**
****************************************************************************/

#include "leansheets.h"

#include <QApplication>
#include <QLayout>
#include <QScreen>

#define MAX_HEIGHT 25
#define MAX_WIDTH  100

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(leanfiles);
    QApplication app(argc, argv);
    QScreen* myScreen = QGuiApplication::primaryScreen();
    QRect screenGeo = myScreen->geometry();
    LeanSheet newSheet(screenGeo.height() / MAX_HEIGHT, screenGeo.width() / MAX_WIDTH);
    newSheet.show();
    return app.exec();
}
