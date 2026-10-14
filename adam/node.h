/*!
  \file   node.h
  \author Aleksey Lekhtin
  \brief  Ууел оборудования
  \date   10.05.2023
*/

#ifndef __NODE_H__
#define __NODE_H__

#include <cstdint>
#include <string>

typedef std::string NODENAME;
typedef uint8_t     NODECODE;
typedef uint16_t    NODESTATUS;

class NODE
{
public:
    NODE(NODECODE code, NODENAME name);
    virtual ~NODE();

public:
    NODECODE code() const;
    std::string code_name() const;
    void set_code(NODECODE code);

    std::string name() const;
    void set_name(std::string name);

protected:
    NODECODE        m_code;
    NODENAME        m_name;
};

#endif // __NODE_H__
