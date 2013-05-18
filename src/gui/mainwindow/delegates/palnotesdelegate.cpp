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

#include "palnotesdelegate.h"

#include <QDateEdit>
#include <QSqlQueryModel>
#include <QDebug>
#include <QtGui>
#include <QtCore>
#include "delegatetextedit.h"

PalNotesDelegate::PalNotesDelegate (QDate &d, QObject *parent )
    : QStyledItemDelegate ( parent ), myStartDate(d) {}

QWidget *PalNotesDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = new QDateEdit ( parent );
        cal->setCalendarPopup ( TRUE );
        connect ( cal, SIGNAL ( dateChanged ( const QDate ) ), this, SLOT ( emitCommitData() ) );
        return cal;
    }
    break;
    case 3: {
        DelegateTextEdit *te = new DelegateTextEdit ( parent );
        connect ( te, SIGNAL ( editingFinished() ), this, SLOT ( emitCommitData() ) );
        return te;
    }
    break;

    default:
        return QStyledItemDelegate::createEditor ( parent, option, index );
        break;
    }
}

void PalNotesDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{

    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QStyledItemDelegate::setEditorData ( editor, index );
            return;
        }
        cal->setMinimumDate(myStartDate);
        cal->setDate ( QDate::fromString ( index.model()->data(index).toString(),"dd.MM.yyyy" ) );
    }
    break;
    case 3: {
        DelegateTextEdit *te = qobject_cast<DelegateTextEdit *> ( editor );
        if ( !te ) {
            QStyledItemDelegate::setEditorData ( editor, index );
            return;
        }
        te->setHtml( index.model()->data(index).toString() );
    }
    break;
    default: {
        QStyledItemDelegate::setEditorData ( editor, index );
        return;

    }
    break;
    }
}

void PalNotesDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    switch ( index.column() ) {
    case 2: {
        QDateEdit *cal = qobject_cast<QDateEdit *> ( editor );
        if ( !cal ) {
            QStyledItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, cal->date().toString("yyyy-MM-dd") );
    }
    break;
    case 3: {
        DelegateTextEdit *te = qobject_cast<DelegateTextEdit *> ( editor );
        if ( !te ) {
            QStyledItemDelegate::setModelData ( editor, model, index );
            return;
        }
        model -> setData ( index, te->toHtml().replace("<meta name=\"qrichtext\" content=\"1\" />", "<meta name=\"qrichtext\" content=\"0\" />") );
    }
    break;
    default: {
        QStyledItemDelegate::setModelData ( editor, model, index );
        return;

    }
    break;
    }
}

void PalNotesDelegate::emitCommitData()
{
    emit commitData ( qobject_cast<QWidget *> ( sender() ) );
}

void PalNotesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(optionV4.text);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize PalNotesDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());
    return QSize(doc.idealWidth()+10, doc.size().height());
}
