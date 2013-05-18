#include "recitaltabs.h"
#include "finishrecitaldialog.h"
#include "configfile.h"
#include "ui_recitaltabs.h"
#include "concertmanagerdialogimpl.h"
#include <QtSql>

RecitalTabs::RecitalTabs(QWidget *parent, ConcertManagerDialogImpl *cm) :
    QWidget(parent),
    ui(new Ui::RecitalTabs), breakComboBoxStateConnection(false), myCM(cm)
{
    ui->setupUi(this);
    ui->treeWidget->hideColumn(0);

    connect(ui->pushButton_pieceUp, SIGNAL(clicked()), this, SLOT(pieceUp()));
    connect(ui->pushButton_pieceDown, SIGNAL(clicked()), this, SLOT(pieceDown()));
    connect(ui->pushButton_saveSorting, SIGNAL(clicked()), this, SLOT(saveSorting()));
    connect(ui->comboBox_state, SIGNAL(currentIndexChanged(int)), this, SLOT(recitalStateChanged(int)));
}

RecitalTabs::~RecitalTabs()
{
    delete ui;
}

void RecitalTabs::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
}

void RecitalTabs::setDate(QString s)
{
    ui->label_date->setText(QDate::fromString(s, Qt::ISODate).toString("dd.MM.yyyy"));
}

QString RecitalTabs::getDate()
{

    QString returnText = QDate::fromString(ui->label_date->text(), "dd.MM.yyyy").toString(Qt::ISODate);
    return returnText;
}

void RecitalTabs::setTime(QString s)
{
    ui->label_time->setText(s+" Uhr");
}

void RecitalTabs::setLocation(QString s)
{
    ui->label_location->setText(s);
}

QString RecitalTabs::getLocation()
{

    return ui->label_location->text();
}

void RecitalTabs::setOrganisator(QString s)
{
    ui->label_organisator->setText(s);
}

void RecitalTabs::setAccompanist(QString s)
{
    ui->label_accompanist->setText(s);
}

void RecitalTabs::setState(int i)
{
    breakComboBoxStateConnection=true;
    ui->comboBox_state->setCurrentIndex(i);
    breakComboBoxStateConnection=false;
}

void RecitalTabs::setRecitalId(int id)
{
    recitalId = id;
    refreshPieces();
}

void RecitalTabs::refreshPieces()
{
    QList<QTreeWidgetItem *> selectedItemList = ui->treeWidget->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    ui->treeWidget->clear();
    QStringList groupAlreadyDone;
    QSqlQuery query("SELECT l.lessonid, l.lessonname, pc.composer, p.title, p.genre, p.duration, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age, pu.forename, pu.surname, par.parid, par.sorting FROM pupil pu, piece p, lesson l, pupilatlesson pal, piececomposer pc, pieceatrecital par WHERE pal.palid = p.palid AND p.piececomposerid=pc.piececomposerid AND pal.lessonid = l.lessonid AND pal.pupilid= pu.pupilid AND pal.stopdate > date('now') AND par.pieceid=p.pieceid AND par.ifexternalpiece=0 AND par.recitalid="+QString::number(recitalId)+" ORDER by par.sorting ASC, age ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 196 - " << query.lastError();
    }
    while(query.next()) {
        //Prüfen ob der Eintrag ein gemeinsamer Gruppeneintrag ist.
        QSqlQuery query1("SELECT count(*) FROM piece p, lesson l, pupilatlesson pal WHERE pal.palid = p.palid AND pal.lessonid = l.lessonid AND pal.stopdate > date('now') AND l.lessonid="+query.value(0).toString()+" AND p.title='"+query.value(3).toString()+"'");
        if (query1.lastError().isValid()) {
            qDebug() << "DB Error: 197 - " << query1.lastError();
        }
        query1.next();
        if(query1.value(0).toInt() > 1) {

            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setData(1, Qt::DisplayRole, query.value(2).toString());
            item->setData(0, Qt::DisplayRole, query.value(11).toString());
            item->setData(0, Qt::UserRole, query.value(10).toString()); //set hidden parid for database actions
            item->setData(2, Qt::DisplayRole, query.value(3).toString());
            item->setData(3, Qt::DisplayRole, query.value(4).toString());
            item->setData(4, Qt::DisplayRole, query.value(5).toString()+" Min.");
//                                item->setData(4, Qt::DisplayRole, query.value(5).toString());

            QSqlQuery query2("SELECT pu.forename, pu.surname, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age FROM pupil pu, pupilatlesson pal WHERE pal.pupilid= pu.pupilid AND pal.lessonid = "+query.value(0).toString()+" AND pal.stopdate > date('now') ORDER by age ASC");
            if (query2.lastError().isValid()) {
                qDebug() << "DB Error: 198 - " << query2.lastError();
            }
            QString pupilsString;
            while(query2.next()) {
                pupilsString += query2.value(1).toString()+", "+query2.value(0).toString()+" ("+query2.value(3).toString()+") - "+query2.value(2).toString()+"\n";
            }
            pupilsString = pupilsString.remove(pupilsString.length()-1,1); //remove last <br>
            item->setData(5, Qt::DisplayRole, pupilsString);

            ui->treeWidget->resizeColumnToContents(1);
            ui->treeWidget->resizeColumnToContents(2);
            ui->treeWidget->resizeColumnToContents(3);
            ui->treeWidget->resizeColumnToContents(4);
            ui->treeWidget->resizeColumnToContents(5);

        } else {
//          Einzeleintrag
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setData(0, Qt::DisplayRole, query.value(11).toString());
            item->setData(0, Qt::UserRole, query.value(10).toString()); //set hidden parid for database actions
            item->setData(1, Qt::DisplayRole, query.value(2).toString());
            item->setData(2, Qt::DisplayRole, query.value(3).toString());
            item->setData(3, Qt::DisplayRole, query.value(4).toString());
            item->setData(4, Qt::DisplayRole, query.value(5).toString()+" Min.");
            item->setData(5, Qt::DisplayRole, query.value(9).toString()+", "+query.value(8).toString()+" ("+query.value(7).toString()+") - "+query.value(6).toString());

            ui->treeWidget->resizeColumnToContents(1);
            ui->treeWidget->resizeColumnToContents(2);
            ui->treeWidget->resizeColumnToContents(3);
            ui->treeWidget->resizeColumnToContents(4);
            ui->treeWidget->resizeColumnToContents(5);
        }
    }

    QSqlQuery query3("SELECT par.parid, par.sorting, erp.composer, erp.title, erp.genre, erp.duration, erp.musician FROM externalrecitalpiece erp, pieceatrecital par WHERE par.pieceid=erp.erpid AND par.ifexternalpiece=1 AND par.recitalid="+QString::number(recitalId)+" ORDER by par.sorting ASC");
    if (query3.lastError().isValid()) {
        qDebug() << "DB Error: 196 - " << query3.lastError();
    }
    while(query3.next()) {

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setData(0, Qt::DisplayRole, query3.value(1).toString());
        item->setData(0, Qt::UserRole, query3.value(0).toString()); //set hidden parid for database actions
        item->setData(1, Qt::DisplayRole, query3.value(2).toString());
        item->setData(2, Qt::DisplayRole, query3.value(3).toString());
        item->setData(3, Qt::DisplayRole, query3.value(4).toString());
        item->setData(4, Qt::DisplayRole, query3.value(5).toString()+" Min.");
        item->setData(5, Qt::DisplayRole, query3.value(6).toString());

        ui->treeWidget->resizeColumnToContents(1);
        ui->treeWidget->resizeColumnToContents(2);
        ui->treeWidget->resizeColumnToContents(3);
        ui->treeWidget->resizeColumnToContents(4);
        ui->treeWidget->resizeColumnToContents(5);
    }

    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);

    //calculate completeDuration
    completePiecesDuration = 0;
    int piecesCounter = 0;
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        completePiecesDuration += (*it)->data(4, Qt::DisplayRole).toString().remove(QRegExp("[^0-9]")).toInt();
        piecesCounter++;
        ++it;
    }

    if(piecesCounter) {
        completeAllInAllDuration = myConfig->readConfigInt("RecitalModerationDuration")+completePiecesDuration+(piecesCounter*myConfig->readConfigInt("RecitalBetweenPiecesDuration"));
    } else {
        completeAllInAllDuration = 0;
    }


    ui->label_completeDuration->setText("Reine Spielzeit: "+QString("<span style='color: blue;'>%1 Min.</span>").arg(completePiecesDuration)+ " - Gesch&auml;tzte Gesamtdauer: "+QString("<span style='color: blue;'>%1 Min.</span>").arg(completeAllInAllDuration));

    myCM->refreshPieceActions(ui->treeWidget->topLevelItemCount());

    //set selection
    if(ui->treeWidget->topLevelItemCount()) {
        int i;
        bool found = false;
        for (i=0; i<ui->treeWidget->topLevelItemCount(); i++) {

            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == selectedItemIdString) {
                ui->treeWidget->setCurrentItem(item);
                found = true;
                break;
            }
        }
        if(!found) {
            selectFirstItem();
        }
    }

}

void RecitalTabs::pieceUp()
{
    if(ui->treeWidget->selectedItems().count()) {

        int oldPosition = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems().first());
        if(oldPosition > 0) {
            QTreeWidgetItem *item = ui->treeWidget->takeTopLevelItem(oldPosition);
            ui->treeWidget->insertTopLevelItem(oldPosition-1, item);
            ui->treeWidget->clearSelection();
            item->setSelected(TRUE);
            ui->treeWidget->expandAll();
        }
    }
}

void RecitalTabs::pieceDown()
{
    if(ui->treeWidget->selectedItems().count()) {

        int oldPosition = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems().first());
        if(oldPosition < ui->treeWidget->topLevelItemCount()-1) {
            QTreeWidgetItem *item = ui->treeWidget->takeTopLevelItem(oldPosition);
            ui->treeWidget->insertTopLevelItem(oldPosition+1, item);
            ui->treeWidget->clearSelection();
            item->setSelected(TRUE);
            ui->treeWidget->expandAll();
        }
    }
}

void RecitalTabs::saveSorting()
{
    if(ui->treeWidget->topLevelItemCount()) {

        int i;
        for (i=0; i<ui->treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            QSqlQuery query;
            query.prepare("UPDATE pieceatrecital SET sorting = ? WHERE parid="+item->data(0, Qt::UserRole).toString());
            query.addBindValue(i);
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 204 - " << query.lastError();
            }
        }
    }
}

int RecitalTabs::getCurrentPieceId()
{
    if(ui->treeWidget->selectedItems().count()) {
        return ui->treeWidget->selectedItems().first()->data(0, Qt::UserRole).toInt();
    } else {
        return -1;
    }
}

void RecitalTabs::recitalStateChanged(int state)
{

    if(state && !breakComboBoxStateConnection) { //erledigt

        FinishRecitalDialog *dialog = new FinishRecitalDialog;
        dialog->exec();
        if(dialog->result() == QDialog::Accepted) {
            if(dialog->getActivityAnswer()) {
                QSqlQuery query;
                query.prepare("SELECT pieceid FROM pieceatrecital WHERE recitalid=? AND ifexternalpiece=0"); // alle werke aus diesem Vorspiel suchen
                query.addBindValue(recitalId);
                query.exec();
                if (query.lastError().isValid()) {
                    qDebug() << "DB Error: 208 - " << query.lastError();
                } else {
                    while(query.next()) {
                        QSqlQuery query2;
                        query2.prepare("SELECT l.type, pal.pupilid FROM lesson l, piece p, pupilatlesson pal WHERE p.palid=pal.palid AND pal.lessonid=l.lessonid AND p.cpieceid=(SELECT cpieceid FROM piece WHERE pieceid=?)");
                        query2.addBindValue(query.value(0).toInt());
                        query2.exec();
                        if (query2.lastError().isValid()) {
                            qDebug() << "DB Error: 209 - " << query2.lastError();
                        } else {
                            while(query2.next()) {
                                int activityType;
                                if(query2.value(0).toInt() == 3) { //das ist ein Ensemble
                                    activityType = 1;
                                } else { // das ist ein Unterricht Gruppe oder Einzel
                                    activityType = 0;
                                }

                                QSqlQuery query3;
                                query3.prepare("SELECT desc, location, date FROM recital WHERE recitalid=?");
                                query3.addBindValue(recitalId);
                                query3.exec();
                                if (query3.lastError().isValid()) {
                                    qDebug() << "DB Error: 210 - " << query3.lastError();
                                } else {
                                    query3.next();

                                    QSqlQuery query4;
                                    query4.prepare("SELECT p.title , pc.composer FROM piece p, piececomposer pc WHERE p.piececomposerid=pc.piececomposerid AND p.pieceid=?");
                                    query4.addBindValue(query.value(0).toInt());
                                    query4.exec();
                                    if (query4.lastError().isValid()) {
                                        qDebug() << "DB Error: 211 - " << query4.lastError();
                                    } else {
                                        query4.next();

                                        QSqlQuery query5;
                                        query5.prepare("INSERT INTO activity (pupilid, ifcontinous, desc, date, noncontinoustype) VALUES (?, 0, ?, ?, ?)");
                                        query5.addBindValue(query2.value(1).toInt());
                                        query5.addBindValue(query3.value(0).toString()+", "+query3.value(1).toString()+": "+query4.value(1).toString()+" - "+query4.value(0).toString());
                                        query5.addBindValue(query3.value(2).toString());
                                        query5.addBindValue(activityType);
                                        query5.exec();
                                        if (query5.lastError().isValid()) {
                                            qDebug() << "DB Error: 231 - " << query5.lastError();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if(dialog->getPieceAnswer()) {

                QSqlQuery query;
                query.prepare("SELECT pieceid FROM pieceatrecital WHERE recitalid=? AND ifexternalpiece=0"); // alle werke aus diesem Vorspiel suchen
                query.addBindValue(recitalId);
                query.exec();
                if (query.lastError().isValid()) {
                    qDebug() << "DB Error: 213 - " << query.lastError();
                } else {
                    while(query.next()) {
                        QSqlQuery query2;
                        query2.prepare("UPDATE piece SET state = 4, stopdate = date('now') WHERE cpieceid=(SELECT cpieceid FROM piece WHERE pieceid=?)");
                        query2.addBindValue(query.value(0).toInt());
                        query2.exec();
                        if (query2.lastError().isValid()) {
                            qDebug() << "DB Error: 214 - " << query2.lastError();
                        }
                    }
                }
            }
        }
    }

    QSqlQuery query;
    query.prepare("UPDATE recital SET state = ? WHERE recitalid = ?");
    query.addBindValue(state);
    query.addBindValue(recitalId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 212 - " << query.lastError();
    }

}

int RecitalTabs::getPiecesCount()
{

    return ui->treeWidget->topLevelItemCount();
}

QAbstractItemModel* RecitalTabs::getListModel()
{

    return ui->treeWidget->model();
}

void RecitalTabs::selectFirstItem()
{
    if(ui->treeWidget->topLevelItemCount()) ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
}

