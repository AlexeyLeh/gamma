/*!
  \file   misc.cpp
  \author Aleksey Lekhtin
  \date   16.05.2025
  \brief  Мелкие функции
*/

#include <math.h>

#include <QtWidgets>

#include "utils.h"

//-----------------------------------------------------------
// Центрирование строки
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина центрирования
// \return Отцентроированная строка
//!
QString UTILS::str_center(const QString & str, int width)
{
    int str_len = str.size();
    if(str_len > width) return str;
    int ww = (width - str_len)/2.0;
    ww = width - ceil(ww);
    return  QString("%1").arg(str.leftJustified(ww,' '), width);
}

//-----------------------------------------------------------
// Выравнивание строки влево
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина выравнивания
// \return результирующая строка
//!
QString UTILS::str_left(const QString & str, int width)
{
    return  QString("%1").arg(str, width*-1);
}

//-----------------------------------------------------------
// Выравнивание строки вправо
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина выравнивания
// \return результирующая строка
//!
QString UTILS::str_right(const QString & str, int width)
{
    return  QString("%1").arg(str, width);
}

//-----------------------------------------------------------
QString UTILS::split_message(const QString &msg)
{
    QStringList strs = msg.split(QChar(';'));
    if (strs.size() <= 1) {
        return msg;
    }
    QString outmsg;
    for(const auto &str : strs) {
        outmsg += str.trimmed() + QStringLiteral("\n");
    }
    return outmsg;
}

//-----------------------------------------------------------
// Seconds to minute
// \param sec - seconds
// \return Minute as string
//
QString UTILS::secs_to_minute(int sec)
{
    QString ctim;
    if(sec > 60) {
        int mm = sec / 60;
        int ss = sec - (sec / 60) * 60;
        ctim = QString("%1 %2 %3 %4")
                .arg(mm)
                .arg("min")
                .arg((ss > 0 ? QString::number(ss) : QStringLiteral("")))
                .arg((ss > 0 ? "sec" : QStringLiteral("")));
    } else {
        ctim = QString("%1 %2").arg(sec).arg(("sec"));
    }
    return ctim.trimmed();
}

//-------------------------------------------------------------
// Декодирование IEE754 float
// \param[in] data - четыре байта для  декодирования
// \return значение
//
float UTILS::decode_ieee754(const char *data)
{
    float value;
    if(!data) return 0.0f;
    QByteArray ba(data, 4);
    QDataStream s(&ba, QIODevice::ReadOnly);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    //s.setByteOrder(QDataStream::LittleEndian);
    s >> value;
    return value;
}

//-------------------------------------------------------------
// Кодирование в IEE754 float
// \param[in] value - значение для кодирования
// \return четыре закодированныз байта
//
char *UTILS::encode_ieee754(float value)
{
    static char data[5] = {0};
    memset(data, 0, sizeof(data));
    if(value == 0.0f) return data;
    QByteArray ba/*(data, 4)*/;
    QDataStream s(&ba, QIODevice::WriteOnly);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    s << value;
    memcpy(data, ba.constData(), 4);
    return data;
}

//-------------------------------------------------------------
/// Расчет контрольной суммы
///
quint16 UTILS::calc_crc_sum(const char *data, quint16 data_size)
{
    quint16 sum = 0;
    if(!data) return 0;
    for(quint16 i = 0; i < data_size; ++i)
    {
        sum += (quint8)data[i];
    }

    return sum;
}

//-------------------------------------------------------------
QString UTILS::split_path(const QString &path)
{
    auto len = path.length();
    if (len <= 50) {
        return path;
    }
    auto num = len / 2;
    if (num < 25) {
        num = 50;
    }
    auto pos = path.indexOf(QRegularExpression("[\\\\/]"), num);
    if (pos == -1) {
        return path;
    }
    auto str = path.left(pos+1);
    str += QStringLiteral("\n");
    str += path.right(path.length() - pos - 1);
    str += QStringLiteral("/");
    return QDir::toNativeSeparators(str);
}
