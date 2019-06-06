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
#ifndef PUPILTABWIDGET_H
#define PUPILTABWIDGET_H

#include <QtGui>
#include <QtWidgets>
#include <QtCore>
#include <vector>

class ConfigFile;
class mainWindowImpl;
class PalNotesModel;
class PalPiecesModel;
class PupilContActivityModel;
class PupilSingularActivityModel;
/**
	@author Felix Hammer <f.hammer@gmx.de>
*/
class PupilTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    PupilTabWidget(QWidget*);

    ~PupilTabWidget();

    void setMyW ( mainWindowImpl* theValue );
    void setMyConfig ( ConfigFile* theValue );
    void setCurrentPupilId ( int theValue ) {
        currentPupilId = theValue;
    }
    int getCurrentPupilId() const {
        return currentPupilId;
    }
    void setCurrentPalId ( int theValue ) {
        currentPalId = theValue;
    }
    int getCurrentPalId() const {
        return currentPalId;
    }

public slots:

    void init();
    void loadComboBoxItems();

    void loadPupilData();
    void loadPupilPersonalData();
    void loadPupilActivity();
    void loadPupilRentSheetMusic();
    void savePupilPersonalData();
    void calcPupilAge(QString birthdayString);
    void calcSinceFirstLesson(QString firstLessonString);
    void pupilBirthdayChanged( QDate );
    void pupilFirstLessonChanged( QDate );

    void loadPalNotes( int palId );
    void loadPalPieces( int palId );
    void addNewPiece();
    void delPiece();
    void addNewNote();
    void delNote();
    void addNewContActivity();
    void delContActivity();
    void addNewSingularActivity();
    void delSingularActivity();
    void piecesSelectionChanged( int );
    void notesSelectionChanged( int );
    void sheetBackFromPupil();
    void notesItemSelected(const QModelIndex & current, const QModelIndex & previous);
    void refreshPiecesComposerCompleter();
    void refreshPieces();

    void notesTextBold();
    void notesTextItalic();
    void notesTextUnderlined();
    void notesTextColor();

    void palNotesEditSelectionChanged();

    //HACK because of regression in Qt-5.x where hidden columns are shown after editing finished via delegate and scrollState is not saved during editing
    void palNotesRestoreViewLayout();
    void palPiecesRestoreViewLayout();
    void activityRestoreViewLayout();
    void palNotesSaveScrollState();
    void palPiecesSaveScrollState();
    void activitySaveScrollState();


private:
    mainWindowImpl *myW;
    int currentPupilId;
    int currentPalId;
    ConfigFile *myConfig;
    bool saveOk;
    PalNotesModel *myPalNotesModel;
    PalPiecesModel *myPalPiecesModel;
    PupilSingularActivityModel *myPupilSingularActivityModel;
    PupilContActivityModel *myPupilContActivityModel;
    QItemSelectionModel *myPalNotesSelectionModel;
    int palNotesTreeViewVerticalScrollValue;
    int palPiecesTreeViewVerticalScrollValue;
    int continousActivityTreeViewVerticalScrollValue;
    int singularActivityTreeViewVerticalScrollValue;
};

#endif
