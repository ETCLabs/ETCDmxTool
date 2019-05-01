#include "updatedialog.h"

#include <QVBoxLayout>

UpdateDialog::UpdateDialog(QWidget *parent): QDialog(parent)
{
    this->setWindowTitle(tr("Update Gadget2"));
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    m_bar = new QProgressBar(this);
    m_bar->setMinimum(0);
    m_bar->setMaximum(0);

    m_label = new QLabel(this);
    m_label->setMinimumHeight(200);
    m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_label->setWordWrap(true);

    m_button = new QPushButton(this);
    m_button->setText(tr("Done"));
    m_button->setEnabled(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->addWidget(m_bar);
    layout->addWidget(m_button);

    this->setLayout(layout);
}

void UpdateDialog::setStatusText(const QString &text)
{
    m_label->setText(text);
}

void UpdateDialog::doneAndRestart()
{
    m_button->setText(tr("Restart"));
    m_button->setEnabled(true);
}
