#include "selectpiecesdialog.h"
#include "ui_selectpiecesdialog.h"
#include "configfile.h"
#include <QtSql>

SelectPiecesDialog::SelectPiecesDialog(QWidget *parent, ConfigFile *c) :
    QDialog(parent), currentRecitalId(0), myConfig(c),
    ui(new Ui::SelectPiecesDialog)
{
    ui->setupUi(this);

    //lade orte aus der Config
    ui->comboBox_piecesByLocationFilter->clear();
    QStringList myLessonLocationStringList;
    std::list<std::string> locationList = myConfig->readConfigStringList("LessonLocationList");
    std::list<std::string>::iterator it2;
    for(it2= locationList.begin(); it2 != locationList.end(); it2++) {
        myLessonLocationStringList << QString::fromUtf8(it2->c_str());
    }
    ui->comboBox_piecesByLocationFilter->insertItem(0, "alle");
    ui->comboBox_piecesByLocationFilter->insertItems(1, myLessonLocationStringList );

    connect(ui->comboBox_piecesByLocationFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(loadPieces()));

}

SelectPiecesDialog::~SelectPiecesDialog()
{
    delete ui;
}

void SelectPiecesDialog::changeEvent(QEvent *e)
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

int SelectPiecesDialog::exec(int recitalId)
{
    ui->retranslateUi(this);
    currentRecitalId = recitalId;
    loadPieces();
    return QDialog::exec();}

void SelectPiecesDialog::loadPieces()
{
    selectedPieces.clear();

    QString locationFilterInsert;
    if(ui->comboBox_piecesByLocationFilter->currentIndex() == 0) {
        locationFilterInsert = "";
    } else {
        locationFilterInsert = QString(" AND l.lessonlocation = '%1'").arg(ui->comboBox_piecesByLocationFilter->currentText());
    }

    ui->treeWidget->clear();
    QStringList groupAlreadyDone;
    QSqlQuery query("SELECT l.lessonid, l.lessonname, pc.composer, p.title, p.genre, p.duration, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age, pu.forename, pu.surname, p.pieceid, p.cpieceid FROM pupil pu, piece p, lesson l, pupilatlesson pal, piececomposer pc WHERE p.state = 3 AND pal.palid = p.palid AND p.piececomposerid=pc.piececomposerid AND pal.lessonid = l.lessonid AND pal.pupilid= pu.pupilid AND pal.stopdate > date('now')"+locationFilterInsert+" ORDER by age ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 193 - " << query.lastError();
    }
    while(query.next()) {
        //Prüfen ob der Eintrag ein gemeinsamer Gruppeneintrag ist.
        QSqlQuery query1("SELECT count(*) FROM piece p, lesson l, pupilatlesson pal WHERE p.state = 3 AND pal.palid = p.palid AND pal.lessonid = l.lessonid AND pal.stopdate > date('now') AND l.lessonid="+query.value(0).toString()+" AND p.title='"+query.value(3).toString()+"'");
        if (query1.lastError().isValid()) {
            qDebug() << "DB Error: 194 - " << query1.lastError();
        }
        query1.next();
        if(query1.value(0).toInt() > 1) {

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setData(0, Qt::DisplayRole, query.value(2).toString());
			item->setData(0, Qt::UserRole, query.value(11).toString()); // GEÄNDERT um die Prüfung ob bei vorspielkandidaten jemand in einem geplanten vorspiel ist zu ermögliche! BEOBACHTEN ob SEITENEFFEKTE AUFTRETEN!!!   //set hidden cpieceid for database insertion
            item->setData(1, Qt::DisplayRole, query.value(3).toString());
            item->setData(1, Qt::UserRole, query.value(11).toString()); //set hidden cpieceid to avoid double entrys for groups
            item->setData(2, Qt::DisplayRole, query.value(4).toString());
            item->setData(3, Qt::DisplayRole, query.value(5).toString()+" Min.");

            QSqlQuery query2("SELECT pu.forename, pu.surname, pu.instrumenttype, CASE WHEN date(pu.birthday, '+' || (strftime('%Y', 'now') - strftime('%Y', pu.birthday)) || ' years') <= date('now') THEN strftime('%Y', 'now') - strftime('%Y', pu.birthday) ELSE strftime('%Y', 'now') - strftime('%Y', pu.birthday) -1 END AS age FROM pupil pu, pupilatlesson pal WHERE pal.pupilid= pu.pupilid AND pal.lessonid = "+query.value(0).toString()+" AND pal.stopdate > date('now') ORDER by age ASC");
            if (query2.lastError().isValid()) {
                qDebug() << "DB Error: 195 - " << query2.lastError();
            }
            QString pupilsString;
            while(query2.next()) {
                pupilsString += query2.value(1).toString()+", "+query2.value(0).toString()+" ("+query2.value(3).toString()+") - "+query2.value(2).toString()+"\n";
            }
            pupilsString = pupilsString.remove(pupilsString.length()-1,1); //remove last <br>
            item->setData(4, Qt::DisplayRole, pupilsString);

            bool sameItemFound = false;
            QTreeWidgetItemIterator it(ui->treeWidget);
            while (*it) {
                if ((*it)->data(1, Qt::UserRole).toString() == query.value(11).toString()) {
                    sameItemFound = true;
                }
                ++it;
            }

            //Only insert a new item
            if(!sameItemFound) {
                ui->treeWidget->addTopLevelItem(item);
            }

            ui->treeWidget->resizeColumnToContents(0);
            ui->treeWidget->resizeColumnToContents(1);
            ui->treeWidget->resizeColumnToContents(2);
            ui->treeWidget->resizeColumnToContents(3);
            ui->treeWidget->resizeColumnToContents(4);

        } else {
            //          Einzeleintrag
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
			item->setData(0, Qt::UserRole, query.value(11).toString()); // GEÄNDERT um die Prüfung ob bei vorspielkandidaten jemand in einem geplanten vorspiel ist zu ermögliche! BEOBACHTEN ob SEITENEFFEKTE AUFTRETEN!!! //set hidden cpieceid for database insertion
            item->setData(0, Qt::DisplayRole, query.value(2).toString());
            item->setData(1, Qt::DisplayRole, query.value(3).toString());
            item->setData(2, Qt::DisplayRole, query.value(4).toString());
            item->setData(3, Qt::DisplayRole, query.value(5).toString()+" Min.");
            item->setData(4, Qt::DisplayRole, query.value(9).toString()+", "+query.value(8).toString()+" ("+query.value(7).toString()+") - "+query.value(6).toString());

            ui->treeWidget->resizeColumnToContents(0);
            ui->treeWidget->resizeColumnToContents(1);
            ui->treeWidget->resizeColumnToContents(2);
            ui->treeWidget->resizeColumnToContents(3);
            ui->treeWidget->resizeColumnToContents(4);
        }
    }
}


void SelectPiecesDialog::accept()
{
    if(ui->treeWidget->selectedItems().count()) {

        QTreeWidgetItemIterator it(ui->treeWidget, QTreeWidgetItemIterator::Selected);
        while (*it) {
            selectedPieces << (*it)->data(0, Qt::UserRole).toInt();
            ++it;
        }
    }

    QDialog::accept();
}
