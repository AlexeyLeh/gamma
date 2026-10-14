/*!
  \file   misc.h
  \author Aleksey Lekhtin
  \date   16.05.2025
  \brief  Мелкие функции
*/

#include <QString>

class UTILS
{
public:
    explicit UTILS() = default;
    virtual ~UTILS() = default;

    static QString str_center(const QString &str, int width);
    static QString str_left(const QString &str, int width);
    static QString str_right(const QString &str, int width);
    static QString secs_to_minute(int sec);
    static QString split_message(const QString &msg);
    static quint16 calc_crc_sum(const char *data, quint16 data_size);
    static float decode_ieee754(const char *data);
    static char *encode_ieee754(float value);
    static QString split_path(const QString &path);
};
