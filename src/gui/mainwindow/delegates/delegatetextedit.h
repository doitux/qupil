#ifndef DELEGATETEXTEDIT_H
#define DELEGATETEXTEDIT_H

#include <QTextEdit>

class DelegateTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit DelegateTextEdit(QWidget *parent = 0);
    bool event(QEvent *event);

signals:
    void editingFinished();

public slots:

};

#endif // DELEGATETEXTEDIT_H
