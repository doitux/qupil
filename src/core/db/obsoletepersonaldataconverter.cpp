/***************************************************************************
 *   Copyright (C) 2006 by Felix Hammer   *
 *   f.hammer@web.de   *
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
#include "obsoletepersonaldataconverter.h"

using namespace std;

PersonalData::PersonalData(QString pupilname)
{

    //Global Objects
    personalDataFile = new QFile;
    pupilName = new QString(pupilname);

    //set pupildir
    QDir subDir;
    subDir.setPath(QDir::home().absolutePath()+"/.qupil/content/"+pupilName->replace(", ", "_"));

    //create pupildir if doesnt exists
    if ( !subDir.exists() ) {
        subDir.mkdir(subDir.absolutePath());
    }

    //set personalDataFile
    personalDataFile->setFileName(subDir.absolutePath()+"/personaldata.txt");

}


PersonalData::~PersonalData()
{
}

QString PersonalData::readData(QString varName, QString defaultValue)
{

    QString tempstring;
    QString line;
    int foundvarname(0);

    personalDataFile->open( QIODevice::ReadOnly );
    QTextStream readStream( personalDataFile );
    while ( !readStream.atEnd() ) {
        line = readStream.readLine();

        if ( line.section( "=", 0, 0) == varName) {
            tempstring = line.section( "=", 1, 1 );
            foundvarname++;
        }
    }
    personalDataFile->close();

    if (foundvarname == 0) {

        QStringList lines;
        QString line;
        QString listtemp;

        if ( personalDataFile->open( QIODevice::ReadOnly ) ) {
            QTextStream stream( personalDataFile );
            while ( !stream.atEnd() ) {
                line = stream.readLine();
                lines += line;

            }
            personalDataFile->close();
        }

        if ( personalDataFile->open( QIODevice::WriteOnly ) ) {

            QTextStream stream( personalDataFile );

            for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
                stream << *it << "\n";
            }
            stream << varName+"="+defaultValue+"\n";
            personalDataFile->close();
        }

        tempstring = defaultValue;
    }

    return tempstring;

}
