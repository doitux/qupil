/***************************************************************************
 *   Copyright (C) 2008 by Felix Hammer   *
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
#include "concertmanagerdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QPrinter>
#include <QtWidgets>
#include <QPrintDialog>
#include <QtSql>

#include "qupil_defs.h"
#include "configfile.h"
#include "mainwindowimpl.h"
#include "addrecitaldialogimpl.h"
#include "addexternalrecitalpiecedialog.h"
#include "selectpiecesdialog.h"
#include "recitaltabs.h"
#include "docviewerdialogimpl.h"

ConcertManagerDialogImpl::ConcertManagerDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w)
{
    setupUi(this);

    managerRecitalMenu = new QMenu;
    addRecitalAction = new QAction(QString::fromUtf8(tr("Hinzufügen ...").toStdString().c_str()), managerRecitalMenu);
    addRecitalAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V, Qt::Key_H));
    removeRecitalAction = new QAction(QString::fromUtf8(tr("Löschen").toStdString().c_str()), managerRecitalMenu);
    removeRecitalAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V, Qt::Key_L));
    removeAndArchiveRecitalAction = new QAction(QString::fromUtf8(tr("Archivieren und Löschen").toStdString().c_str()), managerRecitalMenu);
    removeAndArchiveRecitalAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V, Qt::Key_A));
    managerRecitalMenu->addAction(addRecitalAction);
    managerRecitalMenu->addAction(removeRecitalAction);
    managerRecitalMenu->addAction(removeAndArchiveRecitalAction);

    toolButton_recital->setMenu(managerRecitalMenu);

    managerPieceMenu = new QMenu;
    addPieceAction = new QAction(QString::fromUtf8(tr("Hinzufügen ...").toStdString().c_str()), managerPieceMenu);
    addPieceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W, Qt::Key_H));
    addExtPieceAction = new QAction(QString::fromUtf8(tr("Externen Beitrag hinzufügen...").toStdString().c_str()), managerPieceMenu);
    addExtPieceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W, Qt::Key_E));
    removePieceAction = new QAction(QString::fromUtf8(tr("Löschen").toStdString().c_str()), managerPieceMenu);
    removePieceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W, Qt::Key_L));
    managerPieceMenu->addAction(addPieceAction);
    managerPieceMenu->addAction(addExtPieceAction);
    managerPieceMenu->addAction(removePieceAction);

    toolButton_piece->setMenu(managerPieceMenu);

    myAddRecitalDialog = new AddRecitalDialogImpl;

    archivePopupMenu = new QMenu();
    delArchiveAction = new QAction(QIcon(":/gfx/archive_remove.png"), QString::fromUtf8(tr("Archiveintrag löschen").toStdString().c_str()), archivePopupMenu);
    archivePopupMenu->addAction(delArchiveAction);

    //lade orte aus der Config
    comboBox_piecesByLocationFilter->clear();
    QStringList myLessonLocationStringList;
    std::list<std::string> locationList = myConfig->readConfigStringList("LessonLocationList");
    std::list<std::string>::iterator it2;
    for(it2= locationList.begin(); it2 != locationList.end(); it2++) {
        myLessonLocationStringList << QString::fromUtf8(it2->c_str());
    }
    comboBox_piecesByLocationFilter->insertItem(0, "alle");
    comboBox_piecesByLocationFilter->insertItems(1, myLessonLocationStringList );


    connect( treeWidget_archivSelection, SIGNAL ( customContextMenuRequested(const QPoint) ), this, SLOT ( callRecitalArchiveListContextMenu( const QPoint) ) );
    connect( delArchiveAction, SIGNAL ( triggered() ), this, SLOT ( delCurrentArchive() ) );
    connect(pushButton_createRecitalDocument, SIGNAL (clicked()), this, SLOT (showRecitalDocument() ) );
    connect(addRecitalAction, SIGNAL(triggered(bool)), this, SLOT(addRecital()));
    connect(removeRecitalAction, SIGNAL(triggered(bool)), this, SLOT(removeRecital()));
    connect(removeAndArchiveRecitalAction, SIGNAL(triggered(bool)), this, SLOT(removeAndArchiveRecital()));
    connect(addPieceAction, SIGNAL(triggered(bool)), this, SLOT(addPiece()));
    connect(addExtPieceAction, SIGNAL(triggered(bool)), this, SLOT(addExtPiece()));
    connect(removePieceAction, SIGNAL(triggered(bool)), this, SLOT(removePiece()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentMainTabChanged(int)));
    connect(treeWidget_archivSelection, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(displayCurrentRecitalArchiveEntry(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(pushButton_printCurrentRecitalArchive, SIGNAL(clicked()), this, SLOT(printCurrentArchiveDoc()));
    connect(pushButton_exportCurrentRecitalArchiveToPDF, SIGNAL(clicked()), this, SLOT(exportCurrentArchiveDocToPDF()));
    connect(comboBox_piecesByLocationFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(loadReadyPiecesView()));

}


ConcertManagerDialogImpl::~ConcertManagerDialogImpl() {}


void ConcertManagerDialogImpl::loadReadyPiecesView()
{
    QString locationFilterInsert;
    if(comboBox_piecesByLocationFilter->currentIndex() == 0) {
        locationFilterInsert = "";
    } else {
        locationFilterInsert = QString(" AND l.lessonlocation = '%1'").arg(comboBox_piecesByLocationFilter->currentText());
    }

    qDebug() << locationFilterInsert;

    treeWidget->clear();
    QStringList groupAlreadyDone;
    QSqlQuery query("SELECT l.lessonid, l.lessonname, pc.composer, p.title, p.genre, p.duration, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age, pu.forename, pu.surname, p.cpieceid FROM pupil pu, piece p, lesson l, pupilatlesson pal, piececomposer pc WHERE p.state = 3 AND pal.palid = p.palid AND p.piececomposerid=pc.piececomposerid AND pal.lessonid = l.lessonid AND pal.pupilid= pu.pupilid AND pal.stopdate > date('now')"+locationFilterInsert+" ORDER by age ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 41 - " << query.lastError();
    }
    while(query.next()) {
        //Prüfen ob der Eintrag ein gemeinsamer Gruppeneintrag ist.
        QSqlQuery query1("SELECT count(*) FROM piece p, lesson l, pupilatlesson pal WHERE p.state = 3 AND pal.palid = p.palid AND pal.lessonid = l.lessonid AND pal.stopdate > date('now') AND l.lessonid="+query.value(0).toString()+" AND p.title='"+query.value(3).toString()+"'");
        if (query1.lastError().isValid()) {
            qDebug() << "DB Error: 42 - " << query1.lastError();
        }
        query1.next();
        if(query1.value(0).toInt() > 1) {

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setData(0, Qt::DisplayRole, query.value(2).toString());
            item->setData(0, Qt::UserRole, query.value(10).toString());
            item->setData(1, Qt::DisplayRole, query.value(3).toString());
            item->setData(2, Qt::DisplayRole, query.value(4).toString());
            item->setData(3, Qt::DisplayRole, query.value(5).toString()+" Min.");

            QSqlQuery query2("SELECT pu.forename, pu.surname, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age FROM pupil pu, pupilatlesson pal WHERE pal.pupilid= pu.pupilid AND pal.lessonid = "+query.value(0).toString()+" AND pal.stopdate > date('now') ORDER by age ASC");
            if (query2.lastError().isValid()) {
                qDebug() << "DB Error: 43 - " << query2.lastError();
            }
            QString pupilsString;
            while(query2.next()) {
                pupilsString += query2.value(1).toString()+", "+query2.value(0).toString()+" ("+query2.value(3).toString()+") - "+query2.value(2).toString()+"\n";
            }
            pupilsString = pupilsString.remove(pupilsString.length()-1,1); //remove last <br>
            item->setData(4, Qt::DisplayRole, pupilsString);

            bool sameItemFound = false;
            QTreeWidgetItemIterator it(treeWidget);
            while (*it) {
                if ((*it)->data(0, Qt::UserRole).toString() == query.value(10).toString()) {
                    sameItemFound = true;
                }
                ++it;
            }

            //Only insert a new item
            if(!sameItemFound) {
                treeWidget->addTopLevelItem(item);
            }

            treeWidget->resizeColumnToContents(0);
            treeWidget->resizeColumnToContents(1);
            treeWidget->resizeColumnToContents(2);
            treeWidget->resizeColumnToContents(3);
            treeWidget->resizeColumnToContents(4);

        } else {
            // 			Einzeleintrag
            QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
            item->setData(0, Qt::DisplayRole, query.value(2).toString());
            item->setData(1, Qt::DisplayRole, query.value(3).toString());
            item->setData(2, Qt::DisplayRole, query.value(4).toString());
            item->setData(3, Qt::DisplayRole, query.value(5).toString()+" Min.");
            item->setData(4, Qt::DisplayRole, query.value(9).toString()+", "+query.value(8).toString()+" ("+query.value(7).toString()+") - "+query.value(6).toString());

            treeWidget->resizeColumnToContents(0);
            treeWidget->resizeColumnToContents(1);
            treeWidget->resizeColumnToContents(2);
            treeWidget->resizeColumnToContents(3);
            treeWidget->resizeColumnToContents(4);

        }
    }
    treeWidget->sortByColumn(3, Qt::AscendingOrder);
}


int ConcertManagerDialogImpl::exec()
{
    loadReadyPiecesView();
    loadConcertManager();
    return QDialog::exec();
}


void ConcertManagerDialogImpl::loadConcertManager()
{
    tabWidget_recitals->clear();
    int recitalCounter = 0;
    QSqlQuery query("SELECT * FROM recital ORDER by date ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 192 - " << query.lastError();
    } else {
        while(query.next()) {
            RecitalTabs *tab = new RecitalTabs(this, this);
            tab->setMyConfig(myConfig);
            tabWidget_recitals->addTab(tab, query.value(1).toString());
            tabWidget_recitals->layout();
            //        tabWidget_recitals->setStyleSheet( "QTabBar { font-style: italic; }");
            tab->setRecitalId(query.value(0).toInt());
            tab->setDate(query.value(2).toString());
            tab->setTime(query.value(3).toString());
            tab->setLocation(query.value(4).toString());
            tab->setOrganisator(query.value(5).toString());
            tab->setAccompanist(query.value(6).toString());
            tab->setState(query.value(7).toInt());

            recitalCounter++;
        }
    }

    //Menüs anpassen
    if(recitalCounter) {
        removeRecitalAction->setDisabled(false);
        removeAndArchiveRecitalAction->setDisabled(false);
        pushButton_createRecitalDocument->setDisabled(false);
        addPieceAction->setDisabled(false);
        addExtPieceAction->setDisabled(false);
        if(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getPiecesCount()) {
            removePieceAction->setDisabled(false);
        } else {
            removePieceAction->setDisabled(true);
        }
    } else {
        removeRecitalAction->setDisabled(true);
        removeAndArchiveRecitalAction->setDisabled(true);
        pushButton_createRecitalDocument->setDisabled(true);
        addPieceAction->setDisabled(true);
        addExtPieceAction->setDisabled(true);
        removePieceAction->setDisabled(true);
    }
}


void ConcertManagerDialogImpl::addRecital()
{
    myAddRecitalDialog->exec();
    if(myAddRecitalDialog->result() == QDialog::Accepted) {
        QSqlQuery query;
        query.prepare("INSERT INTO recital (desc, date, time, location, organisator, defaultaccompanist) VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(myAddRecitalDialog->lineEdit_desc->text());
        query.addBindValue(myAddRecitalDialog->dateEdit->date().toString(Qt::ISODate));
        query.addBindValue(myAddRecitalDialog->timeEdit->time().toString("HH:mm"));
        query.addBindValue(myAddRecitalDialog->lineEdit_location->text());
        query.addBindValue(myAddRecitalDialog->lineEdit_organisator->text());
        query.addBindValue(myAddRecitalDialog->lineEdit_accompanist->text());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 182 - " << query.lastError();
        }

        loadConcertManager();
    }
}

void ConcertManagerDialogImpl::removeRecital()
{
    int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Qupil - Vorspiel löschen").toStdString().c_str()),
                                   QString::fromUtf8(tr("Möchten Sie das Vorspiel \"%1\" wirklich löschen?").arg(tabWidget_recitals->tabText(tabWidget_recitals->currentIndex())).toStdString().c_str()),
                                   QMessageBox::Yes | QMessageBox::No);
    if(ret == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM pieceatrecital WHERE recitalid = ?");
        query.addBindValue(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 201 - " << query.lastError();
        }

        query.prepare("DELETE FROM recital WHERE recitalid = ?");
        query.addBindValue(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 202 - " << query.lastError();
        }

        query.prepare("SELECT count(*) FROM recital");
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 227 - " << query.lastError();
        } else {
            query.next();
            if(query.value(0).toInt() == 0) {
                QSqlQuery query2("DELETE FROM externalrecitalpiece");
                query2.exec();
                if (query2.lastError().isValid()) {
                    qDebug() << "DB Error: 228 - " << query2.lastError();
                }
            }
        }

        loadConcertManager();
    }
}

void ConcertManagerDialogImpl::removeAndArchiveRecital()
{
    QSqlQuery query1;
    query1.prepare("SELECT desc, date, time FROM recital WHERE recitalid=?");
    query1.addBindValue(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId());
    query1.exec();
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 216 - " << query1.lastError();
    } else {
        query1.next();

        QSqlQuery query2;
        query2.prepare("SELECT recitalid FROM recitalarchive WHERE desc=?");
        query2.addBindValue(query1.value(1).toString()+"_"+query1.value(2).toString()+" "+query1.value(0).toString());
        query2.exec();
        if (query2.lastError().isValid()) {
            qDebug() << "DB Error: 221 - " << query2.lastError();
        } else {
            QSqlQuery query3;
            if(query2.next()) {
                query3.prepare("REPLACE INTO recitalarchive (recitalid, desc, data) VALUES ((SELECT recitalid FROM recitalarchive WHERE desc=?), ?, ?)");
                query3.addBindValue(query1.value(1).toString()+"_"+query1.value(2).toString()+" "+query1.value(0).toString());
                query3.addBindValue(query1.value(1).toString()+"_"+query1.value(2).toString()+" "+query1.value(0).toString());
                query3.addBindValue(createRecitalDocument()->toHtml());
            } else {
                query3.prepare("INSERT INTO recitalarchive (desc, data) VALUES (?, ?)");
                query3.addBindValue(query1.value(1).toString()+"_"+query1.value(2).toString()+" "+query1.value(0).toString());
                query3.addBindValue(createRecitalDocument()->toHtml());
            }
            query3.exec();
            if (query3.lastError().isValid()) {
                qDebug() << "DB Error: 222 - " << query3.lastError();
            } else {
                removeRecital();
            }

        }
    }
}

void ConcertManagerDialogImpl::addPiece()
{
    SelectPiecesDialog *dlg = new SelectPiecesDialog(this, myConfig);
    dlg->exec(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId());
    if(dlg->result() == QDialog::Accepted) {
        QList<int> list = dlg->getSelectedPieces();
        QListIterator<int> i(list);
        while (i.hasNext()) {
            QSqlQuery query;
            query.prepare("INSERT INTO pieceatrecital (pieceid, recitalid) VALUES (?, ?)");
            query.addBindValue(i.next());
            query.addBindValue(dlg->getCurrentRecitalId());
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 199 - " << query.lastError();
            }
        }
        static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->refreshPieces();
    }
    dlg->deleteLater();
}

void ConcertManagerDialogImpl::addExtPiece()
{
    AddExternalRecitalPieceDialog *dlg = new AddExternalRecitalPieceDialog(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId(), myConfig, this);
    dlg->exec();

    static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->refreshPieces();
}


void ConcertManagerDialogImpl::removePiece()
{
    int currentPieceId = static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getCurrentPieceId();
    if(currentPieceId != -1) {
        QSqlQuery query;
        query.prepare("DELETE FROM pieceatrecital WHERE parid = ?");
        query.addBindValue(currentPieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 200 - " << query.lastError();
        }
        static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->refreshPieces();
    }
}

QTextDocument* ConcertManagerDialogImpl::createRecitalDocument()
{
    QSqlQuery recitalquery;
    recitalquery.prepare("SELECT desc, date, time, location, organisator, defaultaccompanist FROM recital WHERE recitalid = ?");
    recitalquery.addBindValue(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getRecitalId());
    recitalquery.exec();
    if (recitalquery.lastError().isValid()) {
        qDebug() << "DB Error: 203 - " << recitalquery.lastError();
    } else {
        recitalquery.next();
    }

    QString string;
    QAbstractItemModel *model = static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getListModel();
    int i;
    for(i=0; i<model->rowCount(); i++) {
        string += "<tr width='100%'><td>"+model->data(model->index(i,1)).toString()+"</td><td>"+model->data(model->index(i,2)).toString()+"</td><td>"+model->data(model->index(i,3)).toString()+"</td><td>"+model->data(model->index(i,4)).toString()+"</td><td>"+model->data(model->index(i,5)).toString().replace(QString("\n"), QString("<br>"))+"</td>";
    }

    QTextDocument *doc = new QTextDocument;

    doc->setHtml("<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'> \
				 <html> \
				 <head> \
				 <meta content='text/html; charset=utf-8' http-equiv='content-type'> \
				 <title>Qupil - Programm&uuml;bersicht f&uuml;r Veranstaltungen</title> \
			</head> \
			<body> \
			<table style='text-align: left; margin-left: auto; margin-right: auto; width: 100%;' border='0' cellpadding='0' cellspacing='0'> \
			<tr><td align='center' width='100%'><h2>Programm&uuml;bersicht f&uuml;r \""+recitalquery.value(0).toString()+"\"</h2></td></tr> \
			<tr><td >\
			<tr><td width='100%'></td></tr>\
			<tr><td width='100%'>\
			<table cellpadding='1' cellspacing='0' width='100%' style='text-align: left; border-width:0px;'> \
			<tr> \
			<td><b>Datum:</b> "+QDate::fromString(recitalquery.value(1).toString(), Qt::ISODate).toString("dd.MM.yyyy")+"<td>\
		<td><b>Uhrzeit:</b> "+recitalquery.value(2).toString()+" Uhr<td>\
		<td><b>Ort:</b> "+recitalquery.value(3).toString()+"<td>\
		<td><b>Organisation:</b> "+recitalquery.value(4).toString()+"<td>\
		<td><b>Begleitung:</b> "+recitalquery.value(5).toString()+"<td>\
		</tr>\
        </table>\
		</td></tr>\
		<tr><td width='100%'></td></tr>\
		<tr><td width='100%'>\
        <table cellpadding='1' cellspacing='0' width='100%' style='text-align: left; border-width:1px; border-style:solid;'> \
		<tr><th>Komponist</th><th>Werk/Satzbezeichnung</th><th>Genre</th><th>Dauer</th><th>Musiker (Alter) - Instrument</th></tr>"+string+"\
	  </table>\
      </td></tr>\
      <tr><td width='100%'></td></tr>\
      <tr><td width='100%'><b>Reine Spielzeit:</b> "+QString("%1 Min.").arg(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getCompletePiecesDuration())+"</td></tr>\
      <tr><td width='100%'><b>Gesch&auml;tzte Gesamtdauer:</b> "+QString("%1 Min.").arg(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getCompleteAllInAllDuration())+"</td></tr>\
		<tr><td width='100%'></td></tr>\
        <tr><td align='center'><i>Qupil "+RELEASE_STRING+" - &copy;2006-"+QDate::currentDate().toString("yyyy")+" - Felix Hammer - qupil.de</i></td></tr>\
			</table>\
			</table>\
			</body>\
			</html>");

    return doc;
}

void ConcertManagerDialogImpl::showRecitalDocument()
{

    QString fileNameSuggestion;
	fileNameSuggestion += QString(tabWidget_recitals->tabText(tabWidget_recitals->currentIndex())+"_");
    fileNameSuggestion += QString(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getLocation()+"_");
    fileNameSuggestion += QString(static_cast<RecitalTabs*>(tabWidget_recitals->currentWidget())->getDate());
	DocViewerDialogImpl myDocViewerDialog(1, fileNameSuggestion); //Landscape Modus, dateiname
	myDocViewerDialog.exec(createRecitalDocument());

}

void ConcertManagerDialogImpl::currentMainTabChanged(int tab)
{
    if(tab==2) {
        loadRecitalArchive();
    }
}

void ConcertManagerDialogImpl::loadRecitalArchive()
{
    treeWidget_archivSelection->clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM recitalarchive ORDER by desc DESC");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 218 - " << query.lastError();
    } else {
        while(query.next()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget_archivSelection);
            item->setData(0, Qt::DisplayRole, query.value(1).toString());
            item->setData(0, Qt::UserRole, query.value(0).toInt());
        }
        selectArchiveFirstItem();
    }
}

void ConcertManagerDialogImpl::displayCurrentRecitalArchiveEntry(QTreeWidgetItem *current, QTreeWidgetItem*)
{
    if(current) {
        QSqlQuery query;
        query.prepare("SELECT data FROM recitalarchive WHERE recitalid=?");
        query.addBindValue(current->data(0, Qt::UserRole).toInt());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 219 - " << query.lastError();
        } else {
            query.next();
            QTextDocument *doc = new QTextDocument();
            doc->setHtml(query.value(0).toString());
            textBrowser_archiv->setDocument(doc);
        }
    } else {
        textBrowser_archiv->clear();
    }
}

void ConcertManagerDialogImpl::callRecitalArchiveListContextMenu(const QPoint point)
{
    if(treeWidget_archivSelection->topLevelItemCount()) {
        archivePopupMenu->popup(treeWidget_archivSelection->mapToGlobal(point));
    }
}

void ConcertManagerDialogImpl::delCurrentArchive()
{
    QList<QTreeWidgetItem *> selectedItemList = treeWidget_archivSelection->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Qupil - Archiveintrag löschen").toStdString().c_str()),
                                   QString::fromUtf8(tr("Möchten Sie den Archiveintrag \"%1\" wirklich löschen?").arg(selectedItemList.first()->data(0,Qt::DisplayRole).toString().toUtf8().constData()).toStdString().c_str()),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok) {
        QSqlQuery delArchiveQuery("DELETE FROM recitalarchive WHERE recitalid = "+selectedItemIdString);
        if (delArchiveQuery.lastError().isValid()) qDebug() << "DB Error: 220 - " << delArchiveQuery.lastError();

        loadRecitalArchive();
    }
}

void ConcertManagerDialogImpl::selectArchiveFirstItem()
{
    if(treeWidget_archivSelection->topLevelItemCount())
        treeWidget_archivSelection->setCurrentItem(treeWidget_archivSelection->topLevelItem(0));
}

void ConcertManagerDialogImpl::printCurrentArchiveDoc()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Landscape);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument *doc = textBrowser_archiv->document();
        doc->print(printDialog.printer());
    }

}

void ConcertManagerDialogImpl::exportCurrentArchiveDocToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportiere Datei"),
                       QDir::homePath(),
                       tr("PDF Datei (*.pdf)"));
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");

        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printer.setPaperSize(QPrinter::A4);
        printer.setOrientation(QPrinter::Landscape);
        QTextDocument *doc = textBrowser_archiv->document();
        doc->print(&printer);
    }
}

void ConcertManagerDialogImpl::refreshPieceActions(int pieceCounter)
{
    if(pieceCounter) {
        removePieceAction->setDisabled(false);
    } else {
        removePieceAction->setDisabled(true);
    }
}
