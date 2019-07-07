#include "mymetronomespinbox.h"
#include "QtCore"

MyMetronomeSpinBox::MyMetronomeSpinBox(QWidget *parent)
{

}

void MyMetronomeSpinBox::setMyPlayButton(QPushButton* b)
{
    myPlayButton = b;
}

void MyMetronomeSpinBox::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
//        qDebug() << "space pressed";
        myPlayButton->click();
        QSpinBox::keyPressEvent(event);
    } else {
        QSpinBox::keyPressEvent(event);
    }
}
