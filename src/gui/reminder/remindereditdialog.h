#ifndef REMINDEREDITDIALOG_H
#define REMINDEREDITDIALOG_H

#include <QDialog>
#include <QtWidgets>
namespace Ui
{
class ReminderEditDialog;
}

class ReminderEditDialog : public QDialog
{
    Q_OBJECT
public:
    ReminderEditDialog(QWidget *parent = 0, int mode = 0, int reminderid = 0);
    ~ReminderEditDialog();

    void accept();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReminderEditDialog *ui;
    int myMode; // 0 = add, 1 = edit
    int currentReminderId;
};

#endif // REMINDEREDITDIALOG_H
