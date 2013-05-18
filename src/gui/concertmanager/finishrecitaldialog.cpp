#include "finishrecitaldialog.h"
#include "ui_finishrecitaldialog.h"

FinishRecitalDialog::FinishRecitalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinishRecitalDialog)
{
    ui->setupUi(this);
}

FinishRecitalDialog::~FinishRecitalDialog()
{
    delete ui;
}

void FinishRecitalDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool FinishRecitalDialog::getActivityAnswer()
{
    return ui->radioButton_activityYes->isChecked();
}

bool FinishRecitalDialog::getPieceAnswer()
{
    return ui->radioButton_PieceYes->isChecked();
}
