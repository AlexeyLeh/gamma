/*!
  \file   lineedit.cpp
  \brief  Работа с QLineEdit
  \author Aleksey Lekhtin
  \date   created 25.02.2015
*/

#include <QLayout>

#include "lineedit.h"

//-----------------------------------------------------------
LineEdit30::LineEdit30(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

//-----------------------------------------------------------
QSize LineEdit30::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    auto m = textMargins();

    auto sz = QSize(r.width() * 30 + m.left() + m.right(), QLineEdit::sizeHint().height() - m.top() - m.bottom());
    return sz;

    //// return QSize(r.width() * 30 + m.left() + m.right(), QLineEdit::sizeHint().height() - m.top() - m.bottom());
    //// QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit22::LineEdit22(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

//-----------------------------------------------------------
QSize LineEdit22::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    auto m = textMargins();
    return QSize(r.width() * 22 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit20::LineEdit20(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

LineEdit20::LineEdit20(const QString& text, QWidget *parent) : LineEdit20(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
}

LineEdit20::LineEdit20(const QString& text, Qt::Alignment align, QWidget *parent) : LineEdit20(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
    setAlignment(align);
}

//-----------------------------------------------------------
QSize LineEdit20::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    auto m = textMargins();
    return QSize(r.width() * 20 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit16::LineEdit16(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

LineEdit16::LineEdit16(const QString& text, QWidget *parent) : LineEdit16(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
}

LineEdit16::LineEdit16(const QString& text, Qt::Alignment align, QWidget *parent) : LineEdit16(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
    setAlignment(align);
}

//-----------------------------------------------------------
QSize LineEdit16::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    auto m = textMargins();
    return QSize(r.width() * 16 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit12::LineEdit12(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

LineEdit12::LineEdit12(const QString& text, QWidget *parent) : LineEdit12(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
}

LineEdit12::LineEdit12(const QString& text, Qt::Alignment align, QWidget *parent) : LineEdit12(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
    setAlignment(align);
}

//-----------------------------------------------------------
QSize LineEdit12::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    //// int lm, rm, tm, bm;
    //// getTextMargins(&lm, &tm, &rm, &bm);
    //// return QSize(r.width() * 12 + lm + rm, QLineEdit::sizeHint().height());
    auto m = textMargins();
    return QSize(r.width() * 12 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit8::LineEdit8(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

LineEdit8::LineEdit8(const QString& text, QWidget *parent) : LineEdit8(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
}

LineEdit8::LineEdit8(const QString& text, Qt::Alignment align, QWidget *parent) : LineEdit8(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
    setAlignment(align);
}

//-----------------------------------------------------------
QSize LineEdit8::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    //// int lm, rm, tm, bm;
    //// getTextMargins(&lm, &tm, &rm, &bm);
    //// return QSize(r.width() * 8 + lm + rm, QLineEdit::sizeHint().height());
    auto m = textMargins();
    return QSize(r.width() * 8 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit5::LineEdit5(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    if(parent)
    {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), parent->palette().color(parent->backgroundRole()));
        setPalette(pal);
    }
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setContentsMargins(1, 1, 1, 1);
}

LineEdit5::LineEdit5(const QString& text, QWidget *parent) : LineEdit5(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
}

LineEdit5::LineEdit5(const QString& text, Qt::Alignment align, QWidget *parent) : LineEdit5(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setText(text);
    setAlignment(align);
}

QSize LineEdit5::sizeHint() const
{
    QRect r = fontMetrics().boundingRect("@");
    auto m = textMargins();
    return QSize(r.width() * 5 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//-----------------------------------------------------------
LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{
    if (layout()) layout()->setSizeConstraint(QLayout::SetFixedSize);
    setContentsMargins(1, 1, 1, 1);
}

QSize LineEdit::sizeHint() const
{
    return QLineEdit::sizeHint();
    // QRect r = fontMetrics().boundingRect("@");
    // auto m = textMargins();
    // return QSize(r.width() * 12 + m.left() + m.right(), QLineEdit::sizeHint().height());
}

//// QSizePolicy LineEdit::sizePolicy() const
//// {
////     return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//// }

// //-----------------------------------------------------------
// QSize LineEdit::minimumSizeHint() const
// {
//    return sizeHint();
// }

// //-----------------------------------------------------------
// QSize LineEdit::maximumSizeHint() const
// {
//    return sizeHint();
// }

//-----------------------------------------------------------
// void LineEdit::slot_resizeByContents()
// {
//    setFixedWidth(sizeHint().width());
//    if(QWidget *p = parentWidget())
//       p->adjustSize();
// }
