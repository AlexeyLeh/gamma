/*!
  \file   progress.cpp
  \author Aleksey Lekhtin
  \date   07.05.2015
  \brief  Simple progress bar
*/

#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QTime>
#include <QtDebug>

#include "dbg.h"
#include "progress.h"

// //-----------------------------------------------------------
// //! Создать окно сообщений
// // \param[in] parent  - parent
// //
// PBAR::PBAR(QWidget *parent) :
//    QProgressDialog(parent)
// {
//    if(parent) m_font = parent->font();
//    setFont(m_font);
//    setModal(true);
//    setAutoClose(false);
//    setAutoReset(false);
//    //setMinimumDuration(0);
//
//    QSize sz = size();
//    sz.setWidth(350);
//    setMinimumSize(sz);
//    setMaximumSize(900, 700);
//    resize(sz);
//    connect(this, SIGNAL(signal_set_value(int,int)),
//             this, SLOT(slot_set_value(int,int)));
//
//    //connect(this, SIGNAL(canceled()),
//    //         this, SLOT(slot_cancel()));
//
//    reset();
//    hide();
//    setMinimumDuration(0);
//
//    QString style = QStringLiteral("QProgressBar {"
//        "border: 2px solid grey;"
//        "border-radius: 5px;"
//        "text-align: center; }");
//    setStyleSheet(style);
// }

//-----------------------------------------------------------
PBAR::PBAR(const QString & title, const QString & msg_text, QWidget *parent) :
    QProgressDialog(parent)
{
    if(parent) m_font = parent->font();
    setFont(m_font);
    setModal(true);
    setAutoClose(false);
    setAutoReset(false);

    // QSize sz = size();
    // sz.setWidth(800);
    // setMinimumSize(sz);
    // resize(sz);

    adjustSize();
    setMinimumWidth(600);
    setMaximumSize(1080, 600);
    connect(this, SIGNAL(signal_set_value(int,int)),
             this, SLOT(slot_set_value(int,int)));
    // connect(this, SIGNAL(canceled()),
    //          this, SLOT(slot_cancel()));

    reset();
    setMinimumDuration(0);

    //   QString style = QStringLiteral("QProgressBar {"
    //                                "border: 2px solid grey;"
    //                                "border-radius: 5px;"
    //                                "text-align: center; }");
    //QString style = QStringLiteral("QProgressBar { text-align: center; }");
    QString style = QStringLiteral("QProgressBar { text-align:center;}"
                                  "QProgressBar::label { text-align:center; font-size:22px;}"
                                );
    setStyleSheet(style);
    progress(title, msg_text);
}

//-----------------------------------------------------------
//! Удалить окно
//
PBAR::~PBAR()
{
    DBG(4);
    disconnect(this, SIGNAL(signal_set_value(int,int)));
}

//-----------------------------------------------------------
//! Создать окно индикатора
// \param[in] title         - заголовок
// \param[in] msg_text      - текст
// \param[in] start_value   - начальное значение
// \param[in] max_value     - максимальное значение
// \param[in] cancel_button - наличие кнопик отмены
// \param[in] parent        - родителькое окно
// \note
//  если (start_value == 0 && start_value == max_value)
//  индикатор будет просто перемещаться
//
void PBAR::progress(const QString & title,
                     const QString & msg_text,
                     int             start_value,
                     int             max_value,
                     bool            cancel_button)
{
    // QLabel *label_msg = new QLabel(msg_text);
    // label_msg->setFont(m_font);
    // setLabel(label_msg);
    setLabelText(msg_text);

    setCancelButtonText(QString());
    setCancelButton(0);
    if(cancel_button)
        setCancelButton(new QPushButton(PBAR::tr("Cancel")));

    if(!title.isEmpty()) setWindowTitle(title);

    if(start_value == max_value)
    {
        setMinimum(0);
        setMaximum(0);
        start_value = 1;
    }
    else if(start_value > max_value)
    {
        max_value = start_value + 2;
        setMinimum(1);
        setMaximum(max_value);
    }
    else
    {
        if(start_value == 0 || start_value == 1)
        {
            start_value = 1;
            ++max_value;
        }

        setMinimum(start_value);
        setMaximum(max_value);
    }

    setValue(start_value);
    show();
    QApplication::processEvents();
}

//-----------------------------------------------------------
//! Отрисовать новое значение индикатора
// \param[in] position - положение индикатора
// \param[in] maxpos   - переустановить максимальное значение
//
void PBAR::progress(int position, int maxpos)
{
    if(maxpos > 0)
    {
        setMinimum(0);
        setMaximum(maxpos);
        setValue(position);
        return;
    }
    //setValue(position + 1);
    if(position > value())
        setValue(position);
}

//-----------------------------------------------------------
//! Задать 100% положение индикатора
//
void PBAR::progress_100()
{
    int nmax = maximum();
    int nmin = minimum();
    if(nmin == nmax)
    {
        setMinimum(1);
        setMaximum(100);
        setValue(100);
        return;
    }
    setValue(nmax);

    QTime qtm = QTime::currentTime().addMSecs(500);
    while(QTime::currentTime() < qtm)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
}


//-----------------------------------------------------------
//! Задать текст - описание
//
void PBAR::set_text(const QString & msg_text)
{
    setLabelText(msg_text);
}


//-----------------------------------------------------------
//! Закрыть сообщение
//
void PBAR::end(void)
{
    hide();
    QApplication::processEvents();
}

//-----------------------------------------------------------
//! Задать положение индикатора
// param[in] value - задать новое значение
// param[in] max_value - задать новое максимальное значение
//
void PBAR::slot_set_value(int value, int max_value)
{
    progress(value, max_value);
}

//-----------------------------------------------------------
//! Задать фонт
// \param[in] fnt - фонт
//
void PBAR::set_font(QFont fnt)
{
    m_font = fnt;
    setFont(m_font);
}

// //-----------------------------------------------------------
// //! Cancel
// //
// void PBAR::slot_cancel()
// {
//    //qApp->setOverrideCursor(Qt::WaitCursor);
// }
