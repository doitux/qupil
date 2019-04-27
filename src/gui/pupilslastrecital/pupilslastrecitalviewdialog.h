#ifndef PUPILSLASTRECITALVIEWDIALOG_H
#define PUPILSLASTRECITALVIEWDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets>
class ConfigFile;

namespace Ui
{
class PupilsLastRecitalViewDialog;
}

class PupilsLastRecitalViewDialog : public QDialog
{
    Q_OBJECT
public:
    PupilsLastRecitalViewDialog(ConfigFile *, QWidget *parent = 0);
    ~PupilsLastRecitalViewDialog();

public slots:

    void showPopupMenu(const QPoint point);
    void addReminder();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PupilsLastRecitalViewDialog *ui;
    ConfigFile *myConfig;
    QMenu *pupilPopupMenu;
    QAction *addReminderAction;
};

#endif // PUPILSLASTRECITALVIEWDIALOG_H
