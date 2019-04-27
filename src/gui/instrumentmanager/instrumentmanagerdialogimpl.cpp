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
#include "instrumentmanagerdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "qupil_defs.h"
#include "configfile.h"
#include "mainwindowimpl.h"
#include "docviewerdialogimpl.h"

InstrumentManagerDialogImpl::InstrumentManagerDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w)
{
    setupUi(this);

    myDocViewerDialog = new DocViewerDialogImpl;

    connect( pushButton_createRentInstrumentList, SIGNAL (clicked()), this, SLOT (createRentInstrumentList() ) );

}

InstrumentManagerDialogImpl::~InstrumentManagerDialogImpl() {}

void InstrumentManagerDialogImpl::loadInstrumentManager()
{
    treeWidget_pupilInstrument->clear();

    QString yes(tr("ja"));
    QString no(tr("nein"));
    QSqlQuery query("SELECT (surname||', '||forename) AS name, instrumenttype, instrumentsize, CASE ifrentinstrument WHEN 1 THEN '"+yes+"' WHEN 0 THEN '"+no+"' END, CASE ifinstrumentnextsize WHEN 1 THEN '"+yes+"' WHEN 0 THEN '"+no+"' END FROM pupil WHERE instrumenttype NOT NULL AND instrumentsize NOT NULL");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 45 - " << query.lastError();
    }
    while(query.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget_pupilInstrument);
        item->setData(0, Qt::DisplayRole, query.value(0).toString());
        item->setData(1, Qt::DisplayRole, query.value(1).toString());
        item->setData(2, Qt::DisplayRole, query.value(2).toString());
        item->setData(3, Qt::DisplayRole, query.value(3).toString());
        item->setData(4, Qt::DisplayRole, query.value(4).toString());
    }

    treeWidget_pupilInstrument->resizeColumnToContents(0);
    treeWidget_pupilInstrument->resizeColumnToContents(1);
    treeWidget_pupilInstrument->resizeColumnToContents(2);
    treeWidget_pupilInstrument->resizeColumnToContents(3);
    treeWidget_pupilInstrument->resizeColumnToContents(4);
    treeWidget_pupilInstrument->sortItems(0,Qt::AscendingOrder);

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT instrumenttype, count(instrumenttype) FROM pupil WHERE instrumenttype NOT NULL AND instrumenttype != '' GROUP by instrumenttype");
    model->setHeaderData(0, Qt::Horizontal, tr("Instrument"));
    model->setHeaderData(1, Qt::Horizontal, tr("Anzahl"));
    treeView_InstrumentsNumber->setModel(model);
    treeView_InstrumentsNumber->show();
    treeView_InstrumentsNumber->resizeColumnToContents(0);
    treeView_InstrumentsNumber->resizeColumnToContents(1);

    QSqlQueryModel *model1 = new QSqlQueryModel;
    model1->setQuery("SELECT instrumentsize, count(instrumentsize) FROM pupil WHERE instrumentsize NOT NULL AND instrumentsize != '' GROUP by instrumentsize");
    model1->setHeaderData(0, Qt::Horizontal, QString::fromUtf8(tr("Größe").toStdString().c_str()));
    model1->setHeaderData(1, Qt::Horizontal, tr("Anzahl"));
    treeView_SizesNumber->setModel(model1);
    treeView_SizesNumber->show();
    treeView_SizesNumber->resizeColumnToContents(0);
    treeView_SizesNumber->resizeColumnToContents(1);

    QSqlQuery query1("SELECT count(ifrentinstrument) FROM pupil WHERE ifrentinstrument = 1");
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 46 - " << query1.lastError();
    }
    query1.next();
    label_rentInstruments->setText(query1.value(0).toString());

    QSqlQuery query2("SELECT count(ifinstrumentnextsize) FROM pupil WHERE ifinstrumentnextsize = 1");
    if (query2.lastError().isValid()) {
        qDebug() << "DB Error: 47 - " << query2.lastError();
    }
    query2.next();
    label_nextInstrumentNeeded->setText(query2.value(0).toString());
}

int InstrumentManagerDialogImpl::exec()
{
    loadInstrumentManager();
    QDialog::exec();
}

void InstrumentManagerDialogImpl::createRentInstrumentList()
{
    QString string("");
    QString yes = tr("ja");
    QString no = tr("nein");
    QSqlQuery query("SELECT (surname||', '||forename) AS name, strftime(\"%d.%m.%Y\", birthday), instrumenttype, instrumentsize, rentinstrumentdesc,  CASE ifinstrumentnextsize WHEN 1 THEN '"+yes+"' WHEN 0 THEN '"+no+"' END FROM pupil WHERE instrumenttype NOT NULL AND instrumentsize NOT NULL AND ifrentinstrument=1");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 48 - " << query.lastError();
    }
    while(query.next()) {
        string += "<tr width='100%'><td>"+query.value(0).toString()+"</td><td>"+query.value(1).toString()+"</td><td>"+query.value(2).toString()+"</td><td>"+query.value(3).toString()+"</td><td>"+query.value(4).toString()+"</td><td>"+query.value(5).toString()+"</td></tr>";
    }

    QTextDocument *doc = new QTextDocument;
    doc->setHtml("<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'> \
	<html> \
	<head> \
	<meta content='text/html; charset=ISO-8859-1' http-equiv='content-type'> \
	<title>Qupil - Leihinstrument-Inventurliste</title> \
	</head> \
	<body> \
	<table style='text-align: left; margin-left: auto; margin-right: auto; width: 100%;' border='0' cellpadding='0' cellspacing='0'> \
          <tr><td align='center' width='100%'><h1>Qupil "+QString(RELEASE_STRING)+" - Leihinstrument-Inventurliste</h2></td></tr> \
	  <tr><td >\
	  <tr><td width='100%'></td></tr>\
	  <tr><td width='100%'><h2>Datum: "+QDate::currentDate().toString("dd.MM.yyyy")+"</h2></td></tr>\
	  <tr><td width='100%'></td></tr>\
	  <tr><td width='100%'>\
	    <table cellpadding='1' cellspacing='0' width='100%' style='text-align: left; border-width:1px; border-style:solid;'> \
	      <tr><th>Name</th><th>Geburtsdatum</th><th>Instrument</th><th>Gr&ouml;&szlig;e</th><th>Beschreibung</th><th>n&auml;chste Gr&ouml;&szlig;e erforderlich</th></tr>"+string+"\
	    </table>\
          </td></tr>\
	<tr><td width='100%'></td></tr>\
        <tr><td align='center'><i>Qupil "+RELEASE_STRING+" - &copy;"+COPYRIGHT_TIME_STRING+" - Felix Hammer - www.lehrsaiten.de</i></td></tr>\
	</table>\
        </table>\
        </body>\
	</html>");
    myDocViewerDialog->exec(doc);
}
