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

#include "pupilcontactivitydelegate.h"

#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QSqlQueryModel>
#include <QDebug>

PupilContActivityDelegate::PupilContActivityDelegate (QObject *parent )
    : QItemDelegate ( parent ) {}

QWidget *PupilContActivityDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QStringList typeList;
        typeList << tr("Ensemble") << tr("Theorieunterricht") << tr("Korrepetition") << tr("Sonstiges");

        QComboBox *combo = new QComboBox ( parent );
        combo->insertItems ( 0, typeList );
        connect ( combo, SIGNAL ( activated ( int ) ), this, SLOT ( emitCommitData() ) );
        return combo;
    }
    break;
    case 3: {
        QStringList dayList;
        dayList << tr("Montag") << tr("Dienstag") << tr("Mittwoch") << tr("Donnerstag") << tr("Freitag") << tr("Samstag") << tr("Sonntag") << QString::fromUtf8(tr("unregelmäßig").toStdString().c_str());

        QComboBox *combo = new QComboBox ( parent );
        combo->insertItems ( 0, dayList );
        connect ( combo, SIGNAL ( activated ( int ) ), this, SLOT ( emitCommitData() ) );
        return combo;
    }
    break;
    case 4: {
        QTimeEdit *time = new QTimeEdit ( parent );
        connect ( time, SIGNAL ( dateChanged ( const QDate ) ), this, SLOT ( emitCommitData() ) );
        return time;
    }
    break;
    case 5: {
        QDateEdit *cal = new QDateEdit ( parent );
        cal->setCalendarPopup ( true );
        connect ( cal, SIGNAL ( dateChanged ( const QDate ) ), this, SLOT ( emitCommitData() ) );
        return cal;
    }
    break;
    case 6: {
        QDateEdit *cal1 = new QDateEdit ( parent );
        cal1->setCalendarPopup ( true );
        connect ( cal1, SIGNAL ( dateChanged ( const QDate ) ), this, SLOT ( emitCommitData() ) );
        return cal1;
    }
    break;
    default:
        return QItemDelegate::createEditor ( parent, option, index );
        break;
    }
}

void PupilContActivityDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{

    switch ( index.column() ) {
    case 2: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        int pos = combo->findText(index.model()->data(index).toString(), Qt::MatchExactly );
        if(pos == -1) {
            combo->addItem(index.model()->data(index).toString());
            combo->findText(index.model()->data(index).toString(), Qt::MatchExactly );
        }
        combo->setCurrentIndex(pos);
    }
    case 3: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        int pos = combo->findText(index.model()->data(index).toString(), Qt::MatchExactly );
        if(pos == -1) {
            combo->addItem(index.model()->data(index).toString());
            combo->findText(index.model()->data(index).toString(), Qt::MatchExactly );
        }
        combo->setCurrentIndex(pos);
    }
    break;
    case 4: {
        QTimeEdit *time = qobject_cast<QTimeEdit *> ( editor );
        if ( !time ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        time->setTime ( QTime::fromString ( index.model()->data(index).toString(),"hh:mm" ) );
    }
    break;
    case 5: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        cal->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );

    }
    break;
    case 6: {
        QDateEdit *cal1 = qobject_cast<QDateEdit *> ( editor );
        if ( !cal1 ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        if(index.model()->data(index).toString() == "") cal1->setDate ( QDate::currentDate() );
        else cal1->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );
    }
    break;
    default: {
        QItemDelegate::setEditorData ( editor, index );
        return;

    }
    break;
    }
}

void PupilContActivityDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {

            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, combo -> currentIndex() );
    }
    break;
    case 3: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {

            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, combo -> currentIndex() );
    }
    break;
    case 4: {
        QTimeEdit *time = qobject_cast<QTimeEdit *> ( editor );
        if ( !time ) {
            QItemDelegate::setModelData ( editor, model, index );
        }
        model -> setData ( index, time->time().toString("hh:mm") );
    }
    break;
    case 5: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal->date().toString("yyyy-MM-dd") );
    }
    break;
    case 6: {
        QDateEdit *cal1 = qobject_cast<QDateEdit *> ( editor );
        if ( !cal1 ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal1->date().toString("yyyy-MM-dd") );
    }

    break;
    default: {
        QItemDelegate::setModelData ( editor, model, index );
        return;

    }
    break;
    }
}

void PupilContActivityDelegate::emitCommitData()
{
    emit commitData ( qobject_cast<QWidget *> ( sender() ) );
}
