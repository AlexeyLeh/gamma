/*!
  \file   frame.h
  \author Алексей Лехтин
  \brief  Adam фрайм
  \date   10.05.2023
*/

#ifndef __FRAME_H__
#define __FRAME_H__

#include <string>
#include <vector>
#include <memory>
//// #include <bits/stl_list.h>
#include <list>

#include <QString>


#include "adam.h"

namespace Adam
{
    static constexpr int NodeE4RTime  = 300;
    static constexpr int MinFrameSize = 7;

    class Frame
    {
    public:
        Frame();
        Frame(byte to, byte from, byte cmd, int rtime = NodeE4RTime);
        Frame(byte to, byte from, byte cmd, byte ext, int rtime = NodeE4RTime);
        Frame(byte to, byte from, byte cmd, const std::vector<byte>& payload, int rtime = NodeE4RTime);

        Frame(const byte *frame, size_t size, int rtime = NodeE4RTime);
        Frame(const std::vector<byte> &frame, int rtime = NodeE4RTime);

        virtual ~Frame() = default;

    public:
        const std::vector<byte> bytes() const;      // frame as bytes array

        bool    valid() const;
        byte    to() const;
        byte    from() const;
        byte    cmd() const;
        byte    ext() const;
        ushort  length() const;
        ushort  checksum() const;
        ushort  rtime() const;
        void    set_rtime(ushort rtime);
        bool    response_expected() const;
        void    set_no_response_required();
        int     elapsed_time() const;
        void    set_elapsed_time(int msec);
        void    set_frame_name(const QString &name);
        QString name() const;

        const std::vector<byte> &payload() const;  // payload data from frame

        QString dump(bool field_names = false) const;
        static QString dump(const std::vector<byte> &bytes);

        static int calc_rtime(const std::list<Adam::Frame> &frames);

    private:
        ushort calc_checksum() const;
        void   set_checksum();

    protected:
        QString           m_frame_name;     // название команды
        std::vector<byte> m_frame;          // полный Adam-фрейм
        ushort            m_checksum;       // контрольная сумма (вычисляется из m_frame)
        byte              m_ext;            // есть в некторых командах
        std::vector<byte> m_payload;        // данные (копируются из m_frame)
        bool              m_valid;          // корректность данных фрэйма
        int               m_rtime;          // ms - the maximum response time in millisec
        bool              m_resp_expected;  // ожидает-ли запрос ответа
        int               m_elapsed_time;
    };

} // namespace Adam


#endif // __FRAME_H__
