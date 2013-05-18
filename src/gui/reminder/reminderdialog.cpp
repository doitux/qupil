#include "reminderdialog.h"
#include "ui_reminderdialog.h"
#include "remindereditdialog.h"
#include <QtSql>

ReminderDialog::ReminderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReminderDialog)
{
    ui->setupUi(this);

    connect( ui->pushButton_add, SIGNAL( clicked() ), this, SLOT( addReminder() ) );
    connect( ui->pushButton_del, SIGNAL( clicked() ), this, SLOT( delReminder() ) );
    connect( ui->pushButton_edit, SIGNAL( clicked() ), this, SLOT( editReminder() ) );

    refreshList();
}

ReminderDialog::~ReminderDialog()
{
    delete ui;
}

void ReminderDialog::changeEvent(QEvent *e)
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

void ReminderDialog::refreshList()
{
    ui->treeWidget->clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM reminder");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 238 - " << query.lastError();
        QMessageBox::critical(this, "Qupil - Fehler", QString("Beim Lesen der Erinnerungsliste ist folgender Fehler aufgetreten:\n%1").arg("DB Error: 238 - "+query.lastError().text()), QMessageBox::Ok);
    } else {
        while(query.next()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setData(0, Qt::UserRole, query.value(0).toInt());
            item->setData(0, Qt::DisplayRole, query.value(1).toString());

            QString rmodeString;
            switch(query.value(2).toInt()) {

            case 0:
                rmodeString = "Beim Programmstart";
                break;
            case 1:
                rmodeString = "Bei jeder Unterrichtseinheit";
                break;
            case 2: {
                rmodeString = QString::fromUtf8(QString("Beim Schüler: ").toStdString().c_str());
                QSqlQuery query2;
                query2.prepare("SELECT surname, forename FROM pupil WHERE pupilid = ?");
                query2.addBindValue(query.value(3).toInt());
                query2.exec();
                if (query2.lastError().isValid()) {
                    qDebug() << "DB Error: 239 - " << query2.lastError();
                    QMessageBox::critical(this, "Qupil - Fehler", QString("Beim Lesen der Erinnerungsliste ist folgender Fehler aufgetreten:\n%1").arg("DB Error: 239 - "+query2.lastError().text()), QMessageBox::Ok);
                } else {
                    query2.next();
                    rmodeString += "\""+query2.value(0).toString()+", "+query2.value(1).toString()+"\"";
                }
            }
            break;
            }

            item->setData(1, Qt::DisplayRole, rmodeString);
            if(query.value(4).toInt())
                item->setIcon(2, QIcon(":/gfx/speaker.png"));

        }

        ui->treeWidget->resizeColumnToContents(0);
        ui->treeWidget->resizeColumnToContents(1);
        ui->treeWidget->resizeColumnToContents(2);
    }
}


void ReminderDialog::addReminder()
{
    ReminderEditDialog *dlg = new ReminderEditDialog(this, 0);
    if(dlg->exec() == QDialog::Accepted) {
        refreshList();
    }
}

void ReminderDialog::delReminder()
{
    QList<QTreeWidgetItem *> selectedItemList = ui->treeWidget->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {

        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
        QSqlQuery query;
        query.prepare("DELETE FROM reminder WHERE reminderid = ?");
        query.addBindValue(selectedItemIdString.toInt());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 240 - " << query.lastError();
            QMessageBox::critical(this, "Qupil - Fehler", QString("Beim Loeschen der Erinnerung ist folgender Fehler aufgetreten:\n%1").arg("DB Error: 240 - "+query.lastError().text()), QMessageBox::Ok);
        } else {
            refreshList();
        }
    }
}

void ReminderDialog::editReminder()
{
    QList<QTreeWidgetItem *> selectedItemList = ui->treeWidget->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {

        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();

        ReminderEditDialog *dlg = new ReminderEditDialog(this, 1, selectedItemIdString.toInt());
        if(dlg->exec() == QDialog::Accepted) {
            refreshList();
        }
    }
}

