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

#include "smlalldelegate.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

SmlAllDelegate::SmlAllDelegate (QObject *parent )
    : QItemDelegate ( parent ) {}

QWidget *SmlAllDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 1: {
        QLineEdit *line = new QLineEdit ( parent );

        QStringList authorList;
        QSqlQuery query("SELECT author FROM smlauthor");
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 146 - " << query.lastError();
        }
        while(query.next()) {
            authorList << query.value(0).toString();
        }
        QCompleter *authorCompleter = new QCompleter(authorList);
        authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        line->setCompleter(authorCompleter);

        connect ( line, SIGNAL ( editingFinished () ), this, SLOT ( emitCommitData() ) );
        return line;
    }
    break;
    case 3: {
        QLineEdit *line1 = new QLineEdit ( parent );

        QStringList publisherList;
        QSqlQuery query1("SELECT publisher FROM smlpublisher");
        if (query1.lastError().isValid()) {
            qDebug() << "DB Error: 147 - " << query1.lastError();
        }
        while(query1.next()) {
            publisherList << query1.value(0).toString();
        }
        QCompleter *publisherCompleter = new QCompleter(publisherList);
        publisherCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        line1->setCompleter(publisherCompleter);

        connect ( line1, SIGNAL ( editingFinished () ), this, SLOT ( emitCommitData() ) );
        return line1;
    }
    break;
    default:
        return QItemDelegate::createEditor ( parent, option, index );
        break;
    }
}

void SmlAllDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{

    switch ( index.column() ) {
    case 1: {
        QLineEdit *line = qobject_cast<QLineEdit *> ( editor );
        if ( !line ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        line->setText(index.model()->data(index).toString());
    }
    break;
    case 3: {
        QLineEdit *line1 = qobject_cast<QLineEdit *> ( editor );
        if ( !line1 ) {
            QItemDelegate::setEditorData ( editor, index );
            return;
        }
        line1->setText(index.model()->data(index).toString());
    }
    break;
    default: {
        QItemDelegate::setEditorData ( editor, index );
        return;

    }
    break;
    }
}

void SmlAllDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 1: {
        QLineEdit *line = qobject_cast<QLineEdit *> ( editor );;
        if ( !line ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, line->text() );
    }
    break;
    case 3: {
        QLineEdit *line1 = qobject_cast<QLineEdit *> ( editor );;
        if ( !line1 ) {
            QItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, line1->text() );
    }
    break;
    default: {
        QItemDelegate::setModelData ( editor, model, index );
        return;

    }
    break;
    }
}

void SmlAllDelegate::emitCommitData()
{
    emit commitData ( qobject_cast<QWidget *> ( sender() ) );
}
