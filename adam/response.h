/*!
  \file   response.h
  \author Алексей Лехтин
  \brief  Adam, ответ
  \date   10.05.2023
*/

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <optional>

#include "frame.h"

namespace Adam
{

class connection_error
{
public:
    connection_error() = default;
    connection_error(int err, std::string str) : e(err), s(str) { }

    bool has_value() { return (e > 0); }
    const connection_error *value() { return this; }

    int e{0};
    std::string s;
};

using Error = connection_error;

std::string dump_bytes(const std::vector<byte> &bytes);

class Response;

class RawResponse
{
public:
    RawResponse(const std::vector<byte> &response_bytes, const Error &error = {});
    explicit RawResponse(const Error &error = {});

    const std::vector<Response> responses();
    Adam::Error error() const;
    const std::vector<byte> &bytes() const;
    const std::string dump() const;

private:
    std::vector<byte>    m_bytes;
    Error m_error;
};

class Response : public Frame
{
public:
    Response() = default;
    Response(const std::vector<byte> &bytes);
    Response(const Error &error);

    Error                   error() const;

    byte                    cmd() const;
    byte                    from() const;
    byte                    to() const;
    byte                    ack() const;
    ushort                  status() const;
    byte                    ext() const;

    size_t                  payload_size() const;
    const std::vector<byte> &payload() const;
    byte                    payload_byte() const;
    const std::string       payload_string() const;

    std::string             dump() const;

private:
    byte                    m_ack;
    short                   m_status;
    std::vector<byte>       m_data;
    Error                   m_error;
};

} // namespace Adam


#endif // __RESPONSE_H__
