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
#include "csvimportfieldsdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "qxtcsvmodel.h"

#include "configfile.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"

CsvImportFieldsDialogImpl::CsvImportFieldsDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w), fileCodec(0), myFileName("")
{

    setupUi(this);

    model = new QxtCsvModel;

    connect( comboBox_fileCodec, SIGNAL ( currentIndexChanged( int ) ), this, SLOT ( fileCodecChanged( int ) ) );

}


void CsvImportFieldsDialogImpl::exec(QString fileName)
{
    myFileName = fileName;

    QFile cvsFile(fileName);
    QString str;
    if(cvsFile.open(QFile::ReadOnly)) {
        QTextStream stream(&cvsFile);
        str=stream.readLine();

        bool sepertatorFound = false;
        if(str.count(",") > str.count(";")) {
            mySeperator=',';
            sepertatorFound = true;
        }
        if(str.count(";") > str.count(",")) {
            mySeperator=';';
            sepertatorFound = true;
        }

        if(sepertatorFound) {

            qDebug() << "vor reloadCvsFileFields()";
            reloadCvsFileFields();
            qDebug() << "nach reloadCvsFileFields()";
            QDialog::exec();
            qDebug() << "nach Dialog::exec()";
        } else {
            QMessageBox::warning(this, tr("Qupil"),
                                 QString::fromUtf8(tr("Trennerzeichen der CSV-Datei (üblicherweise Komma oder Semikolon) konnte nicht erkannt werden.\n"
                                                   "CSV-Import wird abgebrochen!").toStdString().c_str()),
                                 QMessageBox::Close);
        }
    } else {
        QMessageBox::warning(this, tr("Qupil"),
                             QString::fromUtf8(tr("CSV-Datei konnte nicht geöffnet werden.\n"
                                               "CSV-Import wird abgebrochen!").toStdString().c_str()),
                             QMessageBox::Close);
    }

}

void CsvImportFieldsDialogImpl::accept()
{
    QString lastNewPupilIdString("");
    int i;
    for(i=0; i<model->rowCount(); i++) {

        QString forename("");
        if(!(comboBox_forename->itemData(comboBox_forename->currentIndex()).isNull()) && comboBox_forename->currentIndex() != comboBox_forename->count()-1) {
            if(fileCodec == 0) //utf8
                forename = QString::fromUtf8(model->data(model->index(i, comboBox_forename->currentIndex())).toString().toStdString().c_str());
            else //latin1
                forename = model->data(model->index(i, comboBox_forename->currentIndex())).toString();
        }

        QString surname("");
        if(!(comboBox_surname->itemData(comboBox_surname->currentIndex()).isNull()) && comboBox_surname->currentIndex() != comboBox_surname->count()-1) {
            if(fileCodec == 0) //utf8
                surname = QString::fromUtf8(model->data(model->index(i, comboBox_surname->currentIndex())).toString().toStdString().c_str());
            else
                surname = model->data(model->index(i, comboBox_surname->currentIndex())).toString();
        }

        QString birthday("");
        if(!(comboBox_birthday->itemData(comboBox_birthday->currentIndex()).isNull()) && comboBox_birthday->currentIndex() != comboBox_birthday->count()-1) {

            QString dateString("");
            QString bday = model->data(model->index(i, comboBox_birthday->currentIndex())).toString();
            bool bdayFormatFound = false;
            if(bday.count(".")) {
                dateString = "dd.MM.yyyy";
                bdayFormatFound = true;
            }
            if(bday.count("-")) {
                dateString = "yyyy-MM-dd";
                bdayFormatFound = true;
            }
            if(bdayFormatFound)
                birthday = QDate::fromString(model->data(model->index(i, comboBox_birthday->currentIndex())).toString(), dateString).toString(Qt::ISODate);
        }

        QString street("");
        if(!(comboBox_street->itemData(comboBox_street->currentIndex()).isNull()) && comboBox_street->currentIndex() != comboBox_street->count()-1) {
            if(fileCodec == 0) //utf8
                street = QString::fromUtf8(model->data(model->index(i, comboBox_street->currentIndex())).toString().toStdString().c_str());
            else
                street = model->data(model->index(i, comboBox_street->currentIndex())).toString();
        }

        QString plz("");
        if(!(comboBox_plz->itemData(comboBox_plz->currentIndex()).isNull()) && comboBox_plz->currentIndex() != comboBox_plz->count()-1)
            plz = model->data(model->index(i, comboBox_plz->currentIndex())).toString();

        QString city("");
        if(!(comboBox_city->itemData(comboBox_city->currentIndex()).isNull()) && comboBox_city->currentIndex() != comboBox_city->count()-1) {
            if(fileCodec == 0) //utf8
                city = QString::fromUtf8(model->data(model->index(i, comboBox_city->currentIndex())).toString().toStdString().c_str());
            else
                city = model->data(model->index(i, comboBox_city->currentIndex())).toString();
        }

        QString phone("");
        if(!(comboBox_phone->itemData(comboBox_phone->currentIndex()).isNull()) && comboBox_phone->currentIndex() != comboBox_phone->count()-1)
            phone = model->data(model->index(i, comboBox_phone->currentIndex())).toString();

        QString handy("");
        if(!(comboBox_handy->itemData(comboBox_handy->currentIndex()).isNull()) && comboBox_handy->currentIndex() != comboBox_handy->count()-1)
            handy = model->data(model->index(i, comboBox_handy->currentIndex())).toString();

        QString email("");
        if(!(comboBox_email->itemData(comboBox_email->currentIndex()).isNull()) && comboBox_email->currentIndex() != comboBox_email->count()-1) {
            if(fileCodec == 0) //utf8
                email = QString::fromUtf8(model->data(model->index(i, comboBox_email->currentIndex())).toString().toStdString().c_str());
            else
                email = model->data(model->index(i, comboBox_email->currentIndex())).toString();
        }

        QSqlQuery query(*myW->getMyDb()->getMyPupilDb());
        query.prepare("INSERT INTO pupil ( forename, surname, birthday, firstlessondate, address, email, telefon, handy ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ? )");
        query.addBindValue(forename);
        query.addBindValue(surname);
        query.addBindValue(birthday);
        query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
        query.addBindValue(street+", "+plz+" "+city);
        query.addBindValue(email);
        query.addBindValue(phone);
        query.addBindValue(handy);
        query.exec();
        if (query.lastError().isValid()) qDebug() << "DB Error: 44 - " << query.lastError();
        lastNewPupilIdString = query.lastInsertId().toString();
    }

    if(myW->comboBox_leftListMode->currentIndex() != 1)
        myW->comboBox_leftListMode->setCurrentIndex(1);
    else {
        myW->treeWidget_pupilList->refreshPupilList();
        //refresh menu
        myW->rightTabsChanged(1);
    }

    myW->tabWidget_pupil->setCurrentIndex(2);

    if(myW->treeWidget_pupilList->topLevelItemCount()) {
        // 	find new Pupil to select
        int i;
        for (i=0; i<myW->treeWidget_pupilList->topLevelItemCount(); i++) {

            QTreeWidgetItem *item = myW->treeWidget_pupilList->topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == lastNewPupilIdString) {
                myW->treeWidget_pupilList->setCurrentItem(item);
                break;
            }
        }
    }

    QMessageBox::information(this, QString::fromUtf8(tr("Qupil - CSV Adressbuch Import").toStdString().c_str()),
                             QString::fromUtf8(tr("Das Importieren der Daten aus der CSV-Datei wurde erfolgreich beendet! \nEs wurden %1 Einräge hinzugefügt").arg(model->rowCount()).toStdString().c_str()), QMessageBox::Ok);

    QDialog::accept();
}

void CsvImportFieldsDialogImpl::fileCodecChanged(int codec)
{
    fileCodec = codec;
    reloadCvsFileFields();
}

void CsvImportFieldsDialogImpl::reloadCvsFileFields()
{
    QStringList localeFieldsList;
    if(QString::fromStdString(myConfig->readConfigString("Language")) == "de_DE") {
        localeFieldsList << "Vorname" << "Nachname" << "Geburtstag" << QString::fromUtf8("Straße") << "Postleitzahl" << "Stadt" << "Telefon" << "Mobiltelefon" << "E-Mail";
    }

    QStringList fieldsList;
    model->setSource(myFileName.toStdString().c_str(), true, mySeperator);
    int i;
    for(i=0; i<model->columnCount(); i++) {

        if(fileCodec == 0) //utf8
            fieldsList << QString::fromUtf8(model->headerData(i, Qt::Horizontal).toString().toStdString().c_str());
        else //latin1
            fieldsList << model->headerData(i, Qt::Horizontal).toString();
    }

    comboBox_forename->clear();
    comboBox_surname->clear();
    comboBox_birthday->clear();
    comboBox_street->clear();
    comboBox_plz->clear();
    comboBox_city->clear();
    comboBox_phone->clear();
    comboBox_handy->clear();
    comboBox_email->clear();

    int j;
    for (j = 0; j < fieldsList.size(); j++) {

        comboBox_forename->addItem(fieldsList.at(j), j);
        comboBox_surname->addItem(fieldsList.at(j), j);
        comboBox_birthday->addItem(fieldsList.at(j), j);
        comboBox_street->addItem(fieldsList.at(j), j);
        comboBox_plz->addItem(fieldsList.at(j), j);
        comboBox_city->addItem(fieldsList.at(j), j);
        comboBox_phone->addItem(fieldsList.at(j), j);
        comboBox_handy->addItem(fieldsList.at(j), j);
        comboBox_email->addItem(fieldsList.at(j), j);
    }

    QString dontImportString = tr("!!!Dieses Feld nicht importieren!!!");

    comboBox_forename->addItem(dontImportString);
    comboBox_surname->addItem(dontImportString);
    comboBox_birthday->addItem(dontImportString);
    comboBox_street->addItem(dontImportString);
    comboBox_plz->addItem(dontImportString);
    comboBox_city->addItem(dontImportString);
    comboBox_phone->addItem(dontImportString);
    comboBox_handy->addItem(dontImportString);
    comboBox_email->addItem(dontImportString);

    comboBox_forename->setCurrentIndex(comboBox_forename->findText(localeFieldsList.at(0),Qt::MatchContains));
    comboBox_surname->setCurrentIndex(comboBox_surname->findText(localeFieldsList.at(1),Qt::MatchContains));
    comboBox_birthday->setCurrentIndex(comboBox_birthday->findText(localeFieldsList.at(2),Qt::MatchContains));
    comboBox_street->setCurrentIndex(comboBox_street->findText(localeFieldsList.at(3),Qt::MatchContains));
    comboBox_plz->setCurrentIndex(comboBox_plz->findText(localeFieldsList.at(4),Qt::MatchContains));
    comboBox_city->setCurrentIndex(comboBox_city->findText(localeFieldsList.at(5),Qt::MatchContains));
    comboBox_phone->setCurrentIndex(comboBox_phone->findText(localeFieldsList.at(6),Qt::MatchContains));
    comboBox_handy->setCurrentIndex(comboBox_handy->findText(localeFieldsList.at(7),Qt::MatchContains));
    comboBox_email->setCurrentIndex(comboBox_email->findText(localeFieldsList.at(8),Qt::MatchContains));
}

