#include "remindereditdialog.h"
#include "ui_remindereditdialog.h"
#include <QtSql>
#include <QtGui>

ReminderEditDialog::ReminderEditDialog(QWidget *parent, int mode, int reminderid) :
    QDialog(parent),
    ui(new Ui::ReminderEditDialog), myMode(mode), currentReminderId(reminderid)
{
    ui->setupUi(this);

    ui->comboBox_schueler->setEnabled(FALSE);

    QSqlQuery query("SELECT surname, forename, pupilid FROM pupil ORDER BY surname ASC");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 241 - " << query.lastError();
    } else {
        while(query.next()) {
            ui->comboBox_schueler->addItem(QIcon(":/gfx/user.png"), query.value(0).toString()+", "+query.value(1).toString(), query.value(2).toString());
        }
        ui->comboBox_schueler->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }

    //bei editmode daten laden
    if(myMode == 1) {

        QSqlQuery query;
        query.prepare("SELECT * FROM reminder WHERE reminderid = ?");
        query.addBindValue(currentReminderId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 242 - " << query.lastError();
            QMessageBox::critical(this, "Qupil - Fehler", QString("Beim Laden der Daten ist folgender Fehler aufgetreten:\n%1").arg("DB Error: 242 - "+query.lastError().text()), QMessageBox::Ok);
        } else {
            query.next();
            ui->lineEdit->setText(query.value(1).toString());

            switch(query.value(2).toInt()) {

            case 0:
                ui->radioButton_modusProgrammstart->setChecked(TRUE);
                break;
            case 1:
                ui->radioButton_modusJedeUnterrichtseinheit->setChecked(TRUE);
                break;
            case 2: {
                ui->radioButton_modusSpeziellerSchueler->setChecked(TRUE);
                int pos = ui->comboBox_schueler->findData(query.value(3).toInt());
                if(pos != -1) {
                    ui->comboBox_schueler->setCurrentIndex(pos);
                }
            }
            break;
            }

            if(query.value(4).toInt())
                ui->checkBox_sound->setChecked(TRUE);
            else
                ui->checkBox_sound->setChecked(FALSE);

        }
    }
}

ReminderEditDialog::~ReminderEditDialog()
{
    delete ui;
}

void ReminderEditDialog::changeEvent(QEvent *e)
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

void ReminderEditDialog::accept()
{
    if(ui->lineEdit->text().isEmpty()) {
        QMessageBox::information(this, "Qupil - Erinnerungen", "Bitte geben Sie einen Erinnerungstext ein um die Funktion zu nutzen!", QMessageBox::Ok);
        return;
    } else {
        int rmode = 0;
        if(ui->radioButton_modusProgrammstart->isChecked()) {
            rmode = 0;
        } else if(ui->radioButton_modusJedeUnterrichtseinheit->isChecked()) {
            rmode = 1;
        } else if(ui->radioButton_modusSpeziellerSchueler->isChecked()) {
            rmode = 2;
        }

        int sound;
        if(ui->checkBox_sound->isChecked())
            sound = 1;
        else
            sound = 0;

        QSqlQuery query;
        if(myMode == 0 /*add*/) {
            query.prepare("INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES (?, ?, ?, ?)");
            query.addBindValue(ui->lineEdit->text());
            query.addBindValue(rmode);
            query.addBindValue(ui->comboBox_schueler->itemData(ui->comboBox_schueler->currentIndex()));
            query.addBindValue(sound);
        } else { /*edit*/
            query.prepare("UPDATE reminder SET desc = ?, mode = ?, pupilid = ?, notificationsound = ? WHERE reminderid = ?");
            query.addBindValue(ui->lineEdit->text());
            query.addBindValue(rmode);
            query.addBindValue(ui->comboBox_schueler->itemData(ui->comboBox_schueler->currentIndex()));
            query.addBindValue(sound);
            query.addBindValue(currentReminderId);
        }
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 237 - " << query.lastError();
            QMessageBox::critical(this, "Qupil - Fehler", QString("Beim Eintragen der Erinnerung ist folgender Fehler aufgetreten:\n%1").arg("DB Error: 237 - "+query.lastError().text()), QMessageBox::Ok);
        } else {
            QDialog::accept();
        }
    }
}
