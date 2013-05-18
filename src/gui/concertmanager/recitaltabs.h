#ifndef RECITALTABS_H
#define RECITALTABS_H

#include <QWidget>
#include <QtGui>
#include <QtCore>

class ConcertManagerDialogImpl;

namespace Ui
{
class RecitalTabs;
}

class ConfigFile;

class RecitalTabs : public QWidget
{
    Q_OBJECT
public:
    RecitalTabs(QWidget *parent = 0, ConcertManagerDialogImpl* =0);
    ~RecitalTabs();

    void setRecitalId(int id);
    void setMyConfig(ConfigFile* c) {
        myConfig = c;
    };
    int getRecitalId() {
        return recitalId;
    };
    int getCurrentPieceId();
    int getPiecesCount();
    QAbstractItemModel* getListModel();
    int getCompletePiecesDuration() {
        return completePiecesDuration;
    }
    int getCompleteAllInAllDuration() {
        return completeAllInAllDuration;
    }
    void setDate(QString);
    QString getDate();
    void setTime(QString);
    void setLocation(QString);
    QString getLocation();
    void setOrganisator(QString);
    void setAccompanist(QString);
    void setState(int);
    void refreshPieces();

public slots:

    void pieceUp();
    void pieceDown();
    void saveSorting();
    void recitalStateChanged(int);
    void selectFirstItem();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecitalTabs *ui;
    int recitalId;
    ConfigFile *myConfig;
    bool breakComboBoxStateConnection;
    ConcertManagerDialogImpl *myCM;
    int completePiecesDuration;
    int completeAllInAllDuration;
};

#endif // RECITALTABS_H
