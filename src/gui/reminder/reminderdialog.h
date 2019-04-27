#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets>

namespace Ui
{
class ReminderDialog;
}

class ReminderDialog : public QDialog
{
    Q_OBJECT
public:
    ReminderDialog(QWidget *parent = 0);
    ~ReminderDialog();

public slots:

    void refreshList();
    void addReminder();
    void delReminder();
    void editReminder();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReminderDialog *ui;
};

#endif // ReminderDIALOG_H
