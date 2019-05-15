/***************************************************************************
 *   Copyright (C) 2008 by Felix Hammer   *
 *   f.hammer@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>

#ifdef __APPLE__
#include <QMacStyle>
#endif

#include <QtGui>
#include <QtCore>

#include "configfile.h"
#include "mydbhandler.h"
#include "mainwindowimpl.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );

    a.setStyleSheet("QDialogButtonBox { dialogbuttonbox-buttons-have-icons: 1; dialog-ok-icon: url(:/gfx/dialog_ok_apply.png); dialog-cancel-icon: url(:/gfx/dialog_close.png); dialog-close-icon: url(:/gfx/dialog_close.png);}");

#ifdef _WIN32
    QString font1String("font-family: \"Arial\";");
    a.setStyleSheet("QApplication, QWidget, QDialog { " + font1String + " font-size: 12px; }");
#else
#ifdef __APPLE__
    QString font1String("font-family: \"Lucida Grande\";");
    a.setStyleSheet("QApplication, QWidget, QDialog { " + font1String + " font-size: 11px; }");
#else
//    QString font1String("font-family: \"Nimbus Sans L\";");
//    a.setStyleSheet("QApplication, QWidget, QDialog { " + font1String + " font-size: 12px; }");
#endif

#endif

    //create defaultconfig
    ConfigFile *myConfig = new ConfigFile(argv[0], false);

    //Set translations
    QTranslator qtTranslator;
    qtTranslator.load(QString(":/translation/qt_") + QString::fromStdString(myConfig->readConfigString("Language")));
    a.installTranslator(&qtTranslator);
//
// 	QTranslator translator;
// 	translator.load(QString(myAppDataPath +"translations/pokerth_") + QString::fromStdString(myConfig->readConfigString("Language")));
// 	a.installTranslator(&translator);

#ifdef __APPLE__
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

    //create dbhandler
    myDBHandler *myDB = new myDBHandler(myConfig);

    mainWindowImpl *myW = new mainWindowImpl(myConfig, myDB);
    myW->show();
    myDB->setMyW(myW);
    myW->checkForOldDataStructure();

    int retVal = a.exec();
    return retVal;
}

