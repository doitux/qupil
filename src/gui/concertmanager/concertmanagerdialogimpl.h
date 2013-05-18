/***************************************************************************
 *   Copyright (C) 2006 by FThauer FHammer   *
 *   f.thauer@web.de   *
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
#ifndef CONCERTMANAGERDIALOGIMPL_H
#define CONCERTMANAGERDIALOGIMPL_H

#include "ui_concertmanager.h"

class ConfigFile;
class myDBHandler;
class mainWindowImpl;
class AddRecitalDialogImpl;

class ConcertManagerDialogImpl: public QDialog, public Ui::ConcertManagerDialog
{
    Q_OBJECT
public:
    ConcertManagerDialogImpl(ConfigFile *c, mainWindowImpl *w);
    ~ConcertManagerDialogImpl();

public slots:

    void loadConcertManager();
    void loadReadyPiecesView();
    void loadRecitalArchive();
    void exec();
    void addRecital();
    void removeRecital();
    void removeAndArchiveRecital();
    void addPiece();
    void addExtPiece();
    void removePiece();
    QTextDocument* createRecitalDocument();
    void showRecitalDocument();
    void currentMainTabChanged(int);
    void displayCurrentRecitalArchiveEntry(QTreeWidgetItem*,QTreeWidgetItem*);
    void callRecitalArchiveListContextMenu( const QPoint);
    void delCurrentArchive();
    void selectArchiveFirstItem();
    void printCurrentArchiveDoc();
    void exportCurrentArchiveDocToPDF();
    void refreshPieceActions(int);

private:
    ConfigFile *myConfig;
    mainWindowImpl *myW;
    AddRecitalDialogImpl *myAddRecitalDialog;

    QMenu *managerRecitalMenu;
    QMenu *managerPieceMenu;
    QMenu *archivePopupMenu;
    QAction *addRecitalAction;
    QAction *delArchiveAction;
    QAction *removeRecitalAction;
    QAction *removeAndArchiveRecitalAction;
    QAction *addPieceAction;
    QAction *addExtPieceAction;
    QAction *removePieceAction;
};

#endif
