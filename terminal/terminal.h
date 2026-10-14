/*!
  \file   terminal.h
  \brief  Терминал
  \author Aleksey Lekhtin
  \date   created 17.11.2014
*/

#ifndef __TERMINAL_H_
#define __TERMINAL_H_

#include <QApplication>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QFile>
#include <QStringList>
#include <QByteArray>
#include <QKeyEvent>

class TERMINAL : public QTextEdit
{
   Q_OBJECT;

public:
   enum PRINT_MODE
   {
      HEX_MODE  = (int)1,
      TEXT_MODE =      2,
      HTML_MODE =      3
   };
   Q_ENUM(PRINT_MODE);

public:
   explicit TERMINAL(QWidget *parent = nullptr);
   ~TERMINAL() override;

   void     set_number( int num );
   int      number() const;

   void     set_name( const QString  &name );
   QString  name() const;

   QLabel   *label();
   void     set_label( const QString  &text );

   QCheckBox *enable_checkbox();

   void     set_activated( bool status, bool enable_log = false );
   void     put_data( const QByteArray &data );
   void     put_data( const QByteArray &data, PRINT_MODE mode);
   void     put_data_ln( const QByteArray &data = "");
   void     put_data_hex_ln( const QByteArray &data);
   void     close_file_log();
   bool     open_file_log();

   void     save_bin( const QByteArray  &data ) const;
   void     save_bin( const char *data, int size ) const;
   void     save_bin( const char *data ) const;

signals:
   void     cursorPositionChanged();

public slots:
   void     slot_clear();

protected:
   virtual void keyPressEvent( QKeyEvent *e ) override;

private:
   int         m_number;
   QString     m_name;
   QLabel     *m_label;
   QCheckBox  *m_enabled;
   QFile      *m_flog;
   QFile      *m_fbin;
   QStringList m_cmd_list;
};

#endif // __TERMINAL_H_
