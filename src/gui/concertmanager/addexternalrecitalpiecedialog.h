#ifndef ADDEXTERNALRECITALPIECEDIALOG_H
#define ADDEXTERNALRECITALPIECEDIALOG_H

#include <QDialog>

class ConfigFile;

namespace Ui
{
class AddExternalRecitalPieceDialog;
}

class AddExternalRecitalPieceDialog : public QDialog
{
    Q_OBJECT
public:
    AddExternalRecitalPieceDialog(int, ConfigFile*, QWidget *parent = 0);
    ~AddExternalRecitalPieceDialog();

public slots:
    void accept();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddExternalRecitalPieceDialog *ui;
    ConfigFile *myConfig;
    int currentRecitalId;
};

#endif // ADDEXTERNALRECITALPIECEDIALOG_H
