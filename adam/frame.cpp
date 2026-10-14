/*!
  \file   frame.cpp
  \author Алексей Лехтин
  \brief  Adam фрайм
  \date   10.05.2023
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <QtDebug>

#include "frame.h"

namespace Adam
{
Frame::Frame() :
    m_checksum(0),
    m_ext(0),
    m_valid(false),
    m_elapsed_time(0)
{
}

Frame::Frame(byte to, byte from, byte cmd, int rtime) :
    m_checksum(0),
    m_ext(),
    m_valid(true),
    m_rtime(rtime),
    m_resp_expected(true),
    m_elapsed_time(0)
{
    short len = MinFrameSize;
    m_frame.resize(len, byte(0));
    m_frame[0] = to;
    m_frame[1] = from;
    m_frame[2] = (byte)(len >> 8 & 0xff);
    m_frame[3] = (byte)(len & 0xff);
    m_frame[4] = cmd;
    m_payload.clear();
    set_checksum();
    if (m_frame.size() != static_cast<size_t>(length()))
        m_valid = false;
}

Frame::Frame(byte to, byte from, byte cmd, byte ext, int rtime) :
    m_checksum(0),
    m_ext(),
    m_valid(true),
    m_rtime(rtime),
    m_resp_expected(true),
    m_elapsed_time(0)
{
    short len = MinFrameSize + 1;
    m_frame.resize(len, byte(0));
    m_frame[0] = to;
    m_frame[1] = from;
    m_frame[2] = (byte)(len >> 8 & 0xff);
    m_frame[3] = (byte)(len & 0xff);
    m_frame[4] = cmd;
    m_frame[5] = ext;
    m_ext = ext;
    m_payload.resize(1);
    m_payload[0] = ext;
    set_checksum();
    if (m_frame.size() != static_cast<size_t>(length()))
        m_valid = false;
}

Frame::Frame(byte to, byte from, byte cmd, const std::vector<byte> &payload, int rtime) :
    m_checksum(0),
    m_ext(),
    m_valid(true),
    m_rtime(rtime),
    m_resp_expected(true),
    m_elapsed_time(0)
{
    short len = MinFrameSize + payload.size();
    m_frame.resize(len, byte(0));
    m_frame[0] = to;
    m_frame[1] = from;
    m_frame[2] = (byte)(len >> 8 & 0xff);
    m_frame[3] = (byte)(len & 0xff);
    m_frame[4] = cmd;
    std::copy(payload.begin(), payload.end(), m_frame.begin()+5);
    m_payload = payload;
    set_checksum();
    if (m_frame.size() != static_cast<size_t>(length()))
        m_valid = false;
}

Frame::Frame(const std::vector<byte> &frame, int rtime) :
    m_checksum(0),
    m_ext(),
    m_valid(true),
    m_rtime(rtime),
    m_resp_expected(true),
    m_elapsed_time(0)
{
    if (frame.size() < Adam::MinFrameSize) {
        m_valid = false;
        return;
    }

    auto len = ((short)frame[2] << 8 | (short)frame[3]);
    if (frame.size() < (size_t)len) {
        m_valid = false;
        return;
    }

    m_frame = std::vector<byte>(frame.begin(), frame.begin()+len);
    // m_payload  = std::vector(m_frame.begin()+5, m_frame.end()-2);
    m_payload.resize(m_frame.size()-7);
    std::copy(m_frame.begin()+5, m_frame.end()-2, m_payload.begin());

    m_checksum = m_frame[len - 2] << 8 | m_frame[len - 1];

    auto estimated_checksum = calc_checksum();
    if (estimated_checksum != m_checksum) {
        m_valid = false;
    }
}

Frame::Frame(const byte *frame, size_t size, int rtime) :
    Frame(std::vector<byte>{frame, frame+size},rtime)
{
}

bool Frame::valid() const
{
    return m_valid;
}

ushort Frame::rtime() const
{
    return m_rtime;
}

void Frame::set_rtime(ushort rtime)
{
    m_rtime = rtime;
}

bool Frame::response_expected() const
{
    return m_resp_expected;
}

void Frame::set_no_response_required()
{
    m_resp_expected = false;
}

ushort Frame::checksum() const
{
    return m_checksum;
}

void Frame::set_checksum()
{
    ushort checksum = calc_checksum();
    m_checksum = checksum;
    m_frame[m_frame.size() - 2] = (byte)(checksum >> 8 & 0xff);
    m_frame[m_frame.size() - 1] = (byte)(checksum & 0xff);
}

ushort Frame::calc_checksum() const
{
    ushort checksum = 0;
    for (size_t i = 0; i < m_frame.size()-2; ++i)
    {
        checksum += ushort(m_frame[i]);
    }
    return checksum;
}

byte Frame::to() const
{
    return m_frame.empty() ? static_cast<byte>(0) : m_frame[0];
}

byte Frame::from() const {
    return m_frame.size() < 2 ? static_cast<byte>(0) : m_frame[1];
}

byte Frame::cmd() const {
    return m_frame.size() < 5 ? static_cast<byte>(0) : m_frame[4];
}

byte Frame::ext() const
{
    return m_ext;
}

ushort Frame::length() const
{
    return ((ushort)m_frame[2] << 8 | (ushort)m_frame[3]);
}

// payload data from frame
const std::vector<byte> &Frame::payload() const
{
    return m_payload;
}

// frame as bytes array
const std::vector<byte> Frame::bytes() const
{
    return m_frame;
}

int Frame::elapsed_time() const
{
    return m_elapsed_time;
}

void Frame::set_elapsed_time(int msec)
{
    m_elapsed_time = msec;
}

QString Frame::dump(const std::vector<byte> &bytes)
{
    char bf[1024*4] = {0};
    for (auto b : bytes)
    {
        auto ln = strlen(bf);
        auto remain = sizeof(bf)-ln;
        if (remain == 0) break;
        std::snprintf(bf+ln, remain, "%02X ", b);
    }
    auto bf_len = std::strlen(bf);
    if (bf_len > 0) {
        bf[bf_len - 1] = '\0';
    }
    return QString::fromLatin1(bf);
}

QString Frame::dump(bool field_names) const
{
    auto bytes = m_frame;
    ushort len = (ushort)bytes[2] << 8 | (ushort)bytes[3];
    ushort checksum = bytes[bytes.size() - 2] << 8 | bytes[bytes.size() - 1];

    char bf[1024] = {0};
    for (int i = 0; i < (len-MinFrameSize); ++i)
    {
        auto ln = strlen(bf);
        auto remain = sizeof(bf)-ln-1;
        std::snprintf(bf+ln, remain, "%02X ", bytes[5+i]);
    }
    bf[strlen(bf)-1] = '\0';

    int sz;
    std::unique_ptr<char[]> buf;
    if (field_names)
    {
        const char *format = "to:%02X, from:%02X, len:%02X %02X(%3d), cmd:%02X, data:[%s], csum:%02X %02X(%3d)";
        sz = std::snprintf(nullptr, 0, format, bytes[0], bytes[1],
                bytes[2], bytes[3], len, bytes[4], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1], checksum)+1;
        buf = std::make_unique<char[]>(sz);
        std::snprintf(buf.get(), sz, format, bytes[0], bytes[1],
                bytes[2], bytes[3], len, bytes[4], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1], checksum);
    }
    else
    {
        const char *fmt1 = "%02X %02X %02X %02X %02X %s%02X %02X";
        const char *fmt2 = "%02X %02X %02X %02X %02X [%s] %02X %02X";
        const char *format = fmt1;
        if (strlen(bf) > 0) format = fmt2;

        sz = std::snprintf(nullptr, 0, format, bytes[0], bytes[1],
                bytes[2], bytes[3], bytes[4], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1]) + 1;
        buf = std::make_unique<char[]>(sz);
        std::snprintf(buf.get(), sz, format, bytes[0], bytes[1],
                bytes[2], bytes[3], bytes[4], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1]);
    }
    return QString(buf.get());
}

QString Frame::name() const
{
    return m_frame_name;
}

void Frame::set_frame_name(const QString &name)
{
    m_frame_name = name;
}

int Frame::calc_rtime(const std::list<Adam::Frame> &frames)
{
    int rtime = 100;
    for (const auto &frame : frames)
        rtime += frame.rtime();
    return rtime;
}

} // namespace Adam
