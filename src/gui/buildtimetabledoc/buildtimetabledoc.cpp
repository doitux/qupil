#include "buildtimetabledoc.h"
#include "qupil_defs.h"

BuildTimeTableDoc::BuildTimeTableDoc(ConfigFile *c): myConfig(c)
{
    myDocViewerDialog = new DocViewerDialogImpl;
}

void BuildTimeTableDoc::run()
{

    QString string("");

    QStringList dayList;
    dayList << tr("Montag") << tr("Dienstag") << tr("Mittwoch") << tr("Donnerstag") << tr("Freitag") << tr("Samstag") << tr("Sonntag") << QString::fromUtf8(tr("unregelmäßig").toStdString().c_str());

    int i;
    for(i = 0; i < 8; i++)  {

        QSqlQuery query;
        if(i < 7) {
            query.exec("SELECT lessonid, lessonname, lessonstarttime, lessonstoptime FROM lesson WHERE state = 1 AND lessonday="+QString("%1").arg(i,10)+" ORDER BY lessonstarttime ASC");
            if (query.lastError().isValid()) qDebug() << "DB Error: 170 - " << query.lastError();
        } else {
            query.exec("SELECT lessonid, lessonname FROM lesson WHERE state = 1 AND unsteadylesson = 1");
            if (query.lastError().isValid()) qDebug()  << "DB Error: 171 - " << query.lastError();
        }

        if(query.next()) {

            string += "<tr width='100%'><td colspan='3'><h1>"+dayList.at(i)+"</h1></td></tr>";
            query.previous();
        }

        while (query.next()) {

            if(i < 7) {
                string += "<tr width='100%'><td><b>"+query.value(2).toString()+" - "+query.value(3).toString()+"</b></td><td>"+query.value(1).toString()+"</td><td>";
            } else {
                string += "<tr width='100%'><td colspan='2'><b>"+query.value(1).toString()+"</b></td><td>";

            }
            QSqlQuery pupilQuery("SELECT pal.palid, p.forename, p.surname FROM pupilatlesson pal, pupil p WHERE pal.lessonid= "+query.value(0).toString()+" AND p.pupilid = pal.pupilid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"' ORDER BY p.surname ASC");
            if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 172 - " << pupilQuery.lastError();
            else {
                while (pupilQuery.next()) {

                    string += QString(pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString()+"<br>");

                }
                if(string.endsWith("<br>")) {
                    string = string.remove(string.length()-4,4); //lösche das letzt <br>
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
				 <title>Qupil - Stundenplan</title> \
				 </head> \
				 <body> \
				 <table style='text-align: left; margin-left: auto; margin-right: auto; width: 100%;' border='0' cellpadding='0' cellspacing='0'> \
				 <tr><td align='center' width='100%'><h1>Qupil "+QString(RELEASE_STRING)+" - Stundenplan</h2></td></tr> \
				 <tr><td >\
				 <tr><td width='100%'></td></tr>\
				 <tr><td width='100%'><h3>Stand: "+QDate::currentDate().toString("dd.MM.yyyy")+"</h3></td></tr>\
				 <tr><td width='100%'></td></tr>\
				 <tr><td width='100%'>\
				 <table cellpadding='1' cellspacing='0' width='100%' style='text-align: left; border-width:1px; border-style:solid;'> \
				 <tr><th>Uhrzeit</th><th>Unterricht</th><th>Sch&uuml;ler</th></tr>"+string+" \
			</table> \
			</td></tr> \
			<tr><td width='100%'></td></tr> \
			<tr><td align='center'><i>Qupil "+RELEASE_STRING+" - &copy;"+COPYRIGHT_TIME_STRING+" - Felix Hammer - www.lehrsaiten.de</i></td></tr> \
			</table> \
			</table> \
			</body> \
			</html>");

    myDocViewerDialog->exec(doc);
}
