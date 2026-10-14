/*!
  \file   create_sn.cpp
  \brief  Создание графиков
  \author Aleksey Lekhtin
  \date   08.11.2016
*/

#include "mainwindow.h"

//---------------------------------------------------------
//! Создание графика
//
void MW::create_SN()
{
    m_fld_SN = new QComboBox( this );
    m_fld_SN->setCurrentText( "0000000000" );
    m_fld_SN->setMinimumContentsLength( m_SN_max_len );
    m_fld_SN->setToolTip( trUtf8("The Serial Number of testing device" ) );
    m_fld_SN->setEditable( true );
    m_fld_SN->setDuplicatesEnabled( false );
    connect( m_fld_SN, SIGNAL( editTextChanged(const QString &) ),
             this, SLOT( slot_SN_changed(const QString &) ) );

    detect_all_SN( m_fld_SN );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( new QLabel( trUtf8("Serial No") ) );
    layout->addWidget( m_fld_SN );
    layout->addStretch();

    m_box_SN = new QWidget();
    m_box_SN->setLayout( layout );
}

//-----------------------------------------------------------
//! Уведомление о вводе серийного номера
// \return признак подтверждения
//
bool MW::ask_for_SN()
{
    bool ok = false;
    m_input_dlg = new QInputDialog( this );
    m_input_dlg->setWindowTitle( trUtf8("The Serial Number of testing device" ) );
    m_input_dlg->setLabelText( trUtf8("Serial No") );
    m_input_dlg->setTextValue( m_fld_SN->currentText() );

    // QLineEdit *lineEdit = m_input_dlg->findChildren<QLineEdit *>()[0];
    // lineEdit->setText( m_fld_SN->text() );
    // lineEdit->deselect();

    connect( m_input_dlg, SIGNAL( textValueChanged(const QString &) ),
             this, SLOT( slot_input_dlg(const QString &) ) );

    connect( m_input_dlg, SIGNAL( textValueSelected(const QString &) ),
             this, SLOT( slot_input_dlg_selected(const QString &) ) );

    QWidget *main_wgt = m_stacked->widget(0);
    if( main_wgt )
    {
        QFont fnt = main_wgt->font();
        int sz = fnt.pointSize();
        if( sz < 14 ) sz = 0;
        if( sz == 0 ) sz = m_sys_content_font_size;
        m_input_dlg->setFont( fnt );
    }

    int rc = m_input_dlg->exec();
    if( rc == QDialog::Accepted )
    {
        //m_fld_SN->addItem( m_input_dlg->textValue().trimmed() );
        m_fld_SN->setCurrentText( m_input_dlg->textValue().trimmed() );
        ok = true;
    }
    delete m_input_dlg;
    return ok;
}
//-----------------------------------------------------------
void MW::slot_input_dlg( const QString & text )
{
    text.trimmed();
    if( text.size() <= m_SN_max_len ) return;
    QString txt = text;
    //txt.resize( m_SN_max_len );
    m_fld_SN->setCurrentText( txt );
}

//-----------------------------------------------------------
void MW::slot_input_dlg_selected( const QString & text )
{
    QString str = text.trimmed();
    if( str.size() > m_SN_max_len )
    {
        //str.resize( m_SN_max_len );
        m_input_dlg->setTextValue( str );
        return;
    }

//    str = QString( "%1%2" )
//          .arg( QString( m_SN_max_len - str.size(), QChar('0') ) )
//          .arg( str );
//    m_input_dlg->setTextValue( str );
}

//-----------------------------------------------------------
void MW::slot_SN_changed(const QString & text)
{
    text.trimmed();
    if( text.size() <= m_SN_max_len ) return;
    QString txt = text;
    //txt.resize( m_SN_max_len );
    m_fld_SN->setCurrentText( txt );
}

//-----------------------------------------------------------
void MW::detect_all_SN( QComboBox *fld_SN )
{
    QString fname = m_log.base_filename();
    QStringList filter;
    filter << QString( "%1.log.*.html" ).arg( fname );
    QDir dir( m_log.real_path() );
    //dir.setFilter(  QDir::Dirs | QDir::NoSymLinks );
    dir.setFilter(  QDir::Files | QDir::NoSymLinks );
    dir.setSorting( QDir::Name  | QDir::IgnoreCase );
    QStringList list = dir.entryList( filter );

    for( int i = 0; i < list.size(); ++i )
    {
        QString & str = list[i];
        for( int n = 0; n < 20; ++n )
        {
            QString bf = QString( "-%1.html" ).arg(n);
            str = str.replace( bf, "" );
        }
        str = str.replace( fname+".log.", "" );
    }

    list.removeDuplicates();

    QString current_text = fld_SN->currentText();
    fld_SN->clear();
    fld_SN->addItems( list );
    int pos = fld_SN->findText( current_text );
    if( pos > -1 )
    {
        fld_SN->setCurrentIndex( pos );
    }
    else if( !current_text.isEmpty() )
    {
        fld_SN->setInsertPolicy( QComboBox::InsertAlphabetically );
        //fld_SN->insertItem( 0, current_text );
        ////fld_SN->addItem( current_text );
    }
}
