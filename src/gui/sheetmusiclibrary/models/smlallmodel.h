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

#ifndef SMLALLMODEL_H
#define SMLALLMODEL_H

#include <QSqlQueryModel>

class mainWindowImpl;
class ConfigFile;

class SmlAllModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    SmlAllModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &item, int role) const;

    void setMyW ( mainWindowImpl* theValue ) {
        myW = theValue;
    }
    void setMyConfig ( ConfigFile* theValue ) {
        myConfig = theValue;
    }

    void refresh(int orderColumn = 0, int orderMode = 0);
    void sort(int column, Qt::SortOrder order);

private:
    bool setTitle(int, const QString &);
    bool setAuthor(int, const QString &);
    bool setPublisher(int, const QString &);
    ConfigFile *myConfig;
    mainWindowImpl *myW;
};

#endif
