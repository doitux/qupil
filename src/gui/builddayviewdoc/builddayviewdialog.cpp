#include "builddayviewdialog.h"
#include "ui_builddayviewdialog.h"
#include <QtGui>
#include <QtSql>
#include "qupil_defs.h"
#include "configfile.h"

BuildDayViewDialog::BuildDayViewDialog(ConfigFile* c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildDayViewDialog), myConfig(c)
{
    ui->setupUi(this);

    int i = 0;
    while(i<7) {
        QDate today = QDate::currentDate();
        if(today.dayOfWeek()-1 == i) {
            ui->comboBox_dayOfWeek->setItemText(i, ui->comboBox_dayOfWeek->itemText(i)+" - "+today.toString("dd.MM.yyyy"));
            ui->comboBox_dayOfWeek->setItemData(i, today);
        } else if(today.dayOfWeek()-1 < i) {
            ui->comboBox_dayOfWeek->setItemText(i, ui->comboBox_dayOfWeek->itemText(i)+" - "+today.addDays(i-(today.dayOfWeek()-1)).toString("dd.MM.yyyy"));
            ui->comboBox_dayOfWeek->setItemData(i, today.addDays(i-(today.dayOfWeek()-1)));
        } else if(today.dayOfWeek()-1 > i) {
            ui->comboBox_dayOfWeek->setItemText(i, ui->comboBox_dayOfWeek->itemText(i)+" - "+today.addDays((7+i)-(today.dayOfWeek()-1)).toString("dd.MM.yyyy"));
            ui->comboBox_dayOfWeek->setItemData(i, today.addDays((7+i)-(today.dayOfWeek()-1)));
        }
        i++;
    }

    ui->comboBox_dayOfWeek->setCurrentIndex(QDate::currentDate().dayOfWeek()-1);
}

BuildDayViewDialog::~BuildDayViewDialog()
{
    delete ui;
}

void BuildDayViewDialog::changeEvent(QEvent *e)
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

QTextDocument* BuildDayViewDialog::returnDoc()
{

    QString dayString;
    QString tableHeadString;
    QString string;
    QSqlQuery query;

    int day = ui->comboBox_dayOfWeek->currentIndex();
    if(day < 7) {
        //build strings for days of week

        dayString = ui->comboBox_dayOfWeek->itemData(ui->comboBox_dayOfWeek->currentIndex()).toDate().toString(Qt::DefaultLocaleLongDate);
        tableHeadString = "<tr><th>Uhrzeit</th><th>Unterricht</th><th>Sch&uuml;ler</th></tr>";
        query.exec("SELECT lessonid, lessonname, lessonstarttime, lessonstoptime FROM lesson WHERE state = 1 AND lessonday="+QString("%1").arg(day,10)+" ORDER BY lessonstarttime ASC");
    } else {
        dayString = "unregelm&auml;&szlig;ige Termine";
        tableHeadString = "<tr><th>Unterricht</th><th>Sch&uuml;ler</th></tr>";
        query.exec("SELECT lessonid, lessonname FROM lesson WHERE state = 1 AND unsteadylesson = 1");
    }

    if (query.lastError().isValid()) qDebug() << "DB Error: 250 - " << query.lastError();
    else {
        while(query.next()) {

            if(day < 7) {
                string += "<tr width='100%'><td><h3>"+query.value(2).toString()+" - "+query.value(3).toString()+"</h3></td><td><h3>"+query.value(1).toString()+"</h3></td><td>";
            } else {
                string += "<tr width='100%'><td><h3>"+query.value(1).toString()+"</h3></td><td>";
            }

            QList<int> palList;
            QStringList pupilList;

            QSqlQuery pupilQuery("SELECT pal.palid, p.forename, p.surname FROM pupilatlesson pal, pupil p WHERE pal.lessonid= "+query.value(0).toString()+" AND p.pupilid = pal.pupilid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"' ORDER BY p.surname ASC");
            pupilQuery.exec();
            if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 252 - " << pupilQuery.lastError();
            else {
                while (pupilQuery.next()) {

                    palList << pupilQuery.value(0).toInt();
                    pupilList << pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString();
                    string += pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString()+"<br>";

                }
                string = string.remove(string.lastIndexOf("<br>"),4);
                string += "</td></tr>";
            }

            if(day < 7) {
                string += "<tr width='100%'><td colspan='3'>";
            } else {
                string += "<tr width='100%'><td colspan='2'>";
            }

            if(ui->spinBox_height->value()) {
                //print empty line
                string += "<table cellpadding='0' cellspacing='0' border='0' width='100%'><tr><td>";
                int i;
                for(i=0; i<ui->spinBox_height->value(); i++) {
                    string += "<p><br></p>";
                }
                string += "<hr></td></tr></table>";
            }

            if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {

                pupilQuery.previous();
                QSqlQuery notesQuery ("SELECT strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid="+QString::number(pupilQuery.value(0).toInt())+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date DESC");
                notesQuery.exec();
                if (notesQuery.lastError().isValid()) qDebug() << "DB Error: 254 - " << notesQuery.lastError();
                else {
                    string += "<table cellpadding='0' cellspacing='0' border='0'>";
                    int i = 1;
                    while (notesQuery.next() && i <= ui->spinBox_NotesNumber->value()) {
                        string += "<tr><td><u>"+notesQuery.value(0).toString()+"</u>: </td><td> "+notesQuery.value(1).toString()+"</td></tr>";
                        i++;
                    }
                    string += "</table>";
                }
            } else {

                QListIterator<int> it(palList);
                int j = 0;
                while (it.hasNext()) {
                    QSqlQuery notesQuery ("SELECT strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid="+QString::number(it.next())+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date DESC");
                    notesQuery.exec();
                    if (notesQuery.lastError().isValid()) qDebug() << "DB Error: 251 - " << notesQuery.lastError();
                    else {
                        string += "<table cellpadding='0' cellspacing='0' border='0'>";
                        int i = 1;
                        while (notesQuery.next() && i <= ui->spinBox_NotesNumber->value()) {
                            string += "<tr><td><u>"+notesQuery.value(0).toString()+"</u> (<i>"+pupilList.at(j)+"</i>): </td><td> "+notesQuery.value(1).toString()+"</td></tr>";
                            i++;
                        }
                        string += "</table>";
                    }
                    j++;
                }
            }
            string += "</td></tr>";
        }
    }

    QTextDocument *doc = new QTextDocument;
    doc->setHtml("<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'> \
    <html> \
    <head> \
    <meta content='text/html; charset=ISO-8859-1' http-equiv='content-type'> \
    <title>Qupil - Tages&uuml;bersicht</title> \
    </head> \
    <body> \
    <table style='text-align: left; margin-left: auto; margin-right: auto; width: 100%;' border='0' cellpadding='0' cellspacing='0'> \
      <tr><td align='center' width='100%'><h2>Qupil "+QString(RELEASE_STRING)+" - Tages&uuml;bersicht - "+ dayString+"</h2></td></tr> \
      <tr><td >\
      <tr><td width='100%'></td></tr>\
      <tr><td width='100%'>\
        <table cellpadding='1' cellspacing='0' width='100%' style='text-align: left; border-width:1px; border-style:solid;'> \
        "+tableHeadString+string+" \
        </table> \
      </td></tr> \
    <tr><td width='100%'></td></tr> \
    <tr><td align='center'><i>Qupil "+RELEASE_STRING+" - &copy;"+COPYRIGHT_TIME_STRING+" - Felix Hammer - www.lehrsaiten.de</i></td></tr> \
    </table> \
    </table> \
    </body> \
    </html>");

    return doc;
}
