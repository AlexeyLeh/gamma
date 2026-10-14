/*!
  \file   progress.h
  \author Aleksey Lekhtin
  \date   07.05.2015
  \brief  Simple progress bar
*/

#ifndef _PBAR_H_
#define _PBAR_H_

#include <QProgressDialog>
#include <QFont>

class PBAR : public QProgressDialog
{
   Q_OBJECT;

public:

   // explicit PBAR( QWidget *parent = 0 );

   explicit PBAR(const QString& title, const QString& msg_text, QWidget *parent = nullptr);
   ~PBAR();

   void progress( const QString & title,
                  const QString & msg_text,
                  int             start_value = 0,
                  int             max_value = 0,
                  bool            cancel_button = false );

   void progress( int position, int maxpos = 0 );
   void progress_100();
   void set_text( const QString & msg_text );
   void set_font( QFont fnt );
   void end( void );

signals:
   void signal_set_value(int,int);

private slots:
   void slot_set_value( int value, int max_value );
   //void slot_cancel();

private:

   QFont m_font;

};

#endif // _PBAR_H_
