#include "delegatetextedit.h"
#include <QtGui>
#include <QtCore>

DelegateTextEdit::DelegateTextEdit(QWidget *parent) :
    QTextEdit(parent)
{

}


bool DelegateTextEdit::event(QEvent *event)
{
//     if (event->type() == QEvent::KeyPress) {
//         QKeyEvent *ke = static_cast<QKeyEvent *>(event);
//         if (ke->key() == QKeySequence(Qt::Key_Control + Qt::Key_Return)) {
//                 qDebug() << "jo";
//             // special tab handling here
//             return true;
//         }
//     }

    return QWidget::event(event);
}
