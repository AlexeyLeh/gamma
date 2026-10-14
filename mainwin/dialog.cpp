/*!
  \file   dialog.cpp
  \author Aleksey Lekhtin
  \date   16.02.2026
  \brief  Методы работы с диалогами (для удобства)
*/

#include <string>

#include <QWidget>

#include "dbg.h"
#include "mainwindow.h"

//---------------------------------------------------------
QList<QWidget *> MW::make_dialog_two_button(QDialog *dlg, const QString &text1, const QString &text2)
{
    QPushButton *apply_button = new QPushButton(text1);
    QPushButton *cancel_button = new QPushButton(text2);

    MW::connect(apply_button,  &QPushButton::clicked, dlg, &QDialog::accept);
    MW::connect(cancel_button, &QPushButton::clicked, dlg, &QDialog::reject);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(apply_button);
    hlayout->addWidget(cancel_button);

    auto *wgt = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addLayout(hlayout);

    return {wgt, apply_button, cancel_button};
}
