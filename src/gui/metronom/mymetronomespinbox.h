#ifndef MYMETRONOMESPINBOX_H
#define MYMETRONOMESPINBOX_H
#include "QSpinBox"
#include "QPushButton"
#include "QKeyEvent"
#include "QGroupBox"

class MyMetronomeSpinBox : public QSpinBox
{
public:
    MyMetronomeSpinBox(QWidget *parent = nullptr);

    void setMyPlayButton(QPushButton *);
    void keyPressEvent(QKeyEvent *);

private:
    QPushButton *myPlayButton;
};

#endif // MYMETRONOMESPINBOX_H
