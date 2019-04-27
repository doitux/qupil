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
#include "docviewerdialogimpl.h"

#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QtSql>
#include <QPrinter>
#include <QPrintDialog>

DocViewerDialogImpl::DocViewerDialogImpl(int o, QString fn) : myOrientation(o), suggestedfileName(fn)
{
    setupUi(this);
    connect( pushButton_print, SIGNAL( clicked() ), this, SLOT( printCurrentDoc() ) );
    connect( pushButton_exportToPdf, SIGNAL( clicked() ), this, SLOT( exportCurrentDocToPDF() ) );
}

DocViewerDialogImpl::~DocViewerDialogImpl() {}


void DocViewerDialogImpl::exec(QTextDocument *myDoc)
{
    textBrowser->setDocument(myDoc);
    QDialog::exec();
}


void DocViewerDialogImpl::printCurrentDoc()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setPageMargins(10, 12, 10 ,12, QPrinter::Millimeter);
    if(myOrientation)
        printer.setOrientation(QPrinter::Landscape);
    else
        printer.setOrientation(QPrinter::Portrait);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument *doc = textBrowser->document();
        doc->print(printDialog.printer());
    }

}

void DocViewerDialogImpl::exportCurrentDocToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportiere Datei"),
                       QDir::homePath()+"/"+suggestedfileName+".pdf",
                       tr("PDF Datei (*.pdf)"));
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");

        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printer.setPaperSize(QPrinter::A4);
        printer.setPageMargins(10, 12, 10 ,12, QPrinter::Millimeter);
        if(myOrientation)
            printer.setOrientation(QPrinter::Landscape);
        else
            printer.setOrientation(QPrinter::Portrait);

        QTextDocument *doc = textBrowser->document();
        doc->print(&printer);
    }
}
