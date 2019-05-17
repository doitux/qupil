/***************************************************************************
 *   Copyright (C) 2006 by FThauer FHammer   *
 *   f.thauer@web.de   *
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
#include "aboutqupildialogimpl.h"
#include "qupil_defs.h"
#include "configfile.h"
#include <QtCore>


AboutQupilDialogImpl::AboutQupilDialogImpl(QWidget *parent, ConfigFile *c)
    : QDialog(parent), myConfig(c)
{

    setupUi(this);

    myAppDataPath = QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str());

    QPalette myPalette = textBrowser_licence->palette();
    QColor myColor = myPalette.color(QPalette::Window);
    myPalette.setColor(QPalette::Base, myColor);
    textBrowser_licence->setPalette(myPalette);
    textBrowser_2->setPalette(myPalette);

    QFont labelQupilVersionFont = label_qupilVersion->font();
    qDebug() << labelQupilVersionFont.pointSize();
    labelQupilVersionFont.setPointSize(labelQupilVersionFont.pointSize() * 2);
    label_qupilVersion->setFont(labelQupilVersionFont);
    label_qupilVersion->setText(QString(tr("Qupil %1").arg(QString::fromStdString(RELEASE_STRING))));

    label_copyright->setText(QString("© 2006-%1, Felix Hammer").arg(QDateTime::currentDateTime().toString("yyyy")));

    QFile gplFile(QDir::toNativeSeparators(myAppDataPath+"/gpl2.html"));
    QString gplString;
    if(gplFile.exists()) {
        if (gplFile.open( QIODevice::ReadOnly)) {
            QTextStream stream( &gplFile );
            gplString = stream.readAll();
            textBrowser_licence->setHtml(gplString);
        }
    }
}
