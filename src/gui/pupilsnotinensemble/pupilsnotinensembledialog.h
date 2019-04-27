#ifndef PUPILSNOTINENSEMBLEDIALOG_H
#define PUPILSNOTINENSEMBLEDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets>

class ConfigFile;

namespace Ui
{
class PupilsNotInEnsembleDialog;
}

class PupilsNotInEnsembleDialog : public QDialog
{
    Q_OBJECT
public:
    PupilsNotInEnsembleDialog(ConfigFile *, QWidget *parent = 0);
    ~PupilsNotInEnsembleDialog();

public slots:

    void showPopupMenu(const QPoint);
    void addReminder();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PupilsNotInEnsembleDialog *ui;
    ConfigFile *myConfig;
    QMenu *pupilPopupMenu;
    QAction *addReminderAction;
};

#endif // PUPILSNOTINENSEMBLEDIALOG_H
