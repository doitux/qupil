#ifndef FINISHRECITALDIALOG_H
#define FINISHRECITALDIALOG_H

#include <QDialog>

namespace Ui
{
class FinishRecitalDialog;
}

class FinishRecitalDialog : public QDialog
{
    Q_OBJECT
public:
    FinishRecitalDialog(QWidget *parent = 0);
    ~FinishRecitalDialog();

    bool getActivityAnswer();
    bool getPieceAnswer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FinishRecitalDialog *ui;
};

#endif // FINISHRECITALDIALOG_H
