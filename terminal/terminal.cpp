/*!
  \file   terminal.h
  \brief  Терминал
  \author Aleksey Lekhtin
  \date   created 17.11.2014
*/

#include <QLabel>
#include <QDebug>
#include <QDir>
#include <QScrollBar>

#include "dbg.h"
#include "terminal.h"

//-----------------------------------------------------------
TERMINAL::TERMINAL(QWidget *parent) :
    QTextEdit(parent),
    m_number(1),
    m_name("XXX"),
    m_label(new QLabel(m_name, parent)),
    m_enabled(new QCheckBox(parent)),
    m_flog(nullptr),
    m_fbin(nullptr)
{
    set_activated(true);
    m_enabled->setChecked(true);
    m_enabled->setToolTip(tr("Enable/Disable serial port connection"));

    verticalScrollBar()->setEnabled(true);
    verticalScrollBar()->setRange(0, 100);
    setEnabled(false);
    setReadOnly(false);
    //setCenterOnScroll(false);
    moveCursor(QTextCursor::End);
    ensureCursorVisible();

    QTextDocument *doc = document();
    if (doc) doc->setMaximumBlockCount(1000);

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    pal.setColor(QPalette::Text, Qt::white);
    setPalette(pal);

    QFont fnt = font();
    fnt.setFamily("fixed");
    fnt.setStyleHint(QFont::Monospace);
    setFont(fnt);
    fnt.setBold(true);
    m_label->setFont(fnt);
    moveCursor(QTextCursor::End);
    this->set_label(this->name());

    setMaximumWidth(1800);
}

//-----------------------------------------------------------
TERMINAL::~TERMINAL()
{
    DBG(4);
    close_file_log();
 }

//-----------------------------------------------------------
void TERMINAL::slot_clear()
{
    clear();
}

//-----------------------------------------------------------
QLabel *TERMINAL::label()
{
    return m_label;
}

void TERMINAL::set_label(const QString  &text)
{
    if (!m_label) return;
    m_label->setText(text);
}

//-----------------------------------------------------------
void TERMINAL::set_number(int num)
{
    m_number = num;
}

int TERMINAL::number() const
{
    return m_number;
}

//-----------------------------------------------------------
void TERMINAL::set_name(const QString  &name)
{
    m_name = name;
    m_label->setText(m_name);
}

QString TERMINAL::name() const
{
    return m_name;
}

//-----------------------------------------------------------
QCheckBox *TERMINAL::enable_checkbox()
{
    return m_enabled;
}

//-----------------------------------------------------------
bool TERMINAL::open_file_log()
{
    QString fn = QString("%1-%2.log")
                 .arg(QCoreApplication::applicationName())
                 .arg(m_number);
    if (m_flog) { m_flog->close(); delete m_flog; }
    m_flog = new QFile(fn);
    if (!m_flog->open(QIODevice::ReadWrite | QIODevice::Append)) {
        delete m_flog;
        m_flog = nullptr;
        return false;
    }

    qDebug() << m_flog->fileName();
    QDateTime dtm = QDateTime::currentDateTime();
    QString dtmsg = QString("\n---- open --- %1 ---\n")
                    .arg(dtm.toString("dd.MM.yyyy hh:mm:ss"));

    m_flog->write(dtmsg.toUtf8(), dtmsg.size());
    m_flog->flush();

    //-----------------------------------------------------------
    if (m_number == 3)
    {
        fn = QString("%1-%2.bin")
                 .arg(QCoreApplication::applicationName())
                 .arg(m_number);
        if (m_fbin)
        {
            m_fbin->close();
            delete m_fbin;
        }
        m_fbin = new QFile(fn);
        if (!m_fbin->open(QIODevice::ReadWrite))
        {
            DBG(1, QString("Can't open binary protocol file: %1")
                .arg(qPrintable(QDir::toNativeSeparators(m_fbin->fileName()))));
            delete m_fbin;
            m_fbin = nullptr;
        }
    }

    return true;
}

//-----------------------------------------------------------
void TERMINAL::close_file_log()
{
    if (m_flog)
    {
        QDateTime dtm = QDateTime::currentDateTime();
        QString dtmsg = QString("\n---- close --- %1 ---\n")
                        .arg(dtm.toString("dd.MM.yyyy hh:mm:ss"));
        m_flog->write(dtmsg.toUtf8(), dtmsg.size());
        m_flog->close();
        delete m_flog;
        m_flog = nullptr;
    }

    if (m_fbin)
    {
        m_fbin->close();
        delete m_fbin;
        m_fbin = nullptr;
    }
}

//-----------------------------------------------------------
void TERMINAL::save_bin(const QByteArray  &data) const
{
    if (!m_fbin) return;
    m_fbin->write(data.data(), data.size());
}

//-----------------------------------------------------------
void TERMINAL::save_bin(const char *data, int size) const
{
    if (!m_fbin) return;
    m_fbin->write(data, size);
}

//-----------------------------------------------------------
void TERMINAL::save_bin(const char *data) const
{
    this->save_bin(data, qstrlen(data));
}


 //-----------------------------------------------------------
 //! Напечатать данные в терминале
 // \param[in] data - последовательность байт
 // \param[in] mode - тип отображения данных
 // \param[in] put_time - печатать метку времени
 //
 void TERMINAL::put_data(const QByteArray &data, PRINT_MODE mode)
 {
     bool put_time = false;
     moveCursor(QTextCursor::End);
     //if (!m_serial_port || !m_serial_port->isOpen()) return;
     if (put_time)
     {
         if (data.trimmed().size() > 0)
         {
             QString fmt = QLatin1String("dd.MM.yy hh:mm:ss");
             QString bf = QString("%1 ")
                          .arg(QDateTime::currentDateTime().toString(fmt));
             insertPlainText(bf);
         }
     }

     if (mode == TEXT_MODE)
     {
         insertPlainText(QString::fromUtf8(data));
     }
     else if (mode == HEX_MODE)
     {
         QByteArray ba = data.toHex().toUpper();
         QString buffer;
         buffer.reserve(ba.size() * 3);
         for(int i = 0; i < ba.size()-1; i+=2) {
             buffer.append(QStringLiteral("0x")).append(ba.at(i)).append(ba.at(i+1)).append(u' ');
         }
         insertPlainText(buffer);
     }

     //moveCursor(QTextCursor::End);
     QScrollBar *bar = verticalScrollBar();
     bar->setValue(bar->maximum());
     ensureCursorVisible();
 }

//-----------------------------------------------------------
//! Напечатать данные в терминале (TEXT_MODE)
// \param[in] data - последовательность байт
// \param[in] put_time - печатать метку времени
//
void TERMINAL::put_data(const QByteArray  &data)
{
    put_data(data, TERMINAL::TEXT_MODE);
}

//-----------------------------------------------------------
//! Напечатать данные в терминале
// \param[in] data - последовательность байт
// \param[in] mode - тип отображения данных
// \param[in] put_time - печатать метку времени
// \note добавляет LF (0xA)
//
void TERMINAL::put_data_hex_ln(const QByteArray  &data)
{
    put_data(data, TERMINAL::HEX_MODE);
    put_data(QByteArrayLiteral("\n"), TERMINAL::TEXT_MODE);
}

//-----------------------------------------------------------
//! Напечатать данные в терминале (TEXT_MODE)
// \param[in] data - последовательность байт
// \param[in] put_time - печатать метку времени
// \note добавляет LF (0xA)
//
void TERMINAL::put_data_ln(const QByteArray  &data)
{
    //bool put_time = false;
    if (data.size() > 0)
        put_data(data, TERMINAL::TEXT_MODE);
    put_data(QByteArrayLiteral("\n"), TERMINAL::TEXT_MODE);
}

//-----------------------------------------------------------
void TERMINAL::keyPressEvent(QKeyEvent *e)
{
    int keynum = e->key();
#if 0
    Qt::KeyboardModifiers mod = e->modifiers();
    QTextDocument *doc = document();
    if (keynum == Qt::Key_Return || keynum == Qt::Key_Enter)
    {
        if (!doc->lastBlock().text().isEmpty())
        {
            int num = doc->blockCount();
            QTextBlock bl = doc->findBlockByNumber(num-1);
            QString cmd = bl.text();
            cmd.replace("$ ", "");
            cmd = cmd.simplified();
            if (!cmd.isEmpty())
            {
                if (mod == Qt::ControlModifier)
                {
                    int pos = m_cmd_list.indexOf(cmd);
                    if (pos >= 0) m_cmd_list.removeAt(pos);
                    m_cmd_list << cmd;
                    m_cmd_list.removeDuplicates();
                }
                else
                {
                    QString msg = QString("%1\nREM> %2")
                                  .arg(QString(40,QChar('-'))).arg(cmd);
                }
            }
        }

        if (m_number == 2)
            put_data("\n");
        else
            put_data("\n$ ");
        e->accept();
        return;
    }
    else if (keynum == Qt::Key_Up || keynum == Qt::Key_Down)
    {
        int nline = -1;
        QTextBlock bl = doc->lastBlock();
        QString cmd = bl.text();
        cmd.replace("$ ", "");
        cmd = cmd.simplified();
        if (!cmd.isEmpty())
        {
            int pos = m_cmd_list.indexOf(cmd);
            if (pos >= 0)
            {
                if (keynum == Qt::Key_Up)
                    nline = pos - 1;
                else if (pos < m_cmd_list.size()-1)
                    nline = pos + 1;
            }
            else if (keynum == Qt::Key_Up)
            {
                nline = m_cmd_list.size()-1;
            }
        }
        else if (keynum == Qt::Key_Up)
        {
            nline = m_cmd_list.size()-1;
        }

        //-----------------------------------------------------------
        if (keynum == Qt::Key_Up && nline >= 0)
        {
            QTextCursor cr = textCursor();
            cr.movePosition(QTextCursor::End);
            cr.select(QTextCursor::LineUnderCursor);
            cr.removeSelectedText();
            if (m_cmd_list.size() > 0)
            {
                QString msg = QString("$ ") + m_cmd_list.at(nline);
                put_data(msg.toUtf8(), TERMINAL::TEXT_MODE);
            }
        }
        else if (keynum == Qt::Key_Down)
        {
            QTextCursor cr = textCursor();
            cr.movePosition(QTextCursor::End);
            cr.select(QTextCursor::LineUnderCursor);
            cr.removeSelectedText();
            if (m_cmd_list.size() > 0)
            {
                QString msg = QString("$ ");
                if (nline >= 0) msg += m_cmd_list.at(nline);
                put_data(msg.toUtf8(), TERMINAL::TEXT_MODE);
            }
        }

        e->accept();
        return;
    }
    else if (keynum == Qt::Key_Left)
    {
        QTextCursor cr = textCursor();
        int col = cr.columnNumber();
        if (col <= 2)
        {
            e->accept();
            return;
        }
    }
    else if (keynum == Qt::Key_Right)
    {
        QTextCursor cr = textCursor();
        QTextBlock bl = cr.block();
        int col = cr.columnNumber();
        if (col >= bl.length())
        {
            e->accept();
            return;
        }
    }
    else if (keynum == Qt::Key_Home)
    {
        QTextCursor cr = textCursor();
        cr.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        int pos = cr.position();
        cr.setPosition(pos + 2);
        setTextCursor(cr);
        e->accept();
        return;
    }
    else if (keynum == Qt::Key_Backspace)
    {
        QTextCursor cr = textCursor();
        int col = cr.columnNumber();
        if (col <= 2)
        {
            e->accept();
            return;
        }
    }
#endif

    //-----------------------------------------------------------
    switch(keynum)
    {
    //case Qt::Key_End:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        e->ignore();
        break;
    default:
        QTextEdit::keyPressEvent(e);
        break;
    }
}

//-----------------------------------------------------------
void TERMINAL::set_activated(bool status, bool enable_log)
{
    if (status)
    {
        setEnabled(true);
        setFocus();
        moveCursor(QTextCursor::End);
        if (enable_log) open_file_log();
        return;
    }

    //setEnabled(false);
    close_file_log();
}
