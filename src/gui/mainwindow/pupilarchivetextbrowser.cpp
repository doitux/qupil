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
#include "pupilarchivetextbrowser.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"
#include <QtSql>

PupilArchiveTextBrowser::PupilArchiveTextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
}

PupilArchiveTextBrowser::~PupilArchiveTextBrowser()
{
}

void PupilArchiveTextBrowser::init()
{

    connect( myW->pushButton_printCurrentPupilArchive, SIGNAL( clicked() ), this, SLOT( printCurrentDoc() ) );
    connect( myW->pushButton_exportCurrentPupilArchiveToPDF, SIGNAL( clicked() ), this, SLOT( exportCurrentDocToPDF() ) );
}

void PupilArchiveTextBrowser::setMyW ( mainWindowImpl* theValue )
{
    myW = theValue;
}

void PupilArchiveTextBrowser::setMyConfig( ConfigFile* theValue )
{
    myConfig = theValue;
}

void PupilArchiveTextBrowser::loadPupilArchiveData()
{

    QSqlQuery pupilQuery("SELECT data FROM pupilarchive WHERE pupilid = "+QString::number(currentPupilArchiveId));
    if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 90 - " << pupilQuery.lastError();

    while (pupilQuery.next()) {
        QTextDocument *doc = new QTextDocument;
        doc->setHtml(pupilQuery.value(0).toString());
        setDocument(doc);
    }

}

void PupilArchiveTextBrowser::printCurrentDoc()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument *doc = document();
        doc->print(printDialog.printer());
    }

}

void PupilArchiveTextBrowser::exportCurrentDocToPDF()
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
        QTextDocument *doc = document();
        doc->print(&printer);
    }
}
