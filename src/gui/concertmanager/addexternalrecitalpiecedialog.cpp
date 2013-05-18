#include "addexternalrecitalpiecedialog.h"
#include "ui_addexternalrecitalpiecedialog.h"
#include <QtSql>
#include <QtGui>
#include "configfile.h"

AddExternalRecitalPieceDialog::AddExternalRecitalPieceDialog(int recitalId, ConfigFile *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddExternalRecitalPieceDialog), myConfig(c), currentRecitalId(recitalId)
{
    ui->setupUi(this);

    QStringList composerList;
    QSqlQuery query("SELECT composer FROM piececomposer");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 223 - " << query.lastError();
    }
    while(query.next()) {
        composerList << query.value(0).toString();
    }

    QCompleter *composerCompleter = new QCompleter(composerList);
    composerCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_composer->setCompleter(composerCompleter);

    QStringList myGenreStringList;
    std::list<std::string> genreList = myConfig->readConfigStringList("PalPiecesGenreList");
    std::list<std::string>::iterator it3;
    for(it3= genreList.begin(); it3 != genreList.end(); it3++) {
        myGenreStringList << QString::fromUtf8(it3->c_str());
    }

    QCompleter *genreCompleter = new QCompleter(myGenreStringList);
    genreCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_genre->setCompleter(genreCompleter);
}

AddExternalRecitalPieceDialog::~AddExternalRecitalPieceDialog()
{
    delete ui;
}

void AddExternalRecitalPieceDialog::changeEvent(QEvent *e)
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

void AddExternalRecitalPieceDialog::accept()
{
    //zuerst Komponist suchen oder hinzufügen
    if(ui->lineEdit_composer->text().isEmpty() || ui->lineEdit_title->text().isEmpty() || ui->plainTextEdit_musician->toPlainText().isEmpty()) {
        QMessageBox::warning(this, tr("Qupil"),
                             QString::fromUtf8(tr("Sie müssen mindestens die Felder \"Komponist\", \"Titel\" und \"Musiker\" ausfüllen\n um den Eintrag hinzuzufügen!").toStdString().c_str()),
                             QMessageBox::Ok);
    } else {

        QSqlQuery query;
        query.prepare("INSERT INTO externalrecitalpiece (composer, title, genre, duration, musician) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(ui->lineEdit_composer->text());
        query.addBindValue(ui->lineEdit_title->text());
        query.addBindValue(ui->lineEdit_genre->text());
        query.addBindValue(ui->spinBox_duration->value());
        query.addBindValue(ui->plainTextEdit_musician->toPlainText());
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 225 - " << query.lastError();
        } else {

            QSqlQuery query2;
            query2.prepare("INSERT INTO pieceatrecital (pieceid, recitalid, ifexternalpiece) VALUES (?, ?, 1)");
            query2.addBindValue(query.lastInsertId().toInt());
            query2.addBindValue(currentRecitalId);
            query2.exec();
            if (query2.lastError().isValid()) {
                qDebug() << "DB Error: 226 - " << query2.lastError();
            }

        }

        QDialog::accept();
    }
}
