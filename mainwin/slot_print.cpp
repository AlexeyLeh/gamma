/*!
  \file   slot_print.cpp
  \brief  Печать протокола
  \author Aleksey Lekhtin
  \date   created 13.04.2015
*/

#include <QApplication>
#include <QPrinterInfo>
#include <QPrintDialog>

#include "mainwindow.h"

static void print_param( QPrinter *printer )
{
#if 1
   qreal left, top, right, bottom;
   printer->getPageMargins( &left, &top, &right, &bottom,
                            QPrinter::Millimeter );
   QPageSize pg_sz = printer->pageLayout().pageSize();
   QSizeF sz = pg_sz.size( QPageSize::Millimeter );

   qDebug()
      << "got printerName:"            << printer->printerName()
      << "\ngot outputFileName:"       << printer->outputFileName()
      << "\ngot pageSize:"             << pg_sz << sz
      << printer->paperSize(QPrinter::Millimeter)
      << "\ngot orientation:"
      << printer->orientation()
      << (printer->orientation() == QPrinter::Portrait ?
          "Portrait" : "")
      << (printer->orientation() == QPrinter::Landscape ?
          "Landscape" : "")
      << "\ngot res:" << printer->resolution()
      << "\ngot margins:"              << left << top << right << bottom
      << "\n";
#endif
}


//-----------------------------------------------------------
void MW::printer_init()
{
#ifdef Q_OS_LINUX
   QPrinter::PrinterMode mode = QPrinter::HighResolution;
#else
   QPrinter::PrinterMode mode = QPrinter::ScreenResolution;
#endif

   if( !m_printer )
      m_printer = new QPrinter( mode );

   qDebug() << __FUNCTION__ << "(0):" << m_printer;

   //-----------------------------------------------------------
   m_printer->setColorMode( QPrinter::GrayScale );
   m_printer->setPrinterName( m_printer_name );
#ifdef Q_OS_LINUX
   m_printer->setOutputFileName( m_output_fname );
#endif

   if( m_printer->printerName().isEmpty()
#ifdef Q_OS_LINUX
       && m_printer->outputFileName().isEmpty()
#endif
      )
   {
      QPrinterInfo printer = QPrinterInfo::defaultPrinter();
      qDebug() << "default printer:" << printer.printerName();
      if( printer.isNull() || printer.printerName().isEmpty() )
      {
         QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
         foreach( QPrinterInfo prn, printer_list )
         {
            qDebug() << prn.printerName()
                     << prn.QPrinterInfo::supportedPageSizes();
         }
         printer = printer_list[0];
         qDebug() << "selected printer:" << printer.printerName();
      }
      m_printer_name = printer.printerName();
      m_printer->setPrinterName( m_printer_name );
   }

   if( m_output_fname.isEmpty() )
   {
      int num = 1;
      for( int i = 5; i > 0; --i )
      {
          QAction  *act = NULL;
          if(      i == 1 ) act = m_action__view_log1;
          else if( i == 2 ) act = m_action__view_log2;
          else if( i == 3 ) act = m_action__view_log3;
          else if( i == 4 ) act = m_action__view_log4;
          else if( i == 5 ) act = m_action__view_log5;
          if( act )
          {
              if( act->isChecked() ) num = i;
              break;
          }
      }

      m_output_fname = m_log.protocol_fullname( num );
      if( m_output_fname.endsWith( ".html" ) )
         m_output_fname.replace( ".html", ".pdf" );
   }

   m_printer->setDocName( m_output_fname );

   //-----------------------------------------------------------
   // QPageSize pg_sz( m_page_size, QPageSize::Millimeter, "", QPageSize::FuzzyOrientationMatch );
   // qDebug() << pg_sz << pg_sz.size( QPageSize::Millimeter );

   // QPageLayout pg_lay = m_printer->pageLayout();
   // pg_lay.setUnits( QPageLayout::Millimeter );
   // pg_lay.setPageSize( pg_sz );
   // bool ok = m_printer->setPageLayout( pg_lay );
   // qDebug() << "ok:" << ok;

   // pg_sz = m_printer->pageLayout().pageSize();
   // m_page_size = pg_sz.size( QPageSize::Millimeter );
   // qDebug() << pg_sz << m_page_size;

   m_printer->setOrientation( (QPrinter::Orientation)m_orientation );
   m_printer->setFullPage( true );
   m_printer->setFontEmbeddingEnabled( true );
   //m_printer->setResolution( 100 ); //m_print_res
   //qDebug() << "set margins:" << m_left << m_top << m_right << m_bottom;

   m_printer->setPageMargins( m_left, m_top, m_right, m_bottom,
                              QPrinter::Millimeter );

   //-----------------------------------------------------------
   print_param( m_printer );
}

//-----------------------------------------------------------
void MW::printer_save_option()
{
   m_printer_name = m_printer->printerName();
   m_output_fname = m_printer->outputFileName();
   m_orientation  = m_printer->orientation();
   m_print_res    = m_printer->resolution();
   m_printer->getPageMargins( &m_left, &m_top, &m_right, &m_bottom,
                              QPrinter::Millimeter );
   QPageSize pg_sz = m_printer->pageLayout().pageSize();
   m_page_size = pg_sz.size( QPageSize::Millimeter );
   qDebug() << __FUNCTION__ << pg_sz << m_page_size;

   //m_print_res = 600;
}

//-----------------------------------------------------------
void MW::slot_print_options()
{
   qDebug() << __FUNCTION__ << "(7):" << m_printer;
   qApp->setOverrideCursor( Qt::ArrowCursor );
   printer_init();
   QPageSetupDialog opt( m_printer, this );
   Qt::WindowFlags flags = opt.windowFlags();
   flags |= Qt::WindowTitleHint;
   flags |= Qt::Window;
   opt.setWindowFlags( flags );
   opt.setWindowTitle( trUtf8( "Page Setup" ) );
   qApp->restoreOverrideCursor();
   opt.exec();
   printer_save_option();
}

//-----------------------------------------------------------
void MW::slot_print_preview()
{
   qDebug() << __FUNCTION__ << "(4):" << m_printer;
   qApp->setOverrideCursor( Qt::ArrowCursor );
   printer_init();
   QPrintPreviewDialog dlg( m_printer, this );
   Qt::WindowFlags flags = dlg.windowFlags();
   flags |= Qt::WindowTitleHint;
   flags |= Qt::Window;
   dlg.setWindowFlags( flags );
   dlg.setWindowTitle( trUtf8( "Preview" ) );
   QWidget *wgt = dlg.parentWidget();
   if( wgt ) dlg.setGeometry( wgt->geometry() );

   connect( &dlg, SIGNAL( paintRequested( QPrinter* ) ),
            this, SLOT( slot_print_request( QPrinter* ) ) );
   qApp->restoreOverrideCursor();
   //if( dlg.exec() == QDialog::Accepted )
   dlg.exec();
   printer_save_option();
}

//-----------------------------------------------------------
void MW::slot_print()
{
   qDebug() << __FUNCTION__ << "(1):" << m_printer;
   qApp->setOverrideCursor( Qt::ArrowCursor );
   printer_init();
   QPrintDialog dlg( m_printer, this );
   Qt::WindowFlags flags = dlg.windowFlags();
   flags |= Qt::WindowTitleHint;
   dlg.setWindowFlags( flags );
   dlg.setWindowTitle( trUtf8( "Print" ) );
   dlg.setOption( QAbstractPrintDialog::PrintToFile );
   dlg.setOption( QAbstractPrintDialog::PrintPageRange );
   dlg.setOption( QAbstractPrintDialog::PrintShowPageSize );
   dlg.setOption( QAbstractPrintDialog::PrintCollateCopies );
   dlg.setOption( QAbstractPrintDialog::PrintCurrentPage );
   if( m_protocol_page->textCursor().hasSelection() )
      dlg.setOption( QAbstractPrintDialog::PrintSelection, true );

   qApp->restoreOverrideCursor();

   if( dlg.exec() == QDialog::Accepted )
      slot_print_request( m_printer );
   printer_save_option();
}

//-----------------------------------------------------------
void MW::slot_print_request( QPrinter *printer )
{
   qDebug() << __FUNCTION__ << "(6):" << printer;
   //m_page2->document()->print( printer );
   qApp->setOverrideCursor( Qt::ArrowCursor );
   print_param( printer );
   int num = 0;
   for( int i = 5; i > 0; --i )
   {
       QAction  *act = NULL;
       if(      i == 1 ) act = m_action__view_log1;
       else if( i == 2 ) act = m_action__view_log2;
       else if( i == 3 ) act = m_action__view_log3;
       else if( i == 4 ) act = m_action__view_log4;
       else if( i == 5 ) act = m_action__view_log5;
       if( !act ) continue;
       if( !act->isChecked() ) continue;
       num = i;
       break;
   }

   int orientation = -1;
   if( num == 4 )
   {
       orientation = printer->orientation();
       printer->setOrientation( (QPrinter::Orientation)1 ); // album
   }

   QString text = m_log.get_protocol( num );
   if( text.isEmpty() )
   {
      qApp->restoreOverrideCursor();
      return;
   }
   QTextDocument td;
   td.setHtml( text );
   td.print( printer );

   if( orientation != -1 )
       printer->setOrientation( (QPrinter::Orientation)orientation );

   qApp->restoreOverrideCursor();
}

#if 0
//-----------------------------------------------------------
QString MW::select_printer()
{
      QString printer_name;
   QStringList prn_list = QPrinterInfo::availablePrinterNames();
   QInputDialog *input_dlg = new QInputDialog( this );
   input_dlg->setWindowTitle( "Printer's List" );
   input_dlg->setLabelText( "Select Printer" );
   input_dlg->setComboBoxItems( prn_list );

   QWidget *main_wgt = m_stacked->widget(0);
   if( main_wgt )
   {
      QFont fnt = main_wgt->font();
      int sz = fnt.pointSize();
      if( sz < 14 ) sz = 0;
      if( sz == 0 ) sz = m_sys_content_font_size;
      input_dlg->setFont( fnt );
   }

   int rc = input_dlg->exec();
   if( rc == QDialog::Accepted )
   {
      printer_name = input_dlg->textValue();
   }
   delete input_dlg;
}
#endif
