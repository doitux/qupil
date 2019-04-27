#include "pupilsnotinensembledialog.h"
#include "ui_pupilsnotinensembledialog.h"
#include "configfile.h"
#include <QtSql>

PupilsNotInEnsembleDialog::PupilsNotInEnsembleDialog(ConfigFile *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PupilsNotInEnsembleDialog), myConfig(c)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnHidden(0, true);

    QString msg;
    int duePupilCounter = 0;
    int activeEnsembleCounter = 0;

    QSqlQuery query;
    query.prepare("SELECT pupilid, forename, surname FROM pupil WHERE ensembleactivityrequested = 1");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 258 - " << query.lastError();
    } else {
        while(query.next()) {

            QSqlQuery query2;
            query2.prepare("SELECT count(*) FROM pupilatlesson WHERE pupilid=? AND stopdate > date('now')");
            query2.addBindValue(query.value(0).toInt());
            query2.exec();
            if (query2.lastError().isValid()) {
                qDebug() << "DB Error: 259 - " << query2.lastError();
            } else {
                query2.next();
                if(query2.value(0).toInt()) { // check if pupil is in any active lesson

                    QSqlQuery query3;
                    query3.prepare("SELECT desc, continousstopdate FROM activity WHERE ifcontinous=1 AND continoustype=0 AND pupilid = ? ORDER BY continousstopdate DESC");
                    query3.addBindValue(query.value(0).toInt());
                    query3.exec();
                    if (query3.lastError().isValid()) {
                        qDebug() << "DB Error: 260 - " << query3.lastError();
                    } else {
                        QString lastEnsenbleStopDate = "";
                        bool activeEnsemble = false;
                        bool anyEnsemble = false;
                        if(query3.next()) {

                            if(query3.value(1).toString() == "9999-99-99") {
                                activeEnsemble = true;
                                activeEnsembleCounter++;
                            } else {
                                lastEnsenbleStopDate = query3.value(1).toString();
                            }

                            anyEnsemble = true;
                        } else {

                        }

                        if(!activeEnsemble || !anyEnsemble) {
                            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                            item->setData(0, Qt::DisplayRole, lastEnsenbleStopDate);
                            item->setData(0, Qt::UserRole, query.value(0).toInt());
                            item->setData(1, Qt::DisplayRole, query.value(2).toString()+", "+query.value(1).toString());
                            if(anyEnsemble) {
                                item->setData(2, Qt::DisplayRole, QDate::fromString(query3.value(1).toString(), Qt::ISODate).toString("dd.MM.yyyy")+" ("+query3.value(0).toString()+")");
                            } else {
                                item->setData(2, Qt::DisplayRole, "keine");
                            }


                            duePupilCounter++;
                        }
                    }
                }
            }
        }
    }

    QString head;
    if(duePupilCounter) {
        head = "Folgende Schüler sind derzeit in keinem Ensemble aktiv:";
    } else {
        head = "Aktuell sind alle Schüler in Ensembles aktiv.<br><br>Gute Arbeit!";
    }

    msg += "<span style='font-size:10pt; font-weight:600;'>"+QString::fromUtf8(head.toStdString().c_str())+"</span><br><br>";

    ui->label_txt->setText(msg);
    ui->pupilStats->setText(QString::fromUtf8(QString("Insgesamt <span style='color:red; font-weight:600;'>%1</span> inaktive Schüler | <span style='color:green; font-weight:600;'>%2</span> Schüler sind in Ensembles aktiv").arg(duePupilCounter).arg(activeEnsembleCounter).toStdString().c_str()));

    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->sortByColumn(0, Qt::DescendingOrder);

    pupilPopupMenu = new QMenu();
    addReminderAction = new QAction(QIcon(":/gfx/preferences-desktop-notification-bell.png"), QString::fromUtf8(tr("Erinnerung an Ensemblemitwirkung einrichten").toStdString().c_str()), pupilPopupMenu);
    pupilPopupMenu->addAction(addReminderAction);

    connect(ui->treeWidget, SIGNAL (customContextMenuRequested(const QPoint)), this, SLOT ( showPopupMenu(const QPoint)));
    connect(addReminderAction, SIGNAL (triggered()), this, SLOT ( addReminder()));


}

PupilsNotInEnsembleDialog::~PupilsNotInEnsembleDialog()
{
    delete ui;
}

void PupilsNotInEnsembleDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PupilsNotInEnsembleDialog::showPopupMenu(const QPoint point)
{
    if(ui->treeWidget->topLevelItemCount()) {
        pupilPopupMenu->popup(ui->treeWidget->mapToGlobal(point));
    }

}

void PupilsNotInEnsembleDialog::addReminder()
{
    QList<QTreeWidgetItem *> selectedItemList = ui->treeWidget->selectedItems();
    int selectedItemId;
    QString pupil;
    if(!selectedItemList.empty()) {
        selectedItemId = selectedItemList.first()->data(0,Qt::UserRole).toInt();
        pupil = selectedItemList.first()->data(1,Qt::DisplayRole).toString();

        QSqlQuery query;
        query.prepare("INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES (?, ?, ?, ?)");
        query.addBindValue(QString::fromUtf8(QString("Ensemblemitwirkung ansprechen!").toStdString().c_str()));
        query.addBindValue(2);
        query.addBindValue(selectedItemId);
        query.addBindValue(1);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 262 - " << query.lastError();
        } else {
            QMessageBox::information(this, "Erinnerung eingetragen", QString("Erinnerung zum Thema \"Ensemblemitwirkung\"<br>f&uuml;r den Sch&uuml;ler <i><b>%1</b></i> eingerichtet.").arg(pupil), QMessageBox::Ok);
        }
    }
}
