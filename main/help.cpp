/*!
\file   help.cpp
\brief  Справка
\author Aleksey Lekhtin
\date   created 08.03.2015
*/

#include "mainwindow.h"

/*
//-----------------------------------------------------------
//! Описание методики тестирования
//
void MW::slot_help_methodology()
{
    int ww_min = 900;
    int hh_min = 600;
    QDialog dlg( this, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint );
    dlg.setWindowModality( Qt::WindowModal );
    dlg.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    dlg.setMinimumSize( ww_min, hh_min );
    int ww = qMax( this->geometry().size().width(), ww_min );
    int hh = qMax( this->geometry().size().height(), hh_min );
    dlg.resize( ww, hh );
    dlg.setWindowTitle( tr("The methodology of testing") + "   -   " + tr("Connection rules") );

    //-----------------------------------------------------------
    QString title = "";
    QTextBrowser *doc = new QTextBrowser();
    doc->setHtml( help__get_gamma_hp_methodology() );

    //-----------------------------------------------------------
    QPushButton *btn_close = new QPushButton( tr("Close") );
    btn_close->setShortcut( Qt::Key_F10 );
    btn_close->setDefault( true );
    connect( btn_close, SIGNAL( clicked() ),
        &dlg, SLOT( close() ) );

    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addStretch();
    btn_layout->addWidget( btn_close );
    btn_layout->addStretch();

//    QTabWidget *tab = new QTabWidget();
//    tab->addTab( doc, m_app_title );

    //-----------------------------------------------------------
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( doc ); // tab
    layout->addLayout( btn_layout );

    dlg.setLayout( layout );

    //-----------------------------------------------------------
    QFont fnt = dlg.font();
    int sz = m_content_font_size;
    if( sz > 0 ) fnt.setPointSize( sz );
    //fnt.setStyleHint( QFont::Monospace );
    //fnt.setFamily("Monospace");
    dlg.setFont( fnt );
    doc->setFont( fnt );
    // doc1->setFont( fnt );
    // doc2->setFont( fnt );
    // doc3->setFont( fnt );
    // doc3->setFocus();
    // doc2->setFocus();
    // doc1->setFocus();
    doc->setFocus();
    //btn_close->setFocus();

    dlg.exec();
}

//-----------------------------------------------------------
//! Описание работы с приложением
//
void MW::slot_help_userguide()
{
    int ww_min = 900;
    int hh_min = 600;
    QDialog dlg( this, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint );
    dlg.setWindowModality( Qt::WindowModal );
    dlg.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    dlg.setMinimumSize( ww_min, hh_min );
    int ww = qMax( this->geometry().size().width(), ww_min );
    int hh = qMax( this->geometry().size().height(), hh_min );
    dlg.resize( ww, hh );
    dlg.setWindowTitle( tr("User Guide") );

    //-----------------------------------------------------------
    QString title = "";
    QTextBrowser *doc = new QTextBrowser();
    doc->setHtml( help__get_gamma_hp_userguide() );

    //-----------------------------------------------------------
    QPushButton *btn_close = new QPushButton( tr("Close") );
    btn_close->setShortcut( Qt::Key_F10 );
    btn_close->setDefault( true );
    connect( btn_close, SIGNAL( clicked() ),
        &dlg, SLOT( close() ) );

    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addStretch();
    btn_layout->addWidget( btn_close );
    btn_layout->addStretch();

    //QTabWidget *tab = new QTabWidget();
    //tab->addTab( doc, m_app_title );

    //-----------------------------------------------------------
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( doc ); // tab
    layout->addLayout( btn_layout );

    dlg.setLayout( layout );

    //-----------------------------------------------------------
    QFont fnt = dlg.font();
    int sz = m_content_font_size;
    if( sz > 0 ) fnt.setPointSize( sz );
    //fnt.setStyleHint( QFont::Monospace );
    //fnt.setFamily("Monospace");
    dlg.setFont( fnt );
    doc->setFont( fnt );
    doc->setFocus();
    dlg.exec();
}

//-----------------------------------------------------------
//! Сообщение о справке по подключению и работе
//
void MW::slot_help_show_attention()
{
   QString title = tr("For Your Info");
//    Перед началом работы необходимо убедиться в правильности подключения
//    тестируемого устройства, источника питания.
//    Подробности можно посмотреть в Справке: (клавиша F1)
//    - Руководство пользователя (F1)
//    - Методика тестирования    (Shift+F1)

   QString text = tr( "Before you start, make sure the correct connection "
                          "the device under test and the power supply.\n\n"
                          "Details can be found in the Help\n\n"
                          "- User Guide (press F1)\n"
                          "- The Methodology of testing (press Shift + F1)"
                        );

   QString html = QString("<html><body>"
                          "<table border='0' cellspacing='0' style='margin:auto;' >"
                          "<tr><td>%1</td></tr>"
                          "</table><br></body></html>" ).arg( text );
   html.replace( "\n", "<br>\n" );
   QMessageBox::information( this, title, html, QMessageBox::Close );
}
*/
