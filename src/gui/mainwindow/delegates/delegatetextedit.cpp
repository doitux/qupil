#include "delegatetextedit.h"
#include <QtGui>
#include <QtCore>

DelegateTextEdit::DelegateTextEdit(QWidget *parent) :
    QTextEdit(parent)
{

}


bool DelegateTextEdit::event(QEvent *event)
{
     if (event->type() == QEvent::KeyPress) {
         QKeyEvent *ke = static_cast<QKeyEvent *>(event);
         if (ke->key() == Qt::Key_Return && (ke->modifiers() & Qt::ControlModifier)) {
             emit editingFinished();
             return true;
         }
     }

    return QTextEdit::event(event);
}
