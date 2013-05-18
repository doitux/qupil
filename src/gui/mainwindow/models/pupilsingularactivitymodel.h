/***************************************************************************
 *   Copyright (C) 2008 by Felix Hammer   *
 *   f.hammer@gmx.de   *
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

#ifndef PUPILSINGULARACTIVITYMODEL_H
#define PUPILSINGULARACTIVITYMODEL_H

#include <QSqlQueryModel>

class mainWindowImpl;
class ConfigFile;

class PupilSingularActivityModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    PupilSingularActivityModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &item, int role) const;

    void setMyConfig ( ConfigFile* theValue ) {
        myConfig = theValue;
    }
    void setMyW ( mainWindowImpl* theValue ) {
        myW = theValue;
    }
    void setCurrentPupilId ( int theValue ) {
        currentPupilId = theValue;
    }

    void refresh();
private:
    bool setDate(int id, const QString &date);
    bool setDesc(int id, const QString &desc);
    bool setType(int id, const QString &type);

    ConfigFile *myConfig;
    mainWindowImpl *myW;
    int currentPupilId;
};

#endif
