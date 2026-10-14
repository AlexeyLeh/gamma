/*!
 * \file   translator.h
 * \author Aleksey Lekhtin
 * \date   23.05.2025
 * \brief  Load transaltions
 */

#ifndef TRANSLATOR__H__
#define TRANSLATOR__H__

#include <QString>
#include <QTranslator>

class TRANSLATOR
{
public:
    explicit TRANSLATOR();
    virtual  ~TRANSLATOR();

    const QString &tr_path() const;
    const QString &tr_name() const;
    bool  install(QTranslator *qt_translator, QTranslator *app_translator) const;

private:
    void          load();

private:
    QString       m_tr_name;
    QString       m_tr_path;
};

#endif // TRANSLATOR__H__
