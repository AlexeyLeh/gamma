/*!
 * \file   table.h
 * \brief  Таблица
 * \author Aleksey Lekhtin
 * \date   04.11.2015
 */

#ifndef _TABLE_RECS_H_
#define _TABLE_RECS_H_

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

class TableItem;

// Таблица с записями
class TABLE : public QTableWidget
{
    Q_OBJECT;

public:
    explicit TABLE(QWidget *parent = 0);
    ~TABLE() override;
    QString date_to_yyyymmdd(const QString& date_yyyy_mm_dd) const;

private slots:
    //// void    slot_column_clicked(int column);
    void    slot_column_resized(int column, int oldSize, int newSize);
    void    slot_exec_record(const QModelIndex& parent);
    void    slot_exec_record(QTableWidgetItem * item);

private:
    bool    eventFilter(QObject *obj, QEvent *event) override;

public:
    void    set_labels(const QStringList &labels);
    const   QStringList &get_labels() const;
    int     total_records() const;

    int     add_record(const QStringList &data = {});
    int     set_record(int nrow, const QStringList &data, const QVector<Qt::Alignment> &align = {}, const QString &tip = {});
    void    del_record(int nrow);
    void    del_records(const QList<QModelIndex> &row_list);
    void    del_records(const QList<int> &row_list);

    QString get_field(int nrow, int ncol) const;
    QTableWidgetItem *get_field_item(int nrow, int ncol) const;

    QTableWidgetItem *set_field_text(int nrow, int ncol, const QString &text);

    QTableWidgetItem *set_field_alignment(int nrow, int ncol, Qt::Alignment align);
    void              set_field_alignment(QTableWidgetItem *item, Qt::Alignment align);

    void    set_field_color(int nrow, int ncol, const QBrush &foregr, const QBrush &backgr);
    inline  void set_field_color(int nrow, int ncol, QColor foregr, QColor backgr)
    {
        set_field_color(nrow, ncol, QBrush(foregr), QBrush(backgr));
    }

    void    set_field_font(int nrow, int ncol, const QFont &fnt);
    static  void set_field_font(QTableWidgetItem *item, const QFont &fnt);
    void    set_record_font(int nrow, const QFont &fnt);

    void    set_record_mark(int nrow, bool mark);
    bool    get_record_mark(int nrow) const;

    void    select_row(int nrow);
    void    deselect_row(int row);
    int     get_current_num() const;
    void    get_selected_list(QList<QModelIndex>& rows_list, int &current_nrow);

    //-----------------------------------------------------------
private:
    Qt::KeyboardModifiers   m_current_mod{Qt::NoModifier};
    QStringList             m_labels;   /*!< заголовки колонок */
};


//-----------------------------------------------------------
//! Переопределение класса ячейки таблицы для хранения дополнительных данных
//
class TableItem : public QTableWidgetItem
{
public:
    explicit TableItem(bool date_rus_format = false);
    TableItem(const QString &text, bool date_rus_format = false);
    virtual ~TableItem() = default;

    virtual bool operator<(const QTableWidgetItem& other) const override;

private:
public:
    bool              m_date_rus{};  // признак даты формата Russian
    bool              m_mark{};      // флаг для признака 'выделено' или 'обработано'
};

#endif // _TABLE_RECS_H_
