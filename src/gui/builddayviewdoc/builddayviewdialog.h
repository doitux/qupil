#ifndef BUILDDAYVIEWDIALOG_H
#define BUILDDAYVIEWDIALOG_H

#include <QDialog>

class QTextDocument;
class ConfigFile;

namespace Ui
{
class BuildDayViewDialog;
}

class BuildDayViewDialog : public QDialog
{
    Q_OBJECT
public:
    BuildDayViewDialog(ConfigFile *, QWidget *parent = 0);
    ~BuildDayViewDialog();

    QTextDocument* returnDoc();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::BuildDayViewDialog *ui;
    ConfigFile *myConfig;
};

#endif // BUILDDAYVIEWDIALOG_H
