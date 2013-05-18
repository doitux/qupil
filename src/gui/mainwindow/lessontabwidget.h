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
#ifndef PALTABWIDGET_H
#define PALTABWIDGET_H

#include <QtGui>
#include <QtCore>
#include <vector>

class ConfigFile;
class mainWindowImpl;

/**
	@author Felix Hammer <f.hammer@gmx.de>
*/
class LessonTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    LessonTabWidget(QWidget*);

    ~LessonTabWidget();

    void setMyW ( mainWindowImpl* theValue );
    void setMyConfig ( ConfigFile* theValue );
    void setCurrentLessonId ( int theValue ) {
        currentLessonId = theValue;
    }
    int getCurrentLessonId() const {
        return currentLessonId;
    }
    QList< QStringList > getPupilPalListBeforeEdit() const {
        return pupilPalListBeforeEdit;
    }
    QString getAutoLessonNameBeforeEdit() const {
        return autoLessonNameBeforeEdit;
    }

public slots:

    void init();
    void loadComboBoxItems();

    void loadLessonData();

    void loadLessonDetails();
    void saveLessonDetails();
    void refreshAvailablePupilList();
    void addPupilToLesson();
    void delPupilFromLesson();
    void setSingleLesson();
    void setGroupLesson();
    void setEnsembleLesson();
    void setSteadyLessonDate();
    void setUnsteadyLessonDate();
    void lessonStartTimeChanged( QTime );
    void lessonStopTimeChanged( QTime );
    void lessonDurationChanged();
    void lessonLocationChanged();
    void calcLessonDuration();
    QString updateAutoLessonName();

private:
    mainWindowImpl *myW;
    int currentLessonId;
    ConfigFile *myConfig;
    QList<QStringList> pupilPalListBeforeEdit;
    QStringList pupilListBeforeEdit;
    QStringList tempPupilList;
    QString autoLessonNameBeforeEdit;

    bool saveOk;
};

#endif
