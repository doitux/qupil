#include "pupilslastrecitalviewdialog.h"
#include "ui_pupilslastrecitalviewdialog.h"
#include "configfile.h"
#include <QtSql>


PupilsLastRecitalViewDialog::PupilsLastRecitalViewDialog(ConfigFile *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PupilsLastRecitalViewDialog), myConfig(c)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnHidden(0, true);

    QString msg;
    QString pupilsList;
    int duePupilCounter = 0;

    QSqlQuery query;
    query.prepare("SELECT pupilid, recitalinterval, forename, surname, firstlessondate FROM pupil WHERE recitalinterval!=0");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 229 - " << query.lastError();
    }
    while(query.next()) {

        QSqlQuery query3;
        query3.prepare("SELECT count(*) FROM pupilatlesson WHERE pupilid=? AND stopdate > date('now')");
        query3.addBindValue(query.value(0).toInt());
        query3.exec();
        if (query3.lastError().isValid()) {
            qDebug() << "DB Error: 231 - " << query3.lastError();
        }
        query3.next();
        if(query3.value(0).toInt()) { // check if pupil is in any active lesson

			//check if pupils is currently added in any planned recital
			QSqlQuery query4;
			query4.prepare("SELECT COUNT(*) FROM pieceatrecital par, piece p, pupilatlesson pal, pupil pu WHERE par.pieceid=p.cpieceid AND p.palid=pal.palid AND pal.pupilid=pu.pupilid AND pu.pupilid=?");
			query4.addBindValue(query.value(0).toInt());
			query4.exec();
			if (query4.lastError().isValid()) {
				qDebug() << "DB Error: 266 - " << query4.lastError();
			}
			query4.next();
			if(query4.value(0).toInt() == 0) { // Schüler ist in KEINEM geplanten Vorspiel eingetragen

				int intervalDays=0;

				switch(query.value(1).toInt()) {

				case 1:
					intervalDays=30;
					break;
				case 2:
					intervalDays=60;
					break;
				case 3:
					intervalDays=90;
					break;
				case 4:
					intervalDays=120;
					break;
				case 5:
					intervalDays=180;
					break;
				case 6:
					intervalDays=270;
					break;
				case 7:
					intervalDays=360;
					break;
				case 8:
					intervalDays=540;
					break;
				case 9:
					intervalDays=720;
					break;
				}

				QSqlQuery query2;
				QString type;
				if(myConfig->readConfigInt("RecitalIntervalCheckerOnlySolo")) {
					type="AND noncontinoustype=0";
				} else {
					type="AND (noncontinoustype=0 OR noncontinoustype=1)";
				}
				query2.prepare("SELECT date FROM activity WHERE ifcontinous=0 "+type+" AND pupilid=? ORDER BY date DESC");
				query2.addBindValue(query.value(0).toInt());
				query2.exec();
				if (query2.lastError().isValid()) {
					qDebug() << "DB Error: 230 - " << query2.lastError();
				}
				if(query2.next()) {

					//persöliches Intervall mit dem letzten Termin vergleichen
					QDate lastRecital = QDate::fromString(query2.value(0).toString(), Qt::ISODate);
					int currentIntervall = lastRecital.daysTo(QDate::currentDate());

					if(currentIntervall > intervalDays) {

						QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
						item->setData(0, Qt::UserRole, query.value(0).toInt());
						item->setData(0, Qt::DisplayRole, currentIntervall);
						item->setData(1, Qt::DisplayRole, query.value(3).toString()+", "+query.value(2).toString());
						if(currentIntervall> 30) {
							currentIntervall = currentIntervall/30;
							item->setData(2, Qt::DisplayRole, QString("%1 Monaten").arg(currentIntervall));
						} else {
							item->setData(2, Qt::DisplayRole, QString("%1 Tagen").arg(currentIntervall));
						}
						item->setData(3, Qt::DisplayRole, QDate::fromString(query2.value(0).toString(), Qt::ISODate).toString("dd.MM.yyyy"));

						duePupilCounter++;
					}
				} else {
					//kein Termin gefunden dann mit Unterrichtsbeginn vergleichen
					QDate lessonStartDate = QDate::fromString(query.value(4).toString(), Qt::ISODate);
					int currentIntervall = lessonStartDate.daysTo(QDate::currentDate());

					if(currentIntervall > intervalDays) {

						QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
						item->setData(0, Qt::UserRole, query.value(0).toInt());
						item->setData(0, Qt::DisplayRole, currentIntervall);
						item->setData(1, Qt::DisplayRole, query.value(3).toString()+", "+query.value(2).toString());
						if(currentIntervall> 30) {
							currentIntervall = currentIntervall/30;
							item->setData(2, Qt::DisplayRole, QString("%1 Monaten").arg(currentIntervall));
						} else {
							item->setData(2, Qt::DisplayRole, QString("%1 Tagen").arg(currentIntervall));
						}
						item->setData(3, Qt::DisplayRole, QDate::fromString(query.value(4).toString(), Qt::ISODate).toString("dd.MM.yyyy"));

						duePupilCounter++;
					}
				}
			}
        }
    }


    QString head;
    if(duePupilCounter) {
        head = "Folgende überfällige Vorspielkandidaten wurden ermittelt:";
    } else {
        head = "Derzeit sind keine überfälligen Vorspielkandidaten vorhanden.<br><br>Gute Arbeit!";
    }

    msg += "<span style='font-weight:600;'>"+QString::fromUtf8(head.toStdString().c_str())+"</span><br><br>";

    ui->label_txt->setText(msg);

    ui->pupilStats->setText(QString::fromUtf8(QString("Insgesamt <b>%1</b> überfällige Schüler").arg(duePupilCounter).toStdString().c_str()));

    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
    ui->treeWidget->resizeColumnToContents(4);
    ui->treeWidget->sortByColumn(0, Qt::DescendingOrder);

    pupilPopupMenu = new QMenu();
    addReminderAction = new QAction(QIcon(":/gfx/preferences-desktop-notification-bell.svg"), QString::fromUtf8(tr("Erinnerung an Vorspielaktivität einrichten").toStdString().c_str()), pupilPopupMenu);
    pupilPopupMenu->addAction(addReminderAction);

    connect(ui->treeWidget, SIGNAL (customContextMenuRequested(const QPoint)), this, SLOT ( showPopupMenu(const QPoint)));
    connect(addReminderAction, SIGNAL (triggered()), this, SLOT ( addReminder()));

}

PupilsLastRecitalViewDialog::~PupilsLastRecitalViewDialog()
{
    delete ui;
}

void PupilsLastRecitalViewDialog::changeEvent(QEvent *e)
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

void PupilsLastRecitalViewDialog::showPopupMenu(const QPoint point)
{
    if(ui->treeWidget->topLevelItemCount()) {
        pupilPopupMenu->popup(ui->treeWidget->mapToGlobal(point));
    }

}

void PupilsLastRecitalViewDialog::addReminder()
{
    QList<QTreeWidgetItem *> selectedItemList = ui->treeWidget->selectedItems();
    int selectedItemId;
    QString pupil;
    if(!selectedItemList.empty()) {
        selectedItemId = selectedItemList.first()->data(0,Qt::UserRole).toInt();
        pupil = selectedItemList.first()->data(1,Qt::DisplayRole).toString();

        QSqlQuery query;
        query.prepare("INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES (?, ?, ?, ?)");
        query.addBindValue(QString::fromUtf8(QString("Vorspielaktivität ansprechen!").toStdString().c_str()));
        query.addBindValue(2);
        query.addBindValue(selectedItemId);
        query.addBindValue(1);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 263 - " << query.lastError();
        } else {
            QMessageBox::information(this, "Erinnerung eingetragen", QString("Erinnerung zum Thema \"Vorspielaktivit&auml;t\"<br>f&uuml;r den Sch&uuml;ler <i><b>%1</b></i> eingerichtet.").arg(pupil), QMessageBox::Ok);
        }
    }
}
