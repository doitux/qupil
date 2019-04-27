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
#ifndef PUPILLISTTREEWIDGET_H
#define PUPILLISTTREEWIDGET_H

#include <QtGui>
#include <QtWidgets>
#include <QtCore>

class ConfigFile;
class mainWindowImpl;
/**
	@author Felix Hammer <f.hammer@gmx.de>
*/
class PupilListTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    PupilListTreeWidget(QWidget*);

    ~PupilListTreeWidget();

    void setMyW ( mainWindowImpl* theValue ) {
        myW = theValue;
    }
    void setMyConfig ( ConfigFile* theValue ) {
        myConfig = theValue;
    }

public slots:

    void pupilListSelectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void refreshPupilList();
    void selectFirstItem();
    void callPupilListContextMenu(const QPoint);
    void delCurrentPupil(bool firstItemSelection = true, bool menuRefresh = true);
    void archiveCurrentPupil();
    void archiveAndDelCurrentPupil();

private:
    mainWindowImpl *myW;
    int headerSectionIndent;
    ConfigFile *myConfig;
    QMenu *pupilPopupMenu;
    QAction *delPupil;
    QAction *archivePupil;
    QAction *archiveAndDelPupil;
};

#endif
