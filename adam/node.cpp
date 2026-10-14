/*!
  \file   node.cpp
  \author Aleksey Lekhtin
  \brief  Ууел оборудования
  \date   10.05.2023
*/

#include <sstream>
#include <QDebug>

#include "dbg.h"
#include "node.h"

NODE::NODE(NODECODE code, NODENAME name) :
    m_code(code),
    m_name(name)
{
}

NODE::~NODE()
{
    DBG(4);
}

std::string NODE::code_name() const
{
    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%02X", static_cast<unsigned int>(m_code));
    return std::string(buf);
}

NODECODE NODE::code() const
{
    return m_code;
}

void NODE::set_code(NODECODE code)
{
    m_code = code;
}

std::string NODE::name() const
{
    return m_name;
}

void NODE::set_name(std::string name)
{
    m_name = name;
}



