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
#include "pupiltabwidget.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"
#include <QtSql>
#include "palnotesmodel.h"
#include "palpiecesmodel.h"
#include "pupilcontactivitymodel.h"
#include "pupilsingularactivitymodel.h"
#include "palpiecesdelegate.h"
#include "palnotesdelegate.h"
#include "pupilcontactivitydelegate.h"
#include "pupilsingularactivitydelegate.h"

PupilTabWidget::PupilTabWidget(QWidget *tab)
    : QTabWidget(tab), saveOk(true)
{
    myPalNotesModel = new PalNotesModel();
    myPalPiecesModel = new PalPiecesModel();
    myPupilSingularActivityModel = new PupilSingularActivityModel();
    myPupilContActivityModel = new PupilContActivityModel();
}

PupilTabWidget::~PupilTabWidget()
{
}

void PupilTabWidget::init()
{
    connect( myW->pushButton_addPalPiece, SIGNAL( clicked() ), this, SLOT( addNewPiece() ) );
    connect( myW->pushButton_delPalPiece, SIGNAL( clicked() ), this, SLOT( delPiece() ) );
    connect( myW->pushButton_addPalNotes, SIGNAL( clicked() ), this, SLOT( addNewNote() ) );
    connect( myW->pushButton_delPalNotes, SIGNAL( clicked() ), this, SLOT( delNote() ) );
    connect( myW->pushButton_addContinousActivity, SIGNAL( clicked() ), this, SLOT( addNewContActivity() ) );
    connect( myW->pushButton_delContinousActivity, SIGNAL( clicked() ), this, SLOT( delContActivity() ) );
    connect( myW->pushButton_addSingularActivity, SIGNAL( clicked() ), this, SLOT( addNewSingularActivity() ) );
    connect( myW->pushButton_delSingularActivity, SIGNAL( clicked() ), this, SLOT( delSingularActivity() ) );
    connect( myW->dateEdit_pupilFirstLesson, SIGNAL( dateChanged ( QDate ) ), this, SLOT ( pupilFirstLessonChanged( QDate ) ) );
    connect( myW->dateEdit_pupilBirthday, SIGNAL( dateChanged ( QDate ) ), this, SLOT ( pupilBirthdayChanged( QDate ) ) );
    connect( myW->pushButton_pupilSavePersonalData, SIGNAL( clicked() ), this, SLOT( savePupilPersonalData() ) );
    connect( myW->comboBox_palPiecesSelection, SIGNAL ( currentIndexChanged( int ) ), this, SLOT ( piecesSelectionChanged( int ) ) );
    connect( myW->comboBox_palNotesSelection, SIGNAL ( currentIndexChanged( int ) ), this, SLOT ( notesSelectionChanged( int ) ) );
    connect( myW->pushButton_sheetBackFromPupil, SIGNAL (clicked()), this, SLOT (sheetBackFromPupil() ) );
    connect( myW->pushButton_boldNotesText, SIGNAL (clicked()), this, SLOT( notesTextBold()));
    connect( myW->pushButton_italicNotesText, SIGNAL (clicked()), this, SLOT( notesTextItalic()));
    connect( myW->pushButton_underlinedNotesText, SIGNAL (clicked()), this, SLOT( notesTextUnderlined()));
    connect( myW->pushButton_notesTextColor, SIGNAL (clicked()), this, SLOT( notesTextColor()));
    connect( myW->textEdit_palNotes, SIGNAL (selectionChanged()), this, SLOT( palNotesEditSelectionChanged()));

}

void PupilTabWidget::loadComboBoxItems()
{

    myW->comboBox_pupilInstrument->clear();
    myW->comboBox_pupilInstrumentSize->clear();
    myW->comboBox_palPieceState->clear();
    myW->comboBox_palPieceGenre->clear();
    myW->comboBox_palPiecesSelection->clear();
    myW->comboBox_palNotesSelection->clear();

    QStringList myInstrumentStringList;
    std::list<std::string> instrumentList = myConfig->readConfigStringList("PupilInstrumentList");
    std::list<std::string>::iterator it;
    for(it= instrumentList.begin(); it != instrumentList.end(); it++) {
        myInstrumentStringList << QString::fromUtf8(it->c_str());
    }

    QStringList myInstrumentSizeStringList;
    std::list<std::string> instrumentSizeList = myConfig->readConfigStringList("PupilInstrumentSizeList");
    std::list<std::string>::iterator it1;
    for(it1= instrumentSizeList.begin(); it1 != instrumentSizeList.end(); it1++) {
        myInstrumentSizeStringList << QString::fromUtf8(it1->c_str());
    }

    QStringList myStateStringList;
    myStateStringList << tr("Planned") << tr("In Progress") << tr("Paused") << tr("Ready for Concert") << tr("Finished");

    QStringList myGenreStringList;
    std::list<std::string> genreList = myConfig->readConfigStringList("PalPiecesGenreList");
    std::list<std::string>::iterator it3;
    for(it3= genreList.begin(); it3 != genreList.end(); it3++) {
        myGenreStringList << QString::fromUtf8(it3->c_str());
    }

    myW->comboBox_pupilInstrument->insertItems ( 0, myInstrumentStringList );
    myW->comboBox_pupilInstrumentSize->insertItems ( 0, myInstrumentSizeStringList );
    myW->comboBox_palPieceState->insertItems ( 0, myStateStringList );
    myW->comboBox_palPieceGenre->insertItems ( 0, myGenreStringList );

    //lessonnames for inactive lessons
    QSqlQuery palLessonNameQuery("SELECT pal.palid, lln.lessonname FROM pupilatlesson pal, lastlessonname lln WHERE pal.llnid = lln.llnid AND pal.pupilid = "+QString::number(currentPupilId,10)+" AND pal.stopdate <= '"+QDate::currentDate().toString(Qt::ISODate)+"'");
    if (palLessonNameQuery.lastError().isValid()) qDebug() << "DB Error: 108 - " << palLessonNameQuery.lastError();
    while (palLessonNameQuery.next()) {
        myW->comboBox_palPiecesSelection->addItem(palLessonNameQuery.value(1).toString()+" "+tr("(inactive)"), palLessonNameQuery.value(0).toString());
        // 		qDebug() << myW->comboBox_palPiecesSelection->findData(palLessonNameQuery.value(0).toString());
        //TODO 		myW->comboBox_palPiecesSelection->setItemData(myW->comboBox_palPiecesSelection->findData(palLessonNameQuery.value(0).toString()), 29, Qt::UserRole+1);
        // 		qDebug() << myW->comboBox_palPiecesSelection->itemData(myW->comboBox_palPiecesSelection->findData(palLessonNameQuery.value(0).toString(), Qt::UserRole+1)).toInt();
        // 		myW->comboBox_palPiecesSelection->addItem("test", 20);
        // 		qDebug() << myW->comboBox_palPiecesSelection->findData(20);
        // 		myW->comboBox_palPiecesSelection->setItemData(myW->comboBox_palPiecesSelection->findData(20), 89, Qt::UserRole-1);
        // 		qDebug() << myW->comboBox_palPiecesSelection->itemData(myW->comboBox_palPiecesSelection->findData(20, Qt::UserRole-1)).toInt();
        myW->comboBox_palNotesSelection->addItem(palLessonNameQuery.value(1).toString()+" "+tr("(inactive)"), palLessonNameQuery.value(0).toString());
        // 	TODO	myW->comboBox_palNotesSelection->setItemData(myW->comboBox_palNotesSelection->findData(palLessonNameQuery.value(0).toString()), 0, 33);
    }
    //lessonnames for active lessons
    QSqlQuery palLessonNameQuery1("SELECT pal.palid, l.lessonname FROM pupilatlesson pal, lesson l WHERE pal.pupilid = "+QString::number(currentPupilId,10)+" AND pal.lessonid = l.lessonid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"'");
    if (palLessonNameQuery1.lastError().isValid()) qDebug() << "DB Error: 109 - " << palLessonNameQuery1.lastError();
    while (palLessonNameQuery1.next()) {
        myW->comboBox_palPiecesSelection->addItem(palLessonNameQuery1.value(1).toString(), palLessonNameQuery1.value(0).toString());
        // 		TODO myW->comboBox_palPiecesSelection->setItemData(myW->comboBox_palPiecesSelection->findData(palLessonNameQuery1.value(0).toString()), 1, 33);
        myW->comboBox_palNotesSelection->addItem(palLessonNameQuery1.value(1).toString(), palLessonNameQuery1.value(0).toString());
        // 		TODO myW->comboBox_palNotesSelection->setItemData(myW->comboBox_palNotesSelection->findData(palLessonNameQuery1.value(0).toString()), 1, 33);
    }

    if(myW->comboBox_palPiecesSelection->count())
        myW->tab_palPieces->setEnabled(true);
    else
        myW->tab_palPieces->setDisabled(true);

    if(myW->comboBox_palNotesSelection->count())
        myW->tab_palNotes->setEnabled(true);
    else
        myW->tab_palNotes->setDisabled(true);

}

void PupilTabWidget::setMyW ( mainWindowImpl* theValue )
{
    myW = theValue;
    myPalNotesModel->setMyW(myW);
    myPalPiecesModel->setMyW(myW);
    myPupilSingularActivityModel->setMyW(myW);
    myPupilContActivityModel->setMyW(myW);
}

void PupilTabWidget::setMyConfig( ConfigFile* theValue )
{
    myConfig = theValue;
    myPalPiecesModel->setMyConfig(myConfig);
    myPalNotesModel->setMyConfig(myConfig);
    myPupilSingularActivityModel->setMyConfig(myConfig);
    myPupilContActivityModel->setMyConfig(myConfig);
}

void PupilTabWidget::loadPupilData()
{

    loadComboBoxItems();
    loadPupilPersonalData();
    loadPupilActivity();
    loadPupilRentSheetMusic();

}

void PupilTabWidget::loadPupilPersonalData()
{

    QSqlQuery query("SELECT forename, surname, address, email, telefon, handy, birthday, notes, fathername, fatherjob, fathertelefon, mothername, motherjob, mothertelefon, firstlessondate, instrumenttype, instrumentsize, ifinstrumentnextsize, ifrentinstrument, rentinstrumentdesc, rentinstrumentstartdate, recitalinterval, ensembleactivityrequested FROM pupil WHERE pupilid="+QString::number(currentPupilId,10), *myW->getMyDb()->getMyPupilDb());
    if (query.lastError().isValid()) qDebug() << "DB Error: 110 - " << query.lastError();
    query.next();

    myW->lineEdit_pupilForename->setText(query.value(0).toString());
    myW->lineEdit_pupilSurname->setText(query.value(1).toString());
    myW->lineEdit_pupilAddress->setText(query.value(2).toString());
    myW->lineEdit_pupilEmail->setText(query.value(3).toString());
    myW->lineEdit_pupilPhone->setText(query.value(4).toString());
    myW->lineEdit_pupilHandy->setText(query.value(5).toString());
    if(query.value(6).toString().isEmpty() || query.value(6).toString() == " ") {
        myW->dateEdit_pupilBirthday->setDate(QDate::fromString("2000-01-01", Qt::ISODate));
    } else {
        myW->dateEdit_pupilBirthday->setDate(QDate::fromString(query.value(6).toString(), Qt::ISODate));
    }
    calcPupilAge(query.value(6).toString());
    myW->textEdit_pupilNotes->setPlainText(query.value(7).toString());
    myW->lineEdit_pupilFatherName->setText(query.value(8).toString());
    myW->lineEdit_pupilFatherJob->setText(query.value(9).toString());
    myW->lineEdit_pupilFatherPhone->setText(query.value(10).toString());
    myW->lineEdit_pupilMotherName->setText(query.value(11).toString());
    myW->lineEdit_pupilMotherJob->setText(query.value(12).toString());
    myW->lineEdit_pupilMotherPhone->setText(query.value(13).toString());
    myW->dateEdit_pupilFirstLesson->setDate(QDate::fromString(query.value(14).toString(), Qt::ISODate));
    calcSinceFirstLesson(query.value(14).toString());

    int pos = myW->comboBox_pupilInstrument->findText(query.value(15).toString(), Qt::MatchExactly );
    if(pos == -1) {
        myW->comboBox_pupilInstrument->addItem(query.value(15).toString());
        pos = myW->comboBox_pupilInstrument->findText(query.value(15).toString(), Qt::MatchExactly );
    }
    myW->comboBox_pupilInstrument->setCurrentIndex(pos);

    int pos1 = myW->comboBox_pupilInstrumentSize->findText(query.value(16).toString(), Qt::MatchExactly );
    if(pos1 == -1) {
        myW->comboBox_pupilInstrumentSize->addItem(query.value(16).toString());
        pos1 = myW->comboBox_pupilInstrumentSize->findText(query.value(16).toString(), Qt::MatchExactly );
    }
    myW->comboBox_pupilInstrumentSize->setCurrentIndex(pos1);
    myW->checkBox_pupilNeedsNextInstrumentSize->setChecked(query.value(17).toInt());

    if(query.value(18).toInt()) {
        myW->groupBox_pupilHasRentInstrument->setChecked(true);
        myW->lineEdit_pupilRentInstrumentNumber->setText(query.value(19).toString());
        myW->dateEdit_pupilRentInstrumentSince->setDate(QDate::fromString(query.value(20).toString(), Qt::ISODate));
    } else {
        myW->groupBox_pupilHasRentInstrument->setChecked(false);
        myW->lineEdit_pupilRentInstrumentNumber->clear();
        myW->dateEdit_pupilRentInstrumentSince->setDate(QDate::fromString("0000-00-00", Qt::ISODate));
    }
    myW->comboBox_pupilRecitalInterval->setCurrentIndex(query.value(21).toInt());
    myW->checkBox_ensembleActivityRequested->setChecked(query.value(22).toInt());
}

void PupilTabWidget::savePupilPersonalData()
{

    saveOk = true;

    QSqlQuery query(*myW->getMyDb()->getMyPupilDb());
    query.prepare("UPDATE pupil SET forename = ?, surname = ?, address = ?, email = ?, telefon = ?, handy = ?, birthday = ?, notes = ?, fathername = ?, fatherjob = ?, fathertelefon = ?, mothername = ?, motherjob = ?, mothertelefon = ?, firstlessondate = ?, instrumenttype = ?, instrumentsize = ?, ifinstrumentnextsize = ?, ifrentinstrument = ?, rentinstrumentdesc = ?, rentinstrumentstartdate = ?, recitalinterval = ?, ensembleactivityrequested = ? WHERE pupilid="+QString::number(currentPupilId,10));

    query.addBindValue(myW->lineEdit_pupilForename->text());
    query.addBindValue(myW->lineEdit_pupilSurname->text());
    query.addBindValue(myW->lineEdit_pupilAddress->text());
    query.addBindValue(myW->lineEdit_pupilEmail->text());
    query.addBindValue(myW->lineEdit_pupilPhone->text());
    query.addBindValue(myW->lineEdit_pupilHandy->text());
    query.addBindValue(myW->dateEdit_pupilBirthday->date().toString(Qt::ISODate));
    query.addBindValue(myW->textEdit_pupilNotes->toPlainText());
    query.addBindValue(myW->lineEdit_pupilFatherName->text());
    query.addBindValue(myW->lineEdit_pupilFatherJob->text());
    query.addBindValue(myW->lineEdit_pupilFatherPhone->text());
    query.addBindValue(myW->lineEdit_pupilMotherName->text());
    query.addBindValue(myW->lineEdit_pupilMotherJob->text());
    query.addBindValue(myW->lineEdit_pupilMotherPhone->text());
    query.addBindValue(myW->dateEdit_pupilFirstLesson->date().toString(Qt::ISODate));
    query.addBindValue(myW->comboBox_pupilInstrument->currentText());
    query.addBindValue(myW->comboBox_pupilInstrumentSize->currentText());
    if(myW->checkBox_pupilNeedsNextInstrumentSize->isChecked())
        query.addBindValue(1);
    else
        query.addBindValue(0);

    if(myW->groupBox_pupilHasRentInstrument->isChecked()) {
        query.addBindValue(1);
        query.addBindValue(myW->lineEdit_pupilRentInstrumentNumber->text());
        query.addBindValue(myW->dateEdit_pupilRentInstrumentSince->date().toString(Qt::ISODate));
    } else {
        query.addBindValue(0);
        query.addBindValue(QString(""));
        query.addBindValue(QString(""));
    }
    query.addBindValue(myW->comboBox_pupilRecitalInterval->currentIndex());
    if(myW->checkBox_ensembleActivityRequested->isChecked())
        query.addBindValue(1);
    else
        query.addBindValue(0);

    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 111 - " << query.lastError();

    if(myW->comboBox_leftListMode->currentIndex() == 0) myW->treeWidget_timeTable->refreshTimeTable();
    if(myW->comboBox_leftListMode->currentIndex() == 1) myW->treeWidget_pupilList->refreshPupilList();
}

void PupilTabWidget::calcPupilAge(QString birthdayString)
{

    //calculate age
    if (birthdayString != "") {
        int daysFromBirthday = QDate::currentDate().daysTo(QDate::fromString(birthdayString, Qt::ISODate));
        int years = daysFromBirthday/365;
        myW->label_pupilAge->setText(QString( "%1" ).arg( years ).remove("-")+" "+tr("Years"));
    } else {
        myW->label_pupilAge->setText("");
    }
}

void PupilTabWidget::calcSinceFirstLesson(QString firstLessonString)
{

    //calculate years since first lesson
    if (firstLessonString != "") {
        int daysFromFirstLesson = QDate::currentDate().daysTo(QDate::fromString(firstLessonString, Qt::ISODate));
        int lessonyears = daysFromFirstLesson/365;
        myW->label_pupilWholeLessonTime->setText(QString( "%1" ).arg( lessonyears ).remove("-")+" "+tr("Years"));
    } else {
        myW->label_pupilWholeLessonTime->setText("");
    }
}

void PupilTabWidget::pupilBirthdayChanged(QDate date)
{
    calcPupilAge(date.toString(Qt::ISODate));
}
void PupilTabWidget::pupilFirstLessonChanged(QDate date)
{
    calcSinceFirstLesson(date.toString(Qt::ISODate));
}

void PupilTabWidget::loadPupilActivity()
{
    myW->dateEdit_singularActivityDate->setDate(QDate::currentDate());
    myW->dateEdit_continousActivityStartdate->setDate(QDate::currentDate());

    myPupilSingularActivityModel->setCurrentPupilId(currentPupilId);
    myW->treeView_singularActivity->setModel(myPupilSingularActivityModel);
    myPupilSingularActivityModel->refresh();
    myW->treeView_singularActivity->setItemDelegate(new PupilSingularActivityDelegate());
    myW->treeView_singularActivity->hideColumn(0);
    myW->treeView_singularActivity->resizeColumnToContents(1);
    QHeaderView *head = myW->treeView_singularActivity->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);

    myPupilContActivityModel->setCurrentPupilId(currentPupilId);
    myW->treeView_continousActivity->setModel(myPupilContActivityModel);
    myPupilContActivityModel->refresh();
    myW->treeView_continousActivity->setItemDelegate(new PupilContActivityDelegate());
    myW->treeView_continousActivity->hideColumn(0);
    myW->treeView_continousActivity->resizeColumnToContents(1);
    myW->treeView_continousActivity->setColumnWidth(2,110);
    myW->treeView_continousActivity->setColumnWidth(3,80);
    myW->treeView_continousActivity->resizeColumnToContents(4);
    myW->treeView_continousActivity->resizeColumnToContents(5);

    QHeaderView *head1 = myW->treeView_continousActivity->header();
    head1->setSectionResizeMode(QHeaderView::ResizeToContents);

    //HACK because of regression in Qt-5.x where hidden columns are shown after editing finished via delegate and scrollState is not saved during editing
    QAbstractItemDelegate *myContinousActivityItemDelegate = myW->treeView_continousActivity->itemDelegate();
    connect( myContinousActivityItemDelegate, SIGNAL ( commitData(QWidget *) ), this, SLOT ( activityRestoreViewLayout() ) );
    connect( myContinousActivityItemDelegate, SIGNAL ( editorCreated()), this, SLOT ( activitySaveScrollState() ) );

    QAbstractItemDelegate *mySingularActivityItemDelegate = myW->treeView_singularActivity->itemDelegate();
    connect( mySingularActivityItemDelegate, SIGNAL ( commitData(QWidget *) ), this, SLOT ( activityRestoreViewLayout() ) );
    connect( mySingularActivityItemDelegate, SIGNAL ( editorCreated()), this, SLOT ( activitySaveScrollState() ) );

}

void PupilTabWidget::loadPalNotes( int palId )
{
    myW->dateEdit_palNotes->setDate(QDate::currentDate());

    QSqlQuery query("SELECT startdate FROM pupilatlesson WHERE palid= ?");
    if(palId == -1) {
        myPalNotesModel->setCurrentPalId(currentPalId);
        query.addBindValue(currentPalId);
    } else {
        myPalNotesModel->setCurrentPalId(palId);
        query.addBindValue(palId);
    }

    myW->treeView_palNotes->setModel(myPalNotesModel);
    myPalNotesModel->refresh();

    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 112 - " << query.lastError();
    query.next();
    QDate startDate(QDate::fromString( query.value(0).toString(), Qt::ISODate));

    myW->treeView_palNotes->setItemDelegate(new PalNotesDelegate(startDate, myConfig));
    myW->treeView_palNotes->hideColumn(0);
    myW->treeView_palNotes->hideColumn(1);

    QHeaderView *head = myW->treeView_palNotes->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);

    myPalNotesSelectionModel = myW->treeView_palNotes->selectionModel();
    connect( myPalNotesSelectionModel, SIGNAL (currentChanged( const QModelIndex &, const QModelIndex & )), this, SLOT ( notesItemSelected( const QModelIndex &, const QModelIndex & ) ) );

    //HACK because of regression in Qt-5.x where hidden columns are shown after editing finished via delegate and scrollState is not saved during editing
    PalNotesDelegate *myPalNotesItemDelegate =  qobject_cast<PalNotesDelegate *> (myW->treeView_palNotes->itemDelegate());
    connect( myPalNotesItemDelegate, SIGNAL ( commitData(QWidget *)), this, SLOT ( palNotesRestoreViewLayout() ) );
    connect( myPalNotesItemDelegate, SIGNAL ( editorCreated()), this, SLOT ( palNotesSaveScrollState() ) );
}

void PupilTabWidget::loadPalPieces( int palId )
{

    QSqlQuery query("SELECT startdate FROM pupilatlesson WHERE palid= ?");
    if(palId == -1) {
        myPalPiecesModel->setCurrentPalId(currentPalId);
        query.addBindValue(currentPalId);
    } else {
        myPalPiecesModel->setCurrentPalId(palId);
        query.addBindValue(palId);
    }

    myW->treeView_palPieces->setModel(myPalPiecesModel);
    myPalPiecesModel->refresh();
    myW->treeView_palPieces->hideColumn(0);
    myW->treeView_palPieces->hideColumn(1);

    QStringList myStateStringList;
    myStateStringList << tr("Planned") << tr("In Progress") << tr("Paused") << tr("Ready for Concert") << tr("Finished");

    QStringList myGenreStringList;
    std::list<std::string> genreList = myConfig->readConfigStringList("PalPiecesGenreList");
    std::list<std::string>::iterator it1;
    for(it1= genreList.begin(); it1 != genreList.end(); it1++) {
        myGenreStringList << QString::fromUtf8(it1->c_str());
    }

    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 113 - " << query.lastError();
    query.next();
    QDate startDate(QDate::fromString( query.value(0).toString() ,Qt::ISODate));

    myW->treeView_palPieces->setItemDelegate(new PalPiecesDelegate(myGenreStringList, myStateStringList, startDate));
    myW->treeView_palPieces->setColumnWidth(3,80);
    myW->treeView_palPieces->setColumnWidth(5,80);
    myW->treeView_palPieces->setColumnWidth(6,80);
    myW->treeView_palPieces->setColumnWidth(7,110);
    QHeaderView *head = myW->treeView_palPieces->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);

    refreshPiecesComposerCompleter();

    //HACK because of regression in Qt-5.x where hidden columns are shown after editing finished via delegate and scrollState is not saved during editing
    QAbstractItemDelegate *myPalPiecesItemDelegate = myW->treeView_palPieces->itemDelegate();
    connect( myPalPiecesItemDelegate, SIGNAL ( commitData(QWidget *) ), this, SLOT ( palPiecesRestoreViewLayout() ) );
    connect( myPalPiecesItemDelegate, SIGNAL ( editorCreated() ), this, SLOT ( palPiecesSaveScrollState() ) );
}

void PupilTabWidget::addNewPiece()
{

    //do not add anything while pieces being edited

    //zuerst Komponist suchen oder hinzufügen
    if(myW->lineEdit_newPieceComposer->text().isEmpty() || myW->lineEdit_newPieceTitle->text().isEmpty()) {
        QMessageBox::warning(this, "Qupil",
                             QString::fromUtf8(tr("You have to complete at least the fields \"Composer\" and \"Title\" to add the entry!").toStdString().c_str()),
                             QMessageBox::Ok);
    } else {
        QString authorId;
        QSqlQuery query;
        query.prepare("SELECT piececomposerid FROM piececomposer WHERE composer=?");
        query.addBindValue(myW->lineEdit_newPieceComposer->text());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 183 - " << query.lastError();
        }
        if(query.next()) {
            authorId = query.value(0).toString();
        } else {
            query.prepare("INSERT INTO piececomposer (composer) VALUES (?)");
            query.addBindValue(myW->lineEdit_newPieceComposer->text());
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 184 - " << query.lastError();
            }
            authorId = query.lastInsertId().toString();
            refreshPiecesComposerCompleter();
        }

        if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
            QSqlQuery otherPupilQuery("SELECT palid FROM pupilatlesson WHERE lessonid = (SELECT lessonid FROM pupilatlesson WHERE palid =  "+myW->comboBox_palPiecesSelection->itemData(myW->comboBox_palPiecesSelection->currentIndex()).toString()+") AND stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"'", *myW->getMyDb()->getMyPupilDb());
            if (otherPupilQuery.lastError().isValid()) qDebug() << "DB Error: 114 - " << otherPupilQuery.lastError();
            int commonPieceId = -1;
            while(otherPupilQuery.next()) {
                QSqlQuery myPiecesQuery(*myW->getMyDb()->getMyPupilDb());
                myPiecesQuery.prepare("INSERT INTO piece (palid, title, genre, duration, startdate, state, piececomposerid) VALUES ( ?, ?, ?, ?, ?, ?, ? )");
                myPiecesQuery.addBindValue(otherPupilQuery.value(0).toInt());
                myPiecesQuery.addBindValue(myW->lineEdit_newPieceTitle->text());
                myPiecesQuery.addBindValue(myW->comboBox_palPieceGenre->currentText());
                myPiecesQuery.addBindValue(myW->spinBox_palPieceDuration->value());
                myPiecesQuery.addBindValue(QDate::currentDate().toString(Qt::ISODate));
                myPiecesQuery.addBindValue(myW->comboBox_palPieceState->currentIndex());
                myPiecesQuery.addBindValue(authorId.toInt());
                myPiecesQuery.exec();
                if (myPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 115 - " << myPiecesQuery.lastError();
                QSqlQuery myCommonPieceQuery(*myW->getMyDb()->getMyPupilDb());
                myCommonPieceQuery.prepare("UPDATE piece SET cpieceid = ? WHERE pieceid = ?");
                // 			just init commonNoteId for the first time
                if(commonPieceId == -1) commonPieceId = myPiecesQuery.lastInsertId().toInt();
                myCommonPieceQuery.addBindValue(commonPieceId);
                myCommonPieceQuery.addBindValue(myPiecesQuery.lastInsertId().toInt());
                myCommonPieceQuery.exec();
                if(myCommonPieceQuery.lastError().isValid()) qDebug() << "DB Error: 116 - " << myCommonPieceQuery.lastError();
            }
        } else {
            QSqlQuery myPiecesQuery(*myW->getMyDb()->getMyPupilDb());
            myPiecesQuery.prepare("INSERT INTO piece (palid, title, genre, duration, startdate, state, piececomposerid) VALUES ( ?, ?, ?, ?, ?, ?, ? )");
            myPiecesQuery.addBindValue(myW->comboBox_palPiecesSelection->itemData(myW->comboBox_palPiecesSelection->currentIndex()).toInt());
            myPiecesQuery.addBindValue(myW->lineEdit_newPieceTitle->text());
            myPiecesQuery.addBindValue(myW->comboBox_palPieceGenre->currentText());
            myPiecesQuery.addBindValue(myW->spinBox_palPieceDuration->value());
            myPiecesQuery.addBindValue(QDate::currentDate().toString(Qt::ISODate));
            myPiecesQuery.addBindValue(myW->comboBox_palPieceState->currentIndex());
            myPiecesQuery.addBindValue(authorId.toInt());
            myPiecesQuery.exec();
            if (myPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 117 - " << myPiecesQuery.lastError();
            QSqlQuery myCommonPieceQuery(*myW->getMyDb()->getMyPupilDb());
            myCommonPieceQuery.prepare("UPDATE piece SET cpieceid = ? WHERE pieceid = ?");
            int lastId = myPiecesQuery.lastInsertId().toInt();
            myCommonPieceQuery.addBindValue(lastId);
            myCommonPieceQuery.addBindValue(lastId);
            myCommonPieceQuery.exec();
            if(myCommonPieceQuery.lastError().isValid()) qDebug() << "DB Error: 118 - " << myCommonPieceQuery.lastError();

        }
        myPalPiecesModel->refresh();
        myW->treeView_palPieces->resizeColumnToContents(2);
        myW->treeView_palPieces->resizeColumnToContents(4);
    }

}

void PupilTabWidget::delPiece()
{

    //do not del anything while pieces being edited


    if(myW->treeView_palPieces->selectionModel()->hasSelection()) {

        if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
            QSqlQuery myPiecesQuery("DELETE FROM piece WHERE cpieceid="+myPalPiecesModel->index(myW->treeView_palPieces->currentIndex().row(), 1).data().toString());
            if (myPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 119 - " << myPiecesQuery.lastError();
        } else {
            QSqlQuery myPiecesQuery("DELETE FROM piece WHERE pieceid="+myPalPiecesModel->index(myW->treeView_palPieces->currentIndex().row(), 0).data().toString());
            if (myPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 120 - " << myPiecesQuery.lastError();
        }
        myPalPiecesModel->refresh();
        myW->treeView_palPieces->resizeColumnToContents(2);
        myW->treeView_palPieces->resizeColumnToContents(4);
    }

}

void PupilTabWidget::addNewNote()
{

    //do not add anything while notes being edited


    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {

        QSqlQuery otherPupilQuery("SELECT palid FROM pupilatlesson WHERE lessonid = (SELECT lessonid FROM pupilatlesson WHERE palid =  "+myW->comboBox_palNotesSelection->itemData(myW->comboBox_palNotesSelection->currentIndex()).toString()+") AND stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"'", *myW->getMyDb()->getMyPupilDb());
        if (otherPupilQuery.lastError().isValid()) qDebug() << "DB Error: 121 - " << otherPupilQuery.lastError();
        int commonNoteId = -1;
        while(otherPupilQuery.next()) {
            QSqlQuery myNoteQuery(*myW->getMyDb()->getMyPupilDb());
            myNoteQuery.prepare("INSERT INTO note (palid, date, content) VALUES ( ?, ?, ?)");
            myNoteQuery.addBindValue(otherPupilQuery.value(0).toInt());
            myNoteQuery.addBindValue(myW->dateEdit_palNotes->date().toString(Qt::ISODate));
            myNoteQuery.addBindValue(myW->textEdit_palNotes->toHtml().replace("<meta name=\"qrichtext\" content=\"1\" />", "<meta name=\"qrichtext\" content=\"0\" />"));
            myNoteQuery.exec();
            if (myNoteQuery.lastError().isValid()) qDebug() << "DB Error: 122 - " << myNoteQuery.lastError();

            QSqlQuery myCommonNoteQuery(*myW->getMyDb()->getMyPupilDb());
            myCommonNoteQuery.prepare("UPDATE note SET cnoteid = ? WHERE noteid = ?");
            // 			just init commonNoteId for the first time
            if(commonNoteId == -1) commonNoteId = myNoteQuery.lastInsertId().toInt();
            myCommonNoteQuery.addBindValue(commonNoteId);
            myCommonNoteQuery.addBindValue(myNoteQuery.lastInsertId().toInt());
            myCommonNoteQuery.exec();
            if(myCommonNoteQuery.lastError().isValid()) qDebug() << "DB Error: 123 - " << myCommonNoteQuery.lastError();
        }
    } else {
        QSqlQuery myNoteQuery(*myW->getMyDb()->getMyPupilDb());
        myNoteQuery.prepare("INSERT INTO note (palid, date, content) VALUES ( ?, ?, ?)");
        myNoteQuery.addBindValue(myW->comboBox_palNotesSelection->itemData(myW->comboBox_palNotesSelection->currentIndex()).toInt());
        myNoteQuery.addBindValue(myW->dateEdit_palNotes->date().toString(Qt::ISODate));
        myNoteQuery.addBindValue(myW->textEdit_palNotes->toHtml().replace("<meta name=\"qrichtext\" content=\"1\" />", "<meta name=\"qrichtext\" content=\"0\" />"));
        myNoteQuery.exec();
        if (myNoteQuery.lastError().isValid()) qDebug() << "DB Error: 124 - " << myNoteQuery.lastError();

        QSqlQuery myCommonNoteQuery(*myW->getMyDb()->getMyPupilDb());
        myCommonNoteQuery.prepare("UPDATE note SET cnoteid = ? WHERE noteid = ?");
        int lastId = myNoteQuery.lastInsertId().toInt();
        myCommonNoteQuery.addBindValue(lastId);
        myCommonNoteQuery.addBindValue(lastId);
        myCommonNoteQuery.exec();
        if(myCommonNoteQuery.lastError().isValid()) qDebug() << "DB Error: 125 - " << myCommonNoteQuery.lastError();
    }
    myPalNotesModel->refresh();
    myW->treeView_palNotes->resizeColumnToContents(2);
    myW->treeView_palNotes->resizeColumnToContents(3);


}

void PupilTabWidget::delNote()
{

    //do not del anything while notes being edited


    if(myW->treeView_palNotes->selectionModel()->hasSelection()) {

        if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
            QSqlQuery myNotesQuery("DELETE FROM note WHERE cnoteid="+myPalNotesModel->index(myW->treeView_palNotes->currentIndex().row(), 1).data().toString());
            if (myNotesQuery.lastError().isValid()) qDebug() << "DB Error: 126 - " << myNotesQuery.lastError();
        } else {
            QSqlQuery myNotesQuery("DELETE FROM note WHERE noteid="+myPalNotesModel->index(myW->treeView_palNotes->currentIndex().row(), 0).data().toString());
            if (myNotesQuery.lastError().isValid()) qDebug() << "DB Error: 127 - " << myNotesQuery.lastError();
        }
        myPalNotesModel->refresh();
        myW->treeView_palNotes->resizeColumnToContents(2);
        myW->treeView_palNotes->resizeColumnToContents(3);
    }

}

void PupilTabWidget::piecesSelectionChanged(int index)
{
    if(myW->comboBox_palPiecesSelection->count())
        loadPalPieces(myW->comboBox_palPiecesSelection->itemData(index).toInt());
    else {
        myPalPiecesModel->clear();
        myPalPiecesModel->setHeaderData(2, Qt::Horizontal, tr("Title"));
        myPalPiecesModel->setHeaderData(3, Qt::Horizontal, tr("Genre               "));
        myPalPiecesModel->setHeaderData(4, Qt::Horizontal, tr("Duration"));
        myPalPiecesModel->setHeaderData(5, Qt::Horizontal, tr("Start"));
        myPalPiecesModel->setHeaderData(6, Qt::Horizontal, tr("End"));
        myPalPiecesModel->setHeaderData(7, Qt::Horizontal, tr("State"));
    }
    // 	if(myW->comboBox_palPiecesSelection->itemData(index,33).toInt() == 1) {qDebug() << "yes" << myW->comboBox_palPiecesSelection->itemText(index); }
    // 	if(myW->comboBox_palPiecesSelection->itemData(index,33).toInt() == 2) {qDebug() << "no" << myW->comboBox_palPiecesSelection->itemText(index);}
}

void PupilTabWidget::notesSelectionChanged(int index)
{
    if(myW->comboBox_palNotesSelection->count())
        loadPalNotes(myW->comboBox_palNotesSelection->itemData(index).toInt());
    else {
        myPalNotesModel->clear();
        myPalNotesModel->setHeaderData(2, Qt::Horizontal, tr("Date"));
        myPalNotesModel->setHeaderData(3, Qt::Horizontal, tr("Note"));

    }
}

void PupilTabWidget::addNewContActivity()
{
    QSqlQuery query(*myW->getMyDb()->getMyPupilDb());
    query.prepare("INSERT INTO activity (pupilid, ifcontinous, desc, continousday, continoustime, date, continoustype, continousstopdate) VALUES ( ?, 1, ?, ?, ?, ?, ?, ? )");
    query.addBindValue(currentPupilId);
    query.addBindValue(myW->lineEdit_continousActivityName->text());
    query.addBindValue(myW->comboBox_continousActivityDay->currentIndex());
    query.addBindValue(myW->timeEdit_continousActivityTime->time().toString("hh:mm"));
    query.addBindValue(myW->dateEdit_continousActivityStartdate->date().toString(Qt::ISODate));
    query.addBindValue(myW->comboBox_continoustype->currentIndex());
    query.addBindValue("9999-99-99");
    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 128 - " << query.lastError();

    myPupilContActivityModel->refresh();
    myW->treeView_continousActivity->resizeColumnToContents(1);
    myW->treeView_continousActivity->resizeColumnToContents(4);
    myW->treeView_continousActivity->resizeColumnToContents(5);
}

void PupilTabWidget::delContActivity()
{
    if(myW->treeView_continousActivity->selectionModel()->hasSelection()) {

        QSqlQuery query("DELETE FROM activity WHERE activityid="+myPupilContActivityModel->index(myW->treeView_continousActivity->currentIndex().row(), 0).data().toString());
        if (query.lastError().isValid()) qDebug() << "DB Error: 129 - " << query.lastError();

        myPupilContActivityModel->refresh();
        myW->treeView_continousActivity->resizeColumnToContents(1);
        myW->treeView_continousActivity->resizeColumnToContents(4);
        myW->treeView_continousActivity->resizeColumnToContents(5);
    }
}

void PupilTabWidget::addNewSingularActivity()
{
    QSqlQuery query(*myW->getMyDb()->getMyPupilDb());
    query.prepare("INSERT INTO activity (pupilid, ifcontinous, desc, date, noncontinoustype) VALUES ( ?, 0, ?, ?, ? )");
    query.addBindValue(currentPupilId);
    query.addBindValue(myW->lineEdit_singularActivityName->text());
    query.addBindValue(myW->dateEdit_singularActivityDate->date().toString(Qt::ISODate));
    query.addBindValue(myW->comboBox_singularActivityType->currentIndex());
    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 130 - " << query.lastError();

    myPupilSingularActivityModel->refresh();
    myW->treeView_singularActivity->resizeColumnToContents(1);
    myW->treeView_singularActivity->resizeColumnToContents(2);
}

void PupilTabWidget::delSingularActivity()
{
    if(myW->treeView_singularActivity->selectionModel()->hasSelection()) {

        QSqlQuery query("DELETE FROM activity WHERE activityid="+myPupilSingularActivityModel->index(myW->treeView_singularActivity->currentIndex().row(), 0).data().toString());
        if (query.lastError().isValid()) qDebug() << "DB Error: 131 - " << query.lastError();

        myPupilSingularActivityModel->refresh();
        myW->treeView_singularActivity->resizeColumnToContents(1);
        myW->treeView_singularActivity->resizeColumnToContents(2);
    }
}

void PupilTabWidget::loadPupilRentSheetMusic()
{
    myW->treeWidget_rentToPupil->clear();
    QSqlQuery query1("SELECT sml.smlid, smla.author, sml.title, smlp.publisher, sml.lastrentdate FROM sheetmusiclibrary sml, smlauthor smla, smlpublisher smlp, pupil p WHERE p.pupilid=sml.rentpupilid AND smla.smlauthorid=sml.author AND smlp.smlpublisherid=sml.publisher AND sml.rentpupilid="+QString::number(currentPupilId));
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 132 - " << query1.lastError();
    }
    while(query1.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(myW->treeWidget_rentToPupil);
        QString id(query1.value(0).toString());
        int length = id.length();
        int i;
        for(i=length; i < 4; i++) id.prepend("0");
        item->setData(0, Qt::DisplayRole, id);
        item->setData(0, Qt::UserRole, query1.value(0).toString());
        item->setData(1, Qt::DisplayRole, query1.value(1).toString());
        item->setData(2, Qt::DisplayRole, query1.value(2).toString());
        item->setData(3, Qt::DisplayRole, query1.value(3).toString());
        item->setData(4, Qt::DisplayRole, QDate::fromString(query1.value(4).toString(), Qt::ISODate).toString("dd.MM.yyyy"));
    }
    myW->treeWidget_rentToPupil->resizeColumnToContents(0);
    myW->treeWidget_rentToPupil->resizeColumnToContents(1);
    myW->treeWidget_rentToPupil->resizeColumnToContents(2);
    myW->treeWidget_rentToPupil->resizeColumnToContents(3);
    myW->treeWidget_rentToPupil->resizeColumnToContents(4);
    myW->treeWidget_rentToPupil->sortByColumn(0, Qt::AscendingOrder);
}

void PupilTabWidget::sheetBackFromPupil()
{
    QList<QTreeWidgetItem *> selectedItemList = myW->treeWidget_rentToPupil->selectedItems();
    if(!selectedItemList.empty()) {

        QSqlQuery query("UPDATE sheetmusiclibrary SET rentpupilid=-1 WHERE smlid="+selectedItemList.first()->data(0,Qt::UserRole).toString());
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 133 - " << query.lastError();
        }

        loadPupilRentSheetMusic();
    }
}

void PupilTabWidget::notesItemSelected(const QModelIndex &idx, const QModelIndex & /*previous*/)
{
    myW->textEdit_palNotes->setHtml(myPalNotesModel->index(idx.row(), 3).data().toString());
}


void PupilTabWidget::refreshPiecesComposerCompleter()
{
    QStringList composerList;
    QSqlQuery query("SELECT composer FROM piececomposer");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 186 - " << query.lastError();
    }
    while(query.next()) {
        composerList << query.value(0).toString();
    }

    QCompleter *composerCompleter = new QCompleter(composerList);
    composerCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    myW->lineEdit_newPieceComposer->setCompleter(composerCompleter);
}

void PupilTabWidget::refreshPieces()
{
    if(myW->comboBox_palPiecesSelection->count()) loadPalPieces(myW->comboBox_palPiecesSelection->itemData(myW->comboBox_palPiecesSelection->currentIndex()).toInt());
}

void PupilTabWidget::notesTextBold()
{
    QTextCharFormat fmt  = myW->textEdit_palNotes->currentCharFormat();
    fmt.setFontWeight(myW->pushButton_boldNotesText->isChecked() ? QFont::Bold : QFont::Normal);
    //    fmt.setFontItalic(myW->pushButton_italicNotesText->isChecked());
    //    fmt.setFontUnderline(myW->pushButton_underlinedNotesText->isChecked());
    //    fmt.setForeground(myW->textEdit_palNotes->textColor());
    myW->textEdit_palNotes->setCurrentCharFormat(fmt);
}

void PupilTabWidget::notesTextItalic()
{
    QTextCharFormat fmt  = myW->textEdit_palNotes->currentCharFormat();
    fmt.setFontItalic(myW->pushButton_italicNotesText->isChecked());
    myW->textEdit_palNotes->setCurrentCharFormat(fmt);
}

void PupilTabWidget::notesTextUnderlined()
{
    QTextCharFormat fmt  = myW->textEdit_palNotes->currentCharFormat();
    fmt.setFontUnderline(myW->pushButton_underlinedNotesText->isChecked());
    myW->textEdit_palNotes->setCurrentCharFormat(fmt);
}

void PupilTabWidget::notesTextColor()
{
    QColor col;
    QPalette pal = myW->pushButton_notesTextColor->palette();

    if(myW->pushButton_notesTextColor->isChecked()) {
        col = QColorDialog::getColor(myW->textEdit_palNotes->textColor(), this);
        if (!col.isValid()) {
            return;
        }
        QColor tmp = col;
        tmp.setAlpha(100);
        pal.setColor(QPalette::Button, tmp);
    } else {
        col = QColor(Qt::black);
        pal.setColor(QPalette::Button, myW->pushButton_boldNotesText->palette().background().color());
    }

    myW->pushButton_notesTextColor->setPalette(pal);

    QTextCharFormat fmt = myW->textEdit_palNotes->currentCharFormat();
    fmt.setForeground(col);
    myW->textEdit_palNotes->setCurrentCharFormat(fmt);
}

void PupilTabWidget::palNotesEditSelectionChanged()
{
    QTextCharFormat fmt  = myW->textEdit_palNotes->currentCharFormat();
    myW->pushButton_italicNotesText->setChecked(fmt.fontItalic());
    myW->pushButton_underlinedNotesText->setChecked(fmt.fontUnderline());

    QColor col = fmt.foreground().color();
    QPalette pal = myW->pushButton_notesTextColor->palette();

    if(col != Qt::black) {
        col.setAlpha(100);
        pal.setColor(QPalette::Button, col);
    } else {
        pal.setColor(QPalette::Button, myW->pushButton_boldNotesText->palette().button().color());
    }

    myW->pushButton_notesTextColor->setPalette(pal);

    if(fmt.fontWeight() == 75)
        myW->pushButton_boldNotesText->setChecked(true);
    else
        myW->pushButton_boldNotesText->setChecked(false);

}

void PupilTabWidget::palNotesRestoreViewLayout()
{
    myW->treeView_palNotes->hideColumn(0);
    myW->treeView_palNotes->hideColumn(1);
    myW->treeView_palNotes->verticalScrollBar()->setValue(palNotesTreeViewVerticalScrollValue);
}

void PupilTabWidget::palPiecesRestoreViewLayout()
{
    myW->treeView_palPieces->hideColumn(0);
    myW->treeView_palPieces->hideColumn(1);
    myW->treeView_palPieces->verticalScrollBar()->setValue(palPiecesTreeViewVerticalScrollValue);
}

void PupilTabWidget::activityRestoreViewLayout()
{
    myW->treeView_continousActivity->hideColumn(0);
    myW->treeView_singularActivity->hideColumn(0);
    myW->treeView_continousActivity->verticalScrollBar()->setValue(continousActivityTreeViewVerticalScrollValue);
    myW->treeView_singularActivity->verticalScrollBar()->setValue(singularActivityTreeViewVerticalScrollValue);
}

void PupilTabWidget::palNotesSaveScrollState()
{
    palNotesTreeViewVerticalScrollValue = myW->treeView_palNotes->verticalScrollBar()->value();
}

void PupilTabWidget::palPiecesSaveScrollState()
{
    palPiecesTreeViewVerticalScrollValue = myW->treeView_palPieces->verticalScrollBar()->value();
}

void PupilTabWidget::activitySaveScrollState()
{
    continousActivityTreeViewVerticalScrollValue = myW->treeView_continousActivity->verticalScrollBar()->value();
    singularActivityTreeViewVerticalScrollValue = myW->treeView_singularActivity->verticalScrollBar()->value();
}
