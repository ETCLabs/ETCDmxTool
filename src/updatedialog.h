#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    UpdateDialog(QWidget *parent = Q_NULLPTR);

public slots:
    void setStatusText(const QString &text);
    void doneAndRestart();
private:
    QProgressBar *m_bar;
    QLabel *m_label;
    QPushButton *m_button;
};

#endif // UPDATEDIALOG_H
