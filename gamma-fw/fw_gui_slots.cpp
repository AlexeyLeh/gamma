/*!
  \file   fw_gui_create_page.cpp
  \author Aleksey Lekhtin
  \brief  Формирование раскладки настроек процессора FIRMWARE
  \date   16.06.2017
*/

#include <QMessageBox>
#include <QComboBox>
#include <QMessageBox>

#include "fw.h"
#include "fw_gui.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4_ctable.h"
#include "wmsg.h"

//---------------------------------------------------------
void FIRMWARE_GUI::slot_select_current_gravity(int index)
{
    (void)index;
    auto txt = m_fld_fw_grav_list->itemText(index);
    auto list = txt.split(QChar(' '));
    if (list.count() > 0)
        m_fld_fw_grav_field->setValue(list.at(0).toFloat());
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_edit_grav_field_finished()
{
    m_fld_fw_grav_field->clearFocus();
    auto text = m_fld_fw_grav_field->text();
    int index = m_fld_fw_grav_list->findText(text, Qt::MatchContains);
    m_fld_fw_grav_list->setCurrentIndex(index);
    m_fld_fw_grav_field->setValue(text.toUInt());
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_write_gravity()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    QString msg = QString("%1 [%2]").arg(btn->text(), m_fld_fw_grav_list->currentText());
    showstatus(msg, 5);
    float value = m_fld_fw_grav_field->value();
    if (!wish_write_data()) {
        return;
    }
    comm()->push_requests({ NodeE4::WriteLocalGravity(value) });
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_write_warmup_time()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    showstatus(btn->text(), 5);
    if (!wish_write_data()) {
        return;
    }
    ushort num = m_fld_fw_warmuptime->value();
    comm()->push_requests({ NodeE4::WriteHVWarmUp(static_cast<byte>(num)) });
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_write_acqtime()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    showstatus(btn->text(), 5);
    if (!wish_write_data()) {
        return;
    }
    ushort num = m_fld_fw_acqtime->value();
    comm()->push_requests({ NodeE4::WriteGammaRayAcqusitionTime(num) });
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_accel_poweron()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    showstatus(btn->text(), 5);
    if (!action_confirmation()) {
        return;
    }
    comm()->push_request(NodeE4::WriteAccelPowerOn());
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_accel_poweroff()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) {
        return;
    }
    showstatus(btn->text(), 5);
    if (!action_confirmation()) {
        return;
    }
    comm()->push_request(NodeE4::WriteAccelPowerOff());
}

//---------------------------------------------------------
void FIRMWARE_GUI::refresh_ctable()
{
    const CTable *ctable = m_fw->ctable();

    int dec = 6;
    QString str;
    str = QByteArray((char *)&ctable->m_check_sum, 2).toHex();
    m_ctable_accel_CRC->setText(QStringLiteral("0x") + str);

    str = QByteArray(ctable->m_part_number, sizeof(ctable->m_part_number));
    m_ctable_accel_pn->setText(str);

    str = QString::number(ctable->m_scale_Ksfz0, 'f', dec);
    m_ctable_accel_scale0->setText(str);

    str = QString::number(ctable->m_scale_Ksfz1, 'f', dec);
    m_ctable_accel_scale1->setText(str);

    str = QString::number(ctable->m_scale_Ksfz2, 'f', dec);
    m_ctable_accel_scale2->setText(str);

    str = QString::number(ctable->m_scale_Ksfz3, 'f', dec);
    m_ctable_accel_scale3->setText(str);

    str = QString::number(ctable->m_bias_Kbz0, 'f', dec);
    m_ctable_accel_bias0->setText(str);

    str = QString::number(ctable->m_bias_Kbz1, 'f', dec);
    m_ctable_accel_bias1->setText(str);

    str = QString::number(ctable->m_bias_Kbz2, 'f', dec);
    m_ctable_accel_bias2->setText(str);

    str = QString::number(ctable->m_bias_Kbz3, 'f', dec);
    m_ctable_accel_bias3->setText(str);

    str = QByteArray(ctable->m_accel_SN, sizeof(ctable->m_accel_SN));
    m_ctable_accel_SN->setText(str);

    str = QByteArray(ctable->m_date, sizeof(ctable->m_date));
    m_ctable_accel_Date->setText(str);

    str = QByteArray(ctable->m_time, sizeof(ctable->m_time));
    m_ctable_accel_Time->setText(str);
}

//---------------------------------------------------------
void FIRMWARE_GUI::refresh_ktable()
{
    const KTable *ktable = m_fw->ktable();

    QString str;
    str = QStringLiteral("0x%1").arg(ktable->get_checksum(), 4, 16, QChar(u'0'));
    m_fld_ktable_CRC->setText(str.toUpper());

    str = QByteArray( ktable->get_partnumber(), ktable->partnumber_size() );
    m_fld_ktable_PN->setText( str );

    str = QString::number( ktable->get_Short_bucket_size() );
    m_fld_Short_bucket_size->setText( str );

    str = QString::number( ktable->get_Medium_bucket_size() );
    m_fld_Medium_bucket_size->setText( str );

    str = QString::number( ktable->get_Long_bucket_size() );
    m_fld_Long_bucket_size->setText( str );

    str = QString::number( ktable->get_Calibration_time() );
    m_fld_Calibration_time->setText( str );

    str = QString::number( ktable->get_Acquire_frequency() );
    m_fld_Acquire_frequency->setText( str );
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_cktable_edit_finished()
{
    QString     str;
    bool        ktable_changed = false;
    bool        ctable_changed = false;
    char        *ss = nullptr;
    int         ss_len = 0;

    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    qDebug() << __FUNCTION__ << "():" << obj;
    if (!obj) return;

    CTable *ctable = m_fw->ctable();
    KTable *ktable = m_fw->ktable();

    // --- Проверка полей CTable (float) ---
    if (obj == m_ctable_accel_scale0) {
        auto value = static_cast<float>(obj->text().toDouble());
        if (qFuzzyIsNull(value)) value = 0.55; // 0 не допустим - ставим умолчание 0.55
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Ksfz0(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_scale1) {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Ksfz1(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_scale2) {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Ksfz2(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_scale3) {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Ksfz3(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_bias0)  {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Kbz0(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_bias1)  {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Kbz1(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_bias2)  {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Kbz2(value);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_bias3)  {
        auto value = static_cast<float>(obj->text().toDouble());
        obj->setText(QString::number(value, 'f', 6));
        ctable->set_Kbz3(value);
        ctable_changed = true;
    }

    // --- Проверка текстовых полей CTable ---
    else if (obj == m_ctable_accel_pn)   {
        ss = (char *)ctable->get_partnumber();
        ss_len = sizeof(ctable->m_part_number);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_SN)   {
        ss = (char *)ctable->get_AccSn();
        ss_len = sizeof(ctable->m_accel_SN);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_Date) {
        ss = (char *)ctable->get_date();
        ss_len = sizeof(ctable->m_date);
        ctable_changed = true;
    }
    else if (obj == m_ctable_accel_Time) {
        ss = (char *)ctable->get_time();
        ss_len = sizeof(ctable->m_time);
        ctable_changed = true;
    }

    // --- Проверка текстовых полей KTable ---
    else if (obj == m_fld_ktable_PN) {
        ss = (char *)ktable->get_partnumber();
        ss_len = sizeof(ktable->partnumber_size());
        ktable_changed = true;
    }

    // --- Проверка числовых полей KTable ---
    else if (obj == m_fld_Medium_bucket_size) {
        auto value = obj->text().toUInt();
        obj->setText(QString::number(value, 10));
        ktable->set_Medium_bucket_size(value);
        ktable_changed = true;
    }
    else if (obj == m_fld_Long_bucket_size)   {
        auto value = obj->text().toUInt();
        obj->setText(QString::number(value, 10));
        ktable->set_Long_bucket_size(value);
        ktable_changed = true;
    }
    else if (obj == m_fld_Acquire_frequency)  {
        auto value = obj->text().toUInt();
        obj->setText(QString::number(value, 10));
        ktable->set_Acquire_frequency(value);
        ktable_changed = true;
    }
    else if (obj == m_fld_Calibration_time)   {
        auto value = obj->text().toUInt();
        obj->setText(QString::number(value, 10));
        ktable->set_Calibration_time(value);
        ktable_changed = true;
    }
    else if (obj == m_fld_Short_bucket_size)  {
        auto value = obj->text().toUInt();
        obj->setText(QString::number(value, 10));
        ktable->set_Short_bucket_size(value);
        ktable_changed = true;
    }

    //-----------------------------------------------------
    if (ss && ss_len > 0) {
        QByteArray ba = obj->text().trimmed().toLatin1();
        memset(ss, 0, ss_len);
        strncpy(ss, ba.constData(), ss_len - 1);
        obj->setText(QString::fromLatin1(ss));
    }

    //-----------------------------------------------------
    // Если изменилось поле принадлежащее CTable
    if (ctable_changed) {
        quint16 crc = ctable->update_crc();
        str = QString("%1").arg(crc, 4, 16, QChar('0')).toUpper();
        m_ctable_accel_CRC->setText(QStringLiteral("0x") + str);
    }

    // Если изменилось поле принадлежащее KTable
    if (ktable_changed) {
        quint16 crc = ktable->update_crc();
        str = QStringLiteral("0x%1").arg(crc, 4, 16, QChar('0')).toUpper();
        m_fld_ktable_CRC->setText(str);
    }
}

//---------------------------------------------------------
void FIRMWARE_GUI::clear_fw_fields()
{
    // 1. Очистка текстовых полей QLineEdit (Survey Data и общие)
    m_fld_fw_CPS->clear();
    m_fld_fw_Temp->clear();
    m_fld_fw_accel_raw->clear();
    m_fld_fw_accel_incl->clear();
    m_fld_fw_accel_corr->clear();
    m_fld_fw_partnumber->clear();

    // 2. Безопасный сброс числовых спинбоксов в дефолтные/минимальные значения
    m_fld_fw_grav_field->setValue(0.0);
    m_fld_fw_warmuptime->setValue(0);
    m_fld_fw_acqtime->setValue(0);

    // Поля HVPS имеют диапазон 800-1800, сбрасываем на их логический минимум
    m_fld_fw_hv_min->clear();
    m_fld_fw_hv_max->clear();
    m_fld_fw_hv_tst->clear();
    m_fld_fw_hv_opr->clear(); // Операционное поле имеет диапазон от 0

    // 3. Очистка массива АЦП
    for (auto i: {0,1,2,3,4,5,6,7}) {
        if (m_fld_fw_ADC[i]) {
            m_fld_fw_ADC[i]->clear();
        }
    }

    // 4. Очистка полей CTable
    m_ctable_accel_CRC->clear();
    m_ctable_accel_pn->clear();
    m_ctable_accel_scale0->clear();
    m_ctable_accel_scale1->clear();
    m_ctable_accel_scale2->clear();
    m_ctable_accel_scale3->clear();
    m_ctable_accel_bias0->clear();
    m_ctable_accel_bias1->clear();
    m_ctable_accel_bias2->clear();
    m_ctable_accel_bias3->clear();
    m_ctable_accel_SN->clear();
    m_ctable_accel_Date->clear();
    m_ctable_accel_Time->clear();
    m_ctable_error->clear();
    m_ctable_error->setToolTip("");

    // 5. Очистка полей KTable
    m_fld_ktable_CRC->clear();
    m_fld_ktable_PN->clear();
    m_fld_Short_bucket_size->clear();
    m_fld_Medium_bucket_size->clear();
    m_fld_Long_bucket_size->clear();
    m_fld_Calibration_time->clear();
    m_fld_Acquire_frequency->clear();
    m_fld_ktable_error->clear();
    m_fld_ktable_error->setToolTip("");

    // 6. Очистка сервисных полей панели статуса ACK
    m_fld_fw_ACK_cmd->clear();
    m_fld_fw_ACK_text->clear();
    m_fld_fw_ACK_code->clear();
    m_fld_fw_ACK_status->clear();
}

//---------------------------------------------------------
QString FIRMWARE_GUI::secs_to_minute( int sec )
{
    QString ctim;
    if( sec > 60 ) {
        int mm = sec / 60;
        int ss = sec - (sec / 60) * 60;
        ctim = QString( "%1 %2 %3 %4" )
                .arg( mm )
                .arg( tr("min") )
                .arg( (ss > 0 ? QString::number(ss) : QStringLiteral("")) )
                .arg( (ss > 0 ? tr("sec") : QStringLiteral("")) );
    } else {
        ctim = QString( "%1 %2" ).arg( sec ).arg( tr("sec") );
    }
    return ctim.trimmed();
}

//---------------------------------------------------------
// Центрирование строки
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина центрирования
// \return Отцентроированная строка
//
QString FIRMWARE_GUI::str_center(const QString &str, int width)
{
    int str_len = str.size();
    if (str_len > width) return str;
    int ww = (width - str_len) / 2.0;
    ww = width - ceil(ww);
    return QString("%1").arg(str.leftJustified(ww, ' '), width);
}

//---------------------------------------------------------
// Выравнивание строки влево
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина выравнивания
// \return результирующая строка
//
QString FIRMWARE_GUI::str_left(const QString &str, int width)
{
    return QString("%1").arg(str, width * -1);
}

//---------------------------------------------------------
// Выравнивание строки вправо
// \param str[in]   - исходная строка
// \param width[in] - требуемая ширина выравнивания
// \return результирующая строка
//
QString FIRMWARE_GUI::str_right(const QString &str, int width)
{
    return QString("%1").arg(str, width);
}

//---------------------------------------------------------
bool FIRMWARE_GUI::wish_write_data() const
{
    return WMSG::confirmation(app(), {}, tr("Are you sure you wish change data?"));
}


//---------------------------------------------------------
bool FIRMWARE_GUI::action_confirmation() const
{
    return WMSG::confirmation(app(), {}, tr("Are you sure you wish to continue?"));
}
