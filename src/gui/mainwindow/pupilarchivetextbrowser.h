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
#ifndef PUPILARCHIVETEXTBROWSER_H
#define PUPILARCHIVETEXTBROWSER_H

#include <QtGui>
#include <QtCore>

class ConfigFile;
class mainWindowImpl;
/**
	@author Felix Hammer <f.hammer@gmx.de>
*/
class PupilArchiveTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    PupilArchiveTextBrowser(QWidget*);

    ~PupilArchiveTextBrowser();

    void setMyW ( mainWindowImpl* theValue );
    void setMyConfig ( ConfigFile* theValue );
    void setCurrentPupilArchiveId ( int theValue ) {
        currentPupilArchiveId = theValue;
    }

public slots:

    void init();
    void loadPupilArchiveData();
    void printCurrentDoc();
    void exportCurrentDocToPDF();

private:
    mainWindowImpl *myW;
    int currentPupilArchiveId;
    ConfigFile *myConfig;

};

#endif
