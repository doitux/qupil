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

#include "pupilsingularactivitydelegate.h"

#include <QDateEdit>
#include <QComboBox>
#include <QSqlQueryModel>
#include <QDebug>

PupilSingularActivityDelegate::PupilSingularActivityDelegate (QObject *parent )
    : QItemDelegate ( parent ) {}

QWidget *PupilSingularActivityDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = new QDateEdit ( parent );
        cal->setCalendarPopup ( true );
        connect ( cal, SIGNAL ( dateChanged ( const QDate ) ), this, SLOT ( emitCommitData() ) );
        return cal;
    }
    break;
    case 3: {
        QComboBox *combo = new QComboBox ( parent );
        QStringList myTypeList;
        myTypeList << "Solo-Vortrag" << "Ensemble-Mitwirkung" << "sonstiges";
        combo->insertItems ( 0, myTypeList );
        connect ( combo, SIGNAL ( activated ( int ) ), this, SLOT ( emitCommitData() ) );
        return combo;
    }
    break;

    default:
        return QItemDelegate::createEditor ( parent, option, index );
        break;
    }
}

void PupilSingularActivityDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{

    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        cal->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );
    }
    break;
    case 3: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        combo->setCurrentIndex(index.model()->data(index).toInt());
    }
    break;
    default: {
        QItemDelegate::setEditorData ( editor, index );
        return;

    }
    break;
    }
}

void PupilSingularActivityDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal->date().toString("yyyy-MM-dd") );
    }
    break;
    case 3: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, combo->currentIndex() );
    }
    break;
    default: {
        QItemDelegate::setModelData ( editor, model, index );
        return;

    }
    break;
    }
}

void PupilSingularActivityDelegate::emitCommitData()
{
    emit commitData ( qobject_cast<QWidget *> ( sender() ) );
}
