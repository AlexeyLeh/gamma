/*!
  \file   response.cpp
  \author Алексей Лехтин
  \brief  Adam, ответ
  \date   10.05.2023
*/

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <iterator>
#include <vector>

#include <QtDebug>

#include "response.h"

namespace Adam
{

    RawResponse::RawResponse(const std::vector<byte> &response_bytes, const Error &error) :
        m_bytes(response_bytes),
        m_error(error)
    {
    }

    RawResponse::RawResponse(const Error &error) :
        m_bytes(),
        m_error(error)
    {
    }

    const std::vector<Response> RawResponse::responses()
    {
        if (m_error.has_value())
        {
            auto err = Adam::Error{m_error.value()->e, m_error.value()->s};
            return std::vector<Response>{Response(err)};
        }

        std::vector<Response> out;
        const std::vector<byte> &bytes = m_bytes;
        auto it = bytes.cbegin();
        for(; it != bytes.cend(); ++it)
        {
            auto remaining_dist = std::distance(it, bytes.cend());
            if (remaining_dist < MinFrameSize) {
                break;
            }

            auto data  = it.base();
            auto len  = (static_cast<short>(data[2]) << 8) | static_cast<short>(data[3]);
            if (len < MinFrameSize || len > remaining_dist) {
                break; // Битый пакет, прерываем разбор во избежание Segmentation fault
            }
            auto response = Adam::Response(std::vector<byte>{data, data+len});
            out.emplace_back(response);
            it += len;
        }

        return out;
    }

    Adam::Error RawResponse::error() const
    {
        return m_error;
    }

    const std::vector<byte> &RawResponse::bytes() const
    {
        return m_bytes;
    }

    const std::string RawResponse::dump() const
    {
        return Adam::dump_bytes(m_bytes);
    }

    Response::Response(const Error &error) :
        Frame(),
        m_ack(1),
        m_status(0),
        m_error(error)
    {
    }

    Response::Response(const std::vector<byte> &bytes) :
        Frame(bytes),
        m_ack(1),
        m_status(0xFD),
        m_data(),
        m_error()
    {
        if (bytes.size() < Adam::MinFrameSize)
        {
            // m_error = std::make_optional<Error>({1, "Response is too small"});
            m_error = Error(1, "Response is too small");
            m_valid = false;
            return;
        }

        auto len = ((short)bytes[2] << 8 | (short)bytes[3]);
        if (bytes.size() < (size_t)len)
        {
            // m_error = std::make_optional<Error>({2, "Response size is less than declared length"});
            m_error = Error(2, "Response size is a less than declared length");
            m_valid = false;
            return;
        }

        if (m_payload.size() < 3)
        {
            // m_error = std::make_optional<Error>({3, "Wrong response format/size (no Ack or Status)"});
            m_error = Error(3, "Wrong response format/size (no Ack or Status)");
            m_valid = false;
            return;
        }

        m_ack = m_payload.at(0);
        m_status = m_payload.at(1) << 8 | m_payload.at(2);
        m_data.resize(m_payload.size()-3);
        std::copy(m_payload.begin()+3, m_payload.end(), m_data.begin());
    }

    Adam::Error Response::error() const
    {
        return m_error;
    }

    byte Response::cmd() const
    {
        return Frame::cmd();
    }

    byte Response::from() const
    {
        return Frame::from();   // inverted against request Frame!
    }

    byte Response::to() const
    {
        return Frame::to();     // inverted against request Frame!
    }

    byte Response::ack() const
    {
        return m_ack;
    }

    ushort Response::status() const
    {
        return m_status;
    }

    byte Response::ext() const
    {
        if (m_data.size() > 0)
            return m_data[0];
        return 0;
    }

    size_t Response::payload_size() const
    {
        return m_data.size();
    }

    const std::vector<byte> &Response::payload() const
    {
        return m_data;
    }

    byte Response::payload_byte() const
    {
        if (m_data.empty()) return 0;
        return m_data.at(0);
    }

    const std::string Response::payload_string() const
    {
        return std::string(reinterpret_cast<const char*>(m_data.data()), m_data.size());

    }

    std::string Response::dump() const
    {
        const std::vector<byte> &bytes = m_frame;
        if (bytes.size() < 10) return {};
        int len = (static_cast<ushort>(bytes[2]) << 8) | static_cast<ushort>(bytes[3]);
        // Защищаем длину лога от порчи памяти
        if (len > static_cast<int>(bytes.size())) len = static_cast<int>(bytes.size());

        char bf[1024] = {0};
        for (int i = 0; i < (len-10); ++i)
        {
            auto ln = strlen(bf);
            auto remain = sizeof(bf)-ln-1;
            if (remain == 0) break;

            if (8 + i < static_cast<int>(bytes.size())) {
                std::snprintf(bf + ln, remain, "%02X ", bytes[8 + i]);
            }
        }
        auto bf_len = std::strlen(bf);
        if (bf_len > 0) {
            bf[bf_len - 1] = '\0';
        }

        int sz;
        std::string format;
        std::unique_ptr<char[]> buf;
        format = std::string("%02X %02X %02X %02X %02X %02X %02X %02X [%s] %02X %02X");
        sz = std::snprintf(nullptr, 0, format.c_str(), bytes[0], bytes[1],
                bytes[2], bytes[3], bytes[4], bytes[5],  bytes[6], bytes[7], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1]) + 1;
        buf = std::make_unique<char[]>(sz);
        sz = std::snprintf(buf.get(), sz, format.c_str(), bytes[0], bytes[1],
                bytes[2], bytes[3], bytes[4], bytes[5],  bytes[6], bytes[7], bf,
                bytes[bytes.size()-2], bytes[bytes.size()-1]) + 1;
        return std::string(buf.get(), buf.get() + sz-1);
    }

    std::string dump_bytes(const std::vector<byte> &bytes)
    {
        char bf[1024*4] = {0};
        for (auto b : bytes)
        {
            auto ln = strlen(bf);
            auto remain = sizeof(bf)-ln;
            if (remain == 0) break;
            std::snprintf(bf+ln, remain, "%02X ", b);
        }
        bf[strlen(bf)-1] = '\0';
        return std::string(bf, strlen(bf));
    }

} // namespace Adam

