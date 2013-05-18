/*****************************************************************************
 * PokerTH - The open source texas holdem engine                             *
 * Copyright (C) 2006-2011 Felix Hammer, Florian Thauer, Lothar May          *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/
#include "mymessagedialogimpl.h"
#include <QtCore>
#include "mydbhandler.h"

using namespace std;


myMessageDialogImpl::myMessageDialogImpl(QWidget *parent, myDBHandler *db)
    : QDialog(parent), myDB(db)
{
#ifdef __APPLE__
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::WindowSystemMenuHint | Qt::CustomizeWindowHint | Qt::Dialog);
#endif
    setupUi(this);
}

void myMessageDialogImpl::show(int reminderId, bool showCheckBox, QString checkBoxString)
{
    currentReminderId=reminderId;

    if(showCheckBox) {
        checkBox->show();
        checkBox->setText(checkBoxString);
    } else checkBox->hide();

    QDialog::show();

}

void myMessageDialogImpl::accept()
{
    if(checkBox->isChecked()) {
        myDB->removeReminderById(currentReminderId);
    }

    QDialog::accept();
}

void myMessageDialogImpl::reject()
{
    QDialog::reject();
}

void myMessageDialogImpl::setTextFormat(Qt::TextFormat tf)
{
    label->setTextFormat(tf);
}

void myMessageDialogImpl::setText(QString s)
{
    label->setText(s);
}

void myMessageDialogImpl::setIcon(QMessageBox::Icon i)
{
    QMessageBox	tmpBox;
    tmpBox.setIcon(i);

    label_icon->setPixmap(tmpBox.iconPixmap());
}
