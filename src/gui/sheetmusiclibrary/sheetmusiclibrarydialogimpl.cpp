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
#include "sheetmusiclibrarydialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "configfile.h"
#include "mainwindowimpl.h"
#include "smlallmodel.h"
#include "smlalldelegate.h"

SheetMusicLibraryDialogImpl::SheetMusicLibraryDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w)
{
    setupUi(this);

    mySmlAllModel = new SmlAllModel();
    mySmlAllModel->setMyW(myW);
    mySmlAllModel->setMyConfig(myConfig);

    connect( pushButton_addNewSheetMusic, SIGNAL (clicked()), this, SLOT (addNewSheetMusic() ) );
    connect( pushButton_delSheetMusic, SIGNAL (clicked()), this, SLOT (delSheetMusic() ) );
    connect( pushButton_rentSheetToPupil, SIGNAL (clicked()), this, SLOT (rentSheetToPupil() ) );
    connect( pushButton_sheetBackFromPupil, SIGNAL (clicked()), this, SLOT (sheetBackFromPupil() ) );
    connect( tabWidget, SIGNAL ( currentChanged(int)), this, SLOT ( tabWidgetChanged(int) ) );

}

SheetMusicLibraryDialogImpl::~SheetMusicLibraryDialogImpl() {}

void SheetMusicLibraryDialogImpl::loadSheetMusicLibrary()
{
    loadAll();
    loadAvailable();
    loadRentToPupil();
    refreshCompleter();
}


void SheetMusicLibraryDialogImpl::loadPupilComboBox()
{
    QSqlQuery query("SELECT surname, forename, pupilid FROM pupil ORDER BY surname ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 156 - " << query.lastError();
    }
    while(query.next()) {

        comboBox_pupils->addItem(QIcon(":/gfx/im-user.svg"), query.value(0).toString()+", "+query.value(1).toString(), query.value(2).toString());
    }
    comboBox_pupils->setSizeAdjustPolicy(QComboBox::AdjustToContents);
}

void SheetMusicLibraryDialogImpl::loadAll()
{

    treeView_all->setModel(mySmlAllModel);
    mySmlAllModel->refresh();
    treeView_all->setItemDelegate(new SmlAllDelegate());
    treeView_all->setColumnWidth(0,60);
    treeView_all->setColumnWidth(1,220);
    treeView_all->setColumnWidth(2,220);
    treeView_all->setColumnWidth(3,100);

    QHeaderView *head = treeView_all->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void SheetMusicLibraryDialogImpl::loadAvailable()
{
    treeWidget_available->clear();
    QSqlQuery query1("SELECT sml.smlid, smla.author, sml.title, smlp.publisher FROM sheetmusiclibrary sml, smlauthor smla, smlpublisher smlp WHERE smla.smlauthorid=sml.author AND smlp.smlpublisherid=sml.publisher AND sml.rentpupilid=-1");
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 157 - " << query1.lastError();
    }
    while(query1.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget_available);
        QString id(query1.value(0).toString());
        int length = id.length();
        int i;
        for(i=length; i < 4; i++) id.prepend("0");
        item->setData(0, Qt::DisplayRole, id);
        item->setData(0, Qt::UserRole, query1.value(0).toString());
        item->setData(1, Qt::DisplayRole, query1.value(1).toString());
        item->setData(2, Qt::DisplayRole, query1.value(2).toString());
        item->setData(3, Qt::DisplayRole, query1.value(3).toString());
    }
    treeWidget_available->setColumnWidth(0,60);
    treeWidget_available->resizeColumnToContents(1);
    treeWidget_available->resizeColumnToContents(2);
    treeWidget_available->resizeColumnToContents(3);
    treeWidget_available->sortByColumn(0, Qt::AscendingOrder);
}

void SheetMusicLibraryDialogImpl::loadRentToPupil()
{
    treeWidget_rentToPupil->clear();
    QSqlQuery query1("SELECT sml.smlid, smla.author, sml.title, smlp.publisher, (p.surname||', '||p.forename) AS name, sml.lastrentdate FROM sheetmusiclibrary sml, smlauthor smla, smlpublisher smlp, pupil p WHERE p.pupilid=sml.rentpupilid AND smla.smlauthorid=sml.author AND smlp.smlpublisherid=sml.publisher");
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 158 - " << query1.lastError();
    }
    while(query1.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget_rentToPupil);
        QString id(query1.value(0).toString());
        int length = id.length();
        int i;
        for(i=length; i < 4; i++) id.prepend("0");
        item->setData(0, Qt::DisplayRole, id);
        item->setData(0, Qt::UserRole, query1.value(0).toString());
        item->setData(1, Qt::DisplayRole, query1.value(1).toString());
        item->setData(2, Qt::DisplayRole, query1.value(2).toString());
        item->setData(3, Qt::DisplayRole, query1.value(3).toString());
        item->setData(4, Qt::DisplayRole, query1.value(4).toString());
        item->setData(5, Qt::DisplayRole, QDate::fromString(query1.value(5).toString(), Qt::ISODate).toString("dd.MM.yyyy"));
    }
    treeWidget_rentToPupil->setColumnWidth(0,60);
    treeWidget_rentToPupil->resizeColumnToContents(1);
    treeWidget_rentToPupil->resizeColumnToContents(2);
    treeWidget_rentToPupil->resizeColumnToContents(3);
    treeWidget_rentToPupil->resizeColumnToContents(4);
    treeWidget_rentToPupil->resizeColumnToContents(5);
    treeWidget_rentToPupil->sortByColumn(0, Qt::AscendingOrder);
}

int SheetMusicLibraryDialogImpl::exec()
{
    retranslateUi(this);
    loadPupilComboBox();
    loadSheetMusicLibrary();
    return QDialog::exec();
}

void SheetMusicLibraryDialogImpl::addNewSheetMusic()
{
    if(lineEdit_author->text().isEmpty() || lineEdit_publisher->text().isEmpty() || lineEdit_title->text().isEmpty()) {
        QMessageBox::warning(this, "Qupil",
                             QString::fromUtf8(tr("You must complete all fields (composer / author, title, publisher) \n"
                                                  "to add the entry!").toStdString().c_str()),
                             QMessageBox::Ok);
    } else {
        QString authorId;
        QSqlQuery query;
        query.prepare("SELECT smlauthorid FROM smlauthor WHERE author=?");
        query.addBindValue(lineEdit_author->text());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 159 - " << query.lastError();
        }
        if(query.next()) {
            authorId = query.value(0).toString();
        } else {
            QSqlQuery query1;
            query1.prepare("INSERT INTO smlauthor (author) VALUES (?)");
            query1.addBindValue(lineEdit_author->text());
            query1.exec();
            if (query1.lastError().isValid()) {
                qDebug() << "DB Error: 160 - " << query1.lastError();
            }
            authorId = query1.lastInsertId().toString();

        }

        QString publisherId;
        QSqlQuery query2;
        query2.prepare("SELECT smlpublisherid FROM smlpublisher WHERE publisher=?");
        query2.addBindValue(lineEdit_publisher->text());
        query2.exec();
        if (query2.lastError().isValid()) {
            qDebug() << "DB Error: 161 - " << query2.lastError();
        }
        if(query2.next()) {
            publisherId = query2.value(0).toString();
        } else {
            QSqlQuery query3;
            query3.prepare("INSERT INTO smlpublisher (publisher) VALUES (?)");
            query3.addBindValue(lineEdit_publisher->text());
            query3.exec();
            if (query3.lastError().isValid()) {
                qDebug() << "DB Error: 162 - " << query3.lastError();
            }
            publisherId = query3.lastInsertId().toString();
        }

        QSqlQuery query4;
        query4.prepare("INSERT INTO sheetmusiclibrary (author, title, publisher, rentpupilid) VALUES ( ? , ? , ? , -1 )");
        query4.addBindValue(authorId.toInt());
        query4.addBindValue(lineEdit_title->text());
        query4.addBindValue(publisherId.toInt());
        query4.exec();
        if (query4.lastError().isValid()) {
            qDebug() << "DB Error: 163 - " << query4.lastError();
        }

        loadSheetMusicLibrary();
    }

}

void SheetMusicLibraryDialogImpl::delSheetMusic()
{
    if(treeView_all->selectionModel()->hasSelection()) {

        QSqlQuery query("DELETE FROM sheetmusiclibrary WHERE smlid="+mySmlAllModel->index(treeView_all->currentIndex().row(), 0).data().toString());
        if (query.lastError().isValid()) qDebug() << "DB Error: 164 - " << query.lastError();

        mySmlAllModel->refresh();
    }
}

void SheetMusicLibraryDialogImpl::rentSheetToPupil()
{
    QList<QTreeWidgetItem *> selectedItemList = treeWidget_available->selectedItems();
    if(!selectedItemList.empty() && comboBox_pupils->count()) {

        QSqlQuery query("UPDATE sheetmusiclibrary SET rentpupilid="+comboBox_pupils->itemData(comboBox_pupils->currentIndex()).toString()+", lastrentdate='"+QDate::currentDate().toString(Qt::ISODate)+"' WHERE smlid="+selectedItemList.first()->data(0,Qt::UserRole).toString());
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 165 - " << query.lastError();
        }

        loadSheetMusicLibrary();
    }
}

void SheetMusicLibraryDialogImpl::refreshCompleter()
{
    QStringList authorList;
    QSqlQuery query("SELECT author FROM smlauthor");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 166 - " << query.lastError();
    }
    while(query.next()) {
        authorList << query.value(0).toString();
    }

    QCompleter *authorCompleter = new QCompleter(authorList, this);
    authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit_author->setCompleter(authorCompleter);


    QStringList publisherList;
    QSqlQuery query1("SELECT publisher FROM smlpublisher");
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 167 - " << query1.lastError();
    }
    while(query1.next()) {
        publisherList << query1.value(0).toString();
    }

    QCompleter *publisherCompleter = new QCompleter(publisherList, this);
    publisherCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit_publisher->setCompleter(publisherCompleter);
}

void SheetMusicLibraryDialogImpl::sheetBackFromPupil()
{
    QList<QTreeWidgetItem *> selectedItemList = treeWidget_rentToPupil->selectedItems();
    if(!selectedItemList.empty()) {

        QSqlQuery query("UPDATE sheetmusiclibrary SET rentpupilid=-1 WHERE smlid="+selectedItemList.first()->data(0,Qt::UserRole).toString());
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 168 - " << query.lastError();
        }

        loadSheetMusicLibrary();
    }
}

void SheetMusicLibraryDialogImpl::tabWidgetChanged(int tab)
{
    switch(tab) {
    case 0:
        loadAll();
        break;
    case 1:
        loadAvailable();
        break;
    case 2:
        loadRentToPupil();
        break;
    }
}



