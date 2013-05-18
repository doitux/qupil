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
#include "pupilarchivelisttreewidget.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"
#include <QtSql>

PupilArchiveListTreeWidget::PupilArchiveListTreeWidget(QWidget *tab)
    : QTreeWidget(tab), headerSectionIndent(10)
{
    archivePopupMenu = new QMenu();
    delArchive = new QAction(QIcon(":/gfx/archive_remove.png"), QString::fromUtf8(tr("Archiveintrag löschen").toStdString().c_str()), archivePopupMenu);
    archivePopupMenu->addAction(delArchive);
// 	archivePupil = new QAction(QString::fromUtf8(tr("Schüler archivieren").toStdString().c_str()), pupilPopupMenu);
// 	pupilPopupMenu->addAction(archivePupil);
//  	archiveAndDelPupil = new QAction(QString::fromUtf8(tr("Schüler archivieren und löschen").toStdString().c_str()), pupilPopupMenu);
//  	pupilPopupMenu->addAction(archiveAndDelPupil);
//
    connect( this, SIGNAL (currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT (pupilListSelectionChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );
    connect( this, SIGNAL ( customContextMenuRequested(const QPoint) ), this, SLOT ( callPupilArchiveListContextMenu( const QPoint) ) );
    connect( delArchive, SIGNAL ( triggered() ), this, SLOT ( delCurrentArchive() ) );
// 	connect( archivePupil, SIGNAL ( triggered() ), this, SLOT ( archiveCurrentPupil() ) );
// 	connect( archiveAndDelPupil, SIGNAL ( triggered() ), this, SLOT ( archiveAndDelCurrentPupil() ) );
}

PupilArchiveListTreeWidget::~PupilArchiveListTreeWidget()
{
}


void PupilArchiveListTreeWidget::refreshPupilArchiveList( int pupilId )
{

    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    clear();
    setRootIsDecorated(FALSE);
//
    QSqlQuery pupilQuery("SELECT pupilid, forename, surname FROM pupilarchive ORDER BY surname ASC");
    if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 88 - " << pupilQuery.lastError();

    while (pupilQuery.next()) {
        QTreeWidgetItem *pupilItem = new QTreeWidgetItem(this);
        pupilItem->setFirstColumnSpanned ( TRUE );
        pupilItem->setData(0, Qt::UserRole, pupilQuery.value(0).toString());
        pupilItem->setData(0, Qt::DisplayRole, pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString());
        pupilItem->setData(0, Qt::DecorationRole, QIcon(":/gfx/archive_extract.png"));
    }
//
    if(topLevelItemCount()) {
        // 	find Selection
        if(pupilId == -1) {
            int i;
            for (i=0; i<topLevelItemCount(); i++) {

                QTreeWidgetItem *item = topLevelItem(i);
                if(item->data(0, Qt::UserRole).toString() == selectedItemIdString) {
                    setCurrentItem(item);
                    break;
                }
            }
        } else {
            int i;
            for (i=0; i<topLevelItemCount(); i++) {

                QTreeWidgetItem *item = topLevelItem(i);
                if(item->data(0, Qt::UserRole).toInt() == pupilId) {
                    setCurrentItem(item);
                    break;
                }
            }
        }
        myW->textBrowser_pupilArchive->setEnabled(TRUE);
        myW->pushButton_printCurrentPupilArchive->setEnabled(TRUE);
        myW->pushButton_exportCurrentPupilArchiveToPDF->setEnabled(TRUE);
    } else {
        myW->textBrowser_pupilArchive->clear();
        myW->textBrowser_pupilArchive->setDisabled(TRUE);
        myW->pushButton_printCurrentPupilArchive->setDisabled(TRUE);
        myW->pushButton_exportCurrentPupilArchiveToPDF->setDisabled(TRUE);
    }
    //refresh menu
    myW->rightTabsChanged(2);
}

void PupilArchiveListTreeWidget::pupilListSelectionChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{

    if(!selectedItems().empty()) {
        myW->textBrowser_pupilArchive->setCurrentPupilArchiveId(current->data(0, Qt::UserRole).toInt());
        myW->textBrowser_pupilArchive->loadPupilArchiveData();
    }

}

void PupilArchiveListTreeWidget::selectFirstItem()
{

    if(topLevelItemCount()) setCurrentItem(topLevelItem(0));
}

void PupilArchiveListTreeWidget::callPupilArchiveListContextMenu(const QPoint point)
{
    if(topLevelItemCount()) {
        archivePopupMenu->popup(mapToGlobal(point));
    }
}

void PupilArchiveListTreeWidget::delCurrentArchive()
{
    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Qupil - Archiveintrag löschen").toStdString().c_str()),
                                   QString::fromUtf8(tr("Möchten Sie den Archiveintrag \"%1\" wirklich löschen?").arg(selectedItemList.first()->data(0,Qt::DisplayRole).toString().toUtf8().constData()).toStdString().c_str()),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok) {
        QSqlQuery delArchiveQuery("DELETE FROM pupilarchive WHERE pupilid = "+selectedItemIdString, *myW->getMyDb()->getMyPupilDb());
        if (delArchiveQuery.lastError().isValid()) qDebug() << "DB Error: 89 - " << delArchiveQuery.lastError();

        refreshPupilArchiveList();
        selectFirstItem();

        //refresh menu
        myW->rightTabsChanged(2);
    }
}

