#ifndef SELECTPIECESDIALOG_H
#define SELECTPIECESDIALOG_H

#include <QDialog>
class ConfigFile;

namespace Ui
{
class SelectPiecesDialog;
}

class SelectPiecesDialog : public QDialog
{
    Q_OBJECT
public:
    SelectPiecesDialog(QWidget *parent = nullptr, ConfigFile *c = nullptr);
    ~SelectPiecesDialog();

    int exec(int);
    void accept();
    QList<int> getSelectedPieces() {
        return selectedPieces;
    }
    int getCurrentRecitalId() {
        return currentRecitalId;
    }

public slots:
    void loadPieces();


protected:
    void changeEvent(QEvent *e);

private:
    int currentRecitalId;
    QList<int> selectedPieces;
    ConfigFile *myConfig;
    Ui::SelectPiecesDialog *ui;

};

#endif // SELECTPIECESDIALOG_H
