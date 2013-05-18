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
#ifndef MYMESSAGEDIALOGIMPL_H
#define MYMESSAGEDIALOGIMPL_H

#include "ui_mymessagedialog.h"
#include <QMessageBox>

class myDBHandler;
//class QMessageBox;

class myMessageDialogImpl: public QDialog, public Ui::myMessageDialog
{
    Q_OBJECT
public:

    myMessageDialogImpl(QWidget *parent = 0, myDBHandler *myDb = 0);

public slots:

    void show(int reminderId, bool showCheckBox = false, QString checkBoxString = "");
    void accept();
    void reject();
    void setTextFormat(Qt::TextFormat);
    void setText(QString);
    void setIcon(QMessageBox::Icon);

private:

    myDBHandler *myDB;
    int currentReminderId;

};

#endif

// messages --> id: content
//1: leave lobby during online game: Question(Do you really wanna leave?)
//2: join invite only game: Info(You can invite people with right click ...)
//3: reciev invite to game: Question(You've been invited to the game <b>%1</b> by <b>%2</b>.<br>Do you want to join this game?)
//4: click ignore player: Question(Do you really want to put this player on ignore List?)
//5: add a broken (fields missing) game table style: Info(Selected game table style \"%1\" seems to be incomplete or defective. \n\nThe value(s) of \"%2\" is/are missing. \n\nPlease contact the game table style builder %3.)
//6: close gametable: Do you really wanna quit?
