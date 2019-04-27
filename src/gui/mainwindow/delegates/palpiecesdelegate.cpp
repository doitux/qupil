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

#include "palpiecesdelegate.h"

#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QSqlQueryModel>
#include <QDebug>
#include <QCompleter>
#include <QtSql>

PalPiecesDelegate::PalPiecesDelegate ( QStringList &g, QStringList &s, QDate &d, QObject *parent )
    : QItemDelegate ( parent ), myGenreList ( g ), myStateList ( s ), myStartDate (d) {}

QWidget *PalPiecesDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QLineEdit *line = new QLineEdit ( parent );
        QStringList composerList;
        QSqlQuery query("SELECT composer FROM piececomposer");
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 187 - " << query.lastError();
        }
        while(query.next()) {
            composerList << query.value(0).toString();
        }
        QCompleter *composerCompleter = new QCompleter(composerList, line);
        composerCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        line->setCompleter(composerCompleter);
        connect ( line, SIGNAL ( editingFinished () ), this, SLOT ( emitCommitData() ) );
        return line;
    }
    break;
    case 4: {
        QComboBox *combo = new QComboBox ( parent );
        combo->insertItems ( 0, myGenreList );
        connect ( combo, SIGNAL ( activated ( int ) ), this, SLOT ( emitCommitData() ) );
        return combo;
    }
    break;
    case 5: {
        QSpinBox *spin = new QSpinBox ( parent );
        connect ( spin, SIGNAL ( editingFinished () ), this, SLOT ( emitCommitData() ) );
        return spin;
    }
    break;
    case 6: {
        QDateEdit *cal = new QDateEdit ( parent );
        cal->setCalendarPopup ( true );
        connect ( cal, SIGNAL ( dateChanged( const QDate&) ), this, SLOT ( emitCommitData() ) );
        return cal;
    }
    break;
    case 7: {
        QDateEdit *cal1 = new QDateEdit ( parent );
        cal1->setCalendarPopup ( true );
        connect ( cal1, SIGNAL ( dateChanged( const QDate&) ), this, SLOT ( emitCommitData() ) );
        return cal1;
    }
    break;
    case 8: {
        QComboBox *combo1 = new QComboBox ( parent );
        combo1->insertItems ( 0, myStateList );
        connect ( combo1, SIGNAL ( activated ( int ) ), this, SLOT ( emitCommitData() ) );
        return combo1;
    }
    break;
    default:
        return QItemDelegate::createEditor ( parent, option, index );
        break;
    }
}

void PalPiecesDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{

    switch ( index.column() ) {
    case 2: {
        QLineEdit *line = qobject_cast<QLineEdit *> ( editor );
        if ( !line ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
    }
    break;
    case 4: {
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
    case 5: {
        QSpinBox *spin = qobject_cast<QSpinBox *> ( editor );
        if ( !spin ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        spin->setValue ( index.model()->data(index).toInt() );
    }
    break;
    case 6: {
        //set minimum and maximum date pal-bereich
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        cal->setMinimumDate(myStartDate);
        cal->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );

    }
    break;
    case 7: {
        //set minimum and maximum date pal-bereich
        QDateEdit *cal1 = qobject_cast<QDateEdit *> ( editor );
        if ( !cal1 ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        cal1->setMinimumDate(myStartDate);
        if(index.model()->data(index).toString() == "") cal1->setDate ( QDate::currentDate() );
        else cal1->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );
    }
    break;
    case 8: {
        QComboBox *combo1 = qobject_cast<QComboBox *> ( editor );
        if ( !combo1 ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        int pos = combo1->findText(index.model()->data(index).toString(), Qt::MatchExactly );
        combo1->setCurrentIndex(pos);
    }
    break;
    default: {
        QItemDelegate::setEditorData ( editor, index );
        return;

    }
    break;
    }
}

void PalPiecesDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QLineEdit *line = qobject_cast<QLineEdit *> ( editor );
        if ( !line ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, line->text() );
    }
    break;
    case 4: {
        QComboBox *combo = qobject_cast<QComboBox *> ( editor );
        if ( !combo ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, combo -> currentText() );
    }
    break;
    case 5: {
        QSpinBox *spin = qobject_cast<QSpinBox *> ( editor );
        if ( !spin ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, spin->value() );
    }
    break;
    case 6: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal->date().toString("yyyy-MM-dd") );
    }
    break;
    case 7: {
        QDateEdit *cal1 = qobject_cast<QDateEdit *> ( editor );
        if ( !cal1 ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal1->date().toString("yyyy-MM-dd") );
    }
    break;
    case 8: {
        QComboBox *combo1 = qobject_cast<QComboBox *> ( editor );
        if ( !combo1 ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, combo1->currentIndex() );
    }
    break;
    default: {
        QItemDelegate::setModelData ( editor, model, index );
        return;

    }
    break;
    }
}

void PalPiecesDelegate::emitCommitData()
{
    emit commitData ( qobject_cast<QWidget *> ( sender() ) );
}
