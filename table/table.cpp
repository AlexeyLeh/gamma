/*!
 * \file   ntable.cpp
 * \author Aleksey Lekhtin
 * \date   04.11.2015
 * \brief  Таблица с записями
 */

#include <QApplication>
#include <QDate>
#include <QEvent>
#include <QKeyEvent>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QGuiApplication>
#endif

#include "dbg.h"
#include "table.h"

//-----------------------------------------------------------
//! Таблица
// \param[in] parent - базовый объект
//
TABLE::TABLE(QWidget *parent) : QTableWidget(parent)
{
    this->setColumnCount(m_labels.size());
    this->setHorizontalHeaderLabels(m_labels);

    this->setSortingEnabled(false);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    // this->setSelectionMode(QAbstractItemView::ContiguousSelection);
    // this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setTabKeyNavigation(false);
    this->setShowGrid(true);

    //-----------------------------------------------------------
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setSectionsMovable(true);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    //this->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignJustify);
    this->horizontalHeader()->setSectionsClickable(true);
    this->horizontalHeader()->setSortIndicatorShown(true);
    //QFont hdr_fnt = this->horizontalHeader()->font();

    //-----------------------------------------------------------
    int min_row_size = this->verticalHeader()->minimumSectionSize();
    this->verticalHeader()->setDefaultSectionSize(min_row_size);
    this->verticalHeader()->setSectionsMovable(true);
    // this->verticalHeader()->resizeSections(QHeaderView::Fixed);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->verticalHeader()->hide();

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // QScrollBar *scroll = this->verticalScrollBar();

    //-----------------------------------------------------------
    //// connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)),
    ////          this, SLOT(slot_column_clicked(int)));

    connect(this->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
             this, SLOT(slot_column_resized(int,int,int)));

    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
             this, SLOT(slot_exec_record(QTableWidgetItem*)));

    // connect(m_table, SIGNAL(activated(const QModelIndex &)),
    //          this, SLOT(slot_exec_record(const QModelIndex &)));

    this->installEventFilter(this);
    this->viewport()->installEventFilter(this);
}

//-----------------------------------------------------------
TABLE::~TABLE()
{
    DBG(4);
    m_labels.clear();
}

//// /*----------------------------------------------------------*/
//// void TABLE::slot_column_clicked(int column)
//// {
////     DBG(2);
////     Q_UNUSED(column);
////     const int row = currentRow();
////     if (row >= 0 && row < rowCount())
////     {
////         select_row(row);
////     }
//// }

/*----------------------------------------------------------*/
void TABLE::slot_column_resized(int column_pos, int oldSize, int newSize)
{
    DBG(4);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);
    Q_UNUSED(column_pos);
    ////emit signal_save_settings();
}

//-----------------------------------------------------------
void TABLE::slot_exec_record(const QModelIndex & index)
{
    int nrow = index.row();
    //emit signal__...();
    Q_UNUSED(nrow);
}

//-----------------------------------------------------------
void TABLE::slot_exec_record(QTableWidgetItem *item)
{
    if (!item) return;
    int nrow = item->row();
    //emit signal__...();
    Q_UNUSED(nrow);
}

//-----------------------------------------------------------
bool TABLE::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != this->viewport())
        return QObject::eventFilter(obj, ev);

    QEvent::Type type = ev->type();

    if (type != QEvent::KeyPress &&
        type != QEvent::MouseButtonDblClick)
    {
        return QObject::eventFilter(obj, ev);
    }

    int key = 0;
    QKeyEvent *keyEvent = nullptr;
    Qt::KeyboardModifiers mod = Qt::NoModifier;
    Qt::MouseButton but = Qt::NoButton;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    mod = QGuiApplication::keyboardModifiers();
#else
    mod = qApp->keyboardModifiers();
#endif

    if (type == QEvent::KeyPress)
    {
        keyEvent = static_cast<QKeyEvent*>(ev);
        key = keyEvent->key();
    }
    else if (type == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
        but = mouseEvent->button();
    }

#if 0 /* debug... */
    bool pressed = false;
    qDebug() << "\n--- keyEvent: " << keyEvent << "\n";

    if (mod == Qt::ControlModifier)
    {
        LOG::dbg2("+++ Control \n");
        qDebug() << mod;
    }

    if (key == Qt::Key_Enter)
    {
        LOG::dbg2("+++ Enter   (0x%x): 0x%x\n", Qt::Key_Enter, key);
        pressed = true;
    }
    else if (key == Qt::Key_Return)
    {
        LOG::dbg2("+++ Return  (0x%x): 0x%x\n", Qt::Key_Return, key);
        pressed = true;
    }

    if (mod == Qt::ControlModifier ||
        mod == (Qt::ControlModifier|Qt::KeypadModifier))
    {
        if (key == Qt::Key_Enter)
            LOG::dbg2("--- Control+Enter\n");

        if (key == Qt::Key_Return)
            LOG::dbg2("--- Control+Return\n");

        if (key == Qt::Key_PageUp)
            LOG::dbg2("--- Control+PageUp\n");

        if (key == Qt::Key_PageDown)
            LOG::dbg2("--- Control+PageDown\n");

        if (key == Qt::Key_Home)
            LOG::dbg2("--- Control+PageHome\n");

        if (key == Qt::Key_End)
            LOG::dbg2("--- Control+PageEnd\n");
    }
    if (pressed) LOG::dbg2("\n");
#endif


    //--- Обработка логики событий ---

    if (but == Qt::LeftButton)
    {
        // Ctrl + Двойной клик левой кнопкой мыши
        if (mod == Qt::ControlModifier ||
            mod == (Qt::ControlModifier | Qt::KeypadModifier))
        {
            // slot_...(); // Ваша логика
            ev->accept();
            return true;
        }
    }
    else if (key == Qt::Key_Enter || key == Qt::Key_Return)
    {
        if (keyEvent && keyEvent->isAutoRepeat()) return true;

        if (mod == Qt::ControlModifier ||
            mod == (Qt::ControlModifier | Qt::KeypadModifier))
        {
            // Ctrl + Enter
            // slot_...();
        }
        else
        {
            // Enter без модификаторов
            slot_exec_record(this->currentIndex());
        }
        ev->accept();
        return true;
    }
    else if (key == Qt::Key_Home)
    {
        if (keyEvent && keyEvent->isAutoRepeat()) return true;

        this->clearSelection();
        this->selectRow(0);
        this->scrollToTop();
        ev->accept();
        return true;
    }
    else if (key == Qt::Key_End)
    {
        if (keyEvent && keyEvent->isAutoRepeat()) return true;

        int lastRow = this->rowCount() - 1;
        if (lastRow >= 0)
        {
            this->clearSelection();
            this->selectRow(lastRow);
            this->scrollToBottom();
        }
        ev->accept();
        return true;
    }

    return QObject::eventFilter(obj, ev);
}


#if 0
/*----------------------------------------------------------*/
bool TABLE::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != this->viewport())
        return QObject::eventFilter(obj, ev);

    if (ev->type() != QEvent::KeyPress &&
        ev->type() != QEvent::KeyRelease &&
        ev->type() != QEvent::MouseButtonDblClick)
        return QObject::eventFilter(obj, ev);

    int key = 0;
    QKeyEvent *keyEvent = nullptr;
    Qt::KeyboardModifiers mod = Qt::NoModifier;
    Qt::MouseButton but = Qt::NoButton;

    if (ev->type() == QEvent::KeyPress)
    {
        keyEvent = static_cast<QKeyEvent*>(ev);
        key = keyEvent->key();
        mod = keyEvent->modifiers();
        m_current_mod = mod;
    }
    else if (ev->type() == QEvent::KeyRelease)
    {
        m_current_mod = Qt::NoModifier;
    }
    else if (ev->type() != QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
        but = mouseEvent->button();
    }

#if 0 /* debug... */
    bool pressed = false;
    qDebug() << "\n--- keyEvent: " << keyEvent << "\n";

    if (mod == Qt::ControlModifier)
    {
        LOG::dbg2("+++ Control \n");
        qDebug() << mod;
    }

    if (key == Qt::Key_Enter)
    {
        LOG::dbg2("+++ Enter   (0x%x): 0x%x\n", Qt::Key_Enter, key);
        pressed = true;
    }
    else if (key == Qt::Key_Return)
    {
        LOG::dbg2("+++ Return  (0x%x): 0x%x\n", Qt::Key_Return, key);
        pressed = true;
    }

    if (mod == Qt::ControlModifier ||
        mod == (Qt::ControlModifier|Qt::KeypadModifier))
    {
        if (key == Qt::Key_Enter)
            LOG::dbg2("--- Control+Enter\n");

        if (key == Qt::Key_Return)
            LOG::dbg2("--- Control+Return\n");

        if (key == Qt::Key_PageUp)
            LOG::dbg2("--- Control+PageUp\n");

        if (key == Qt::Key_PageDown)
            LOG::dbg2("--- Control+PageDown\n");

        if (key == Qt::Key_Home)
            LOG::dbg2("--- Control+PageHome\n");

        if (key == Qt::Key_End)
            LOG::dbg2("--- Control+PageEnd\n");
    }
    if (pressed) LOG::dbg2("\n");
#endif

    /*----------------------------------------------------------*/
    if (but == Qt::LeftButton)
    {
        // Ctrl + Mouse Left Button
        if (m_current_mod == Qt::ControlModifier ||
            m_current_mod == (Qt::ControlModifier|Qt::KeypadModifier))
        {
            m_current_mod = Qt::NoModifier;
            //slot_...();
            ev->accept();
            return true;
        }
    }
    else if (key == Qt::Key_Enter || key == Qt::Key_Return)
    {
        if (keyEvent->isAutoRepeat()) return true;
        if (mod == Qt::ControlModifier ||
            mod == (Qt::ControlModifier|Qt::KeypadModifier))
        {
            // Ctrl + Enter
            //slot_...();
        }
        else
        {
            // Enter
            slot_exec_record(this->currentIndex());
        }
        ev->accept();
        return true;
    }
    else if (key == Qt::Key_Home)
    {
        if (keyEvent->isAutoRepeat()) return true;
        int row = 0;
        this->clearSelection();
        this->selectRow(row);
        this->scrollToTop();
        //LOG::dbg5("JUMP TOP \n");
        ev->accept();
        return true;
    }
    else if (key == Qt::Key_End)
    {
        if (keyEvent->isAutoRepeat()) return true;
        int row = this->rowCount()-1;
        this->clearSelection();
        this->selectRow(row);
        this->scrollToBottom();
        //LOG::dbg5("JUMP BOTTOM \n");
        ev->accept();
        return true;
    }
    // else if (key == Qt::Key_PageUp)
    // {
    //    // Ctrl+PgUp - jump into '..'
    //    if (mod == Qt::ControlModifier)
    //    {
    //       slot_up_dir();
    //      LOG::dbg5("JUMP DIR UP \n");
    //       ev->accept();
    //       return true;
    //    }
    // }
    // else if (key == Qt::Key_PageDown)
    // {
    //    //  Ctrl+PgDown - jump into dir */
    //    if (mod == Qt::ControlModifier)
    //    {
    //       slot_go_into_dir();
    //       LOG::dbg5("JUMP DIR DOWN \n");
    //       ev->accept();
    //       return true;
    //    }
    // }

    return QObject::eventFilter(obj, ev);
}
#endif

//-----------------------------------------------------------
//! Количество записей в таблице
// \return число записей
//
int TABLE::total_records() const
{
    return this->rowCount();
}

//-----------------------------------------------------------
//! Определить список колонок в таблице
// \param labels - описание колонок
// \note Содержание таблицы будет очищено, геометрия будет пересчитана.
//
void TABLE::set_labels(const QStringList &labels)
{
    clear();
    m_labels = labels;
    setColumnCount(m_labels.size());
    setHorizontalHeaderLabels(m_labels);
}

//-----------------------------------------------------------
//! Список колонок в таблице
// \return labels - описание колонок
//
const QStringList &TABLE::get_labels() const
{
    return m_labels;
}

//-----------------------------------------------------------
//! Получить поле из записи таблицы
// \param[in] nrow - номер строки таблицы
// \param[in] ncol - номер колонки таблицы
// \return строка с данными
//
QString TABLE::get_field(int nrow, int ncol) const
{
    if (nrow < 0) return QString();
    //// TableItem *item = static_cast<TableItem*>(this->item(nrow, ncol));
    //// if (item == nullptr) return {};
    //// return item->text();

    auto *item = this->item(nrow, ncol);
    if (item == nullptr) return {};
    return item->text();
}

QTableWidgetItem *TABLE::get_field_item(int nrow, int ncol) const
{
    if (nrow < 0 || ncol < 0) return nullptr;
    return this->item(nrow, ncol);
}

//-----------------------------------------------------------
//! Задать значение поля в записи таблицы
// \param[in] nrow - номер строки таблицы
// \param[in] ncol - номер колонки таблицы
// \param[in] str  - строка с данными
//
QTableWidgetItem *TABLE::set_field_text(int nrow, int ncol, const QString &text)
{
    if (nrow < 0 || nrow >= rowCount()) return nullptr;
    if (ncol < 0 || ncol >= columnCount()) return nullptr;

    auto *item = this->item(nrow, ncol);
    if (!item)
    {
         item = new TableItem(text);
         if (!item) return nullptr;
         setItem(nrow, ncol, item);
    }

    item->setText(text);
    item->setToolTip(text);
    return item;
}

//// //-----------------------------------------------------------
//// void TABLE::set_field_text(QTableWidgetItem *item, const QString &text)
//// {
////     if (!item) return;
////     item->setText(text);
////     item->setToolTip(text);
//// }

//-----------------------------------------------------------
//! Задать выравнивание значения в поле таблицы
// \param[in] nrow - номер строки таблицы
// \param[in] ncol - номер колонки таблицы
// \param[in] align  - тип выравнивания
//
QTableWidgetItem *TABLE::set_field_alignment(int nrow, int ncol, Qt::Alignment align)
{
    if (nrow < 0 || ncol < 0) return nullptr;
    auto *item = this->item(nrow, ncol);
    if (item) {
        item->setTextAlignment(align);
    }
    return item;
}

//-----------------------------------------------------------
//! Задать выравнивание значения в поле таблицы
// \param[in] nrow - номер строки таблицы
// \param[in] ncol - номер колонки таблицы
// \param[in] align  - тип выравнивания
//
//-----------------------------------------------------------
void TABLE::set_field_alignment(QTableWidgetItem *item, Qt::Alignment align)
{
    if (!item) return;
    item->setTextAlignment(align);
}

//-----------------------------------------------------------
void TABLE::set_field_color(int nrow, int ncol, const QBrush &foregr, const QBrush &backgr)
{
    if (nrow < 0 || ncol < 0) {
        return;
    }

    auto *item = this->item(nrow, ncol);
    if (item) {
        item->setForeground(foregr);
        item->setBackground(backgr);
    }
}

//-----------------------------------------------------------
void TABLE::set_field_font(int nrow, int ncol, const QFont &fnt)
{
    if (nrow < 0) return;
    auto *item = this->item(nrow, ncol);
    set_field_font(item, fnt);
}

//-----------------------------------------------------------
void TABLE::set_field_font(QTableWidgetItem *item, const QFont &fnt)
{
    if (!item) return;
    item->setFont(fnt);
}

//-----------------------------------------------------------
void TABLE::set_record_font(int nrow, const QFont &fnt)
{
    if (nrow < 0) return;
    int ncols = columnCount();
    for(int i = 0; i < ncols; ++i)
    {
        auto *item = this->item(nrow, i);
        set_field_font(item, fnt);
    }
}

//-----------------------------------------------------------
//! Получить список выделенных записей таблицы
// \param[out] row_list - список выделенных записей
// \param[out] nrow     - номер первой строки выделения
//
void TABLE::get_selected_list(QList<QModelIndex>& rowlist, int &current_nrow)
{
    current_nrow = 0;
    if (this->rowCount() == 0) return;
    QItemSelectionModel *selection = this->selectionModel();
    if (selection == nullptr) return;

    rowlist = selection->selectedRows();
    if (rowlist.size() == 0) return;
    if (rowlist.size() == 1)
    {
        int nrow = rowlist.first().row();
        current_nrow = nrow;
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //// qStableSort(row_list);
    std::stable_sort(rowlist.begin(), rowlist.end());
    int nrow = rowlist.first().row();
    current_nrow = nrow;
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------
//! Получить номер текущей записи
// \return номер текущей строки таблицы
//
int TABLE::get_current_num() const
{
    if (rowCount() == 0) return -1;
    return currentRow();
}

//-----------------------------------------------------------
//! Установить признак 'отмечено'
// \param[in] nrow - номер строки таблицы
// \param[in] mark - да / нет
//
void TABLE::set_record_mark(int nrow, bool mark)
{
    if (nrow < 0) return;
    auto *item = static_cast<TableItem*>(this->item(nrow, 0));
    if (item) item->m_mark = mark;
}

//-----------------------------------------------------------
//! Узнать состояние признака 'отмечено'
// \param[in] nrow - номер строки таблицы
// \return да / нет
//
bool TABLE::get_record_mark(int nrow) const
{
    if (nrow < 0) return false;
    auto *item = static_cast<TableItem*>(this->item(nrow, 0));
    if (!item) return false;
    return item->m_mark;
}

//-----------------------------------------------------------
//! Перейти на строку таблицы
//
void TABLE::select_row(int nrow)
{
    //if (!this->isVisible()) return;
    if (nrow < 0) return;
    nrow = qMin(nrow, this->rowCount()-1);
    this->clearSelection();
    if (nrow < 0) return;
    auto *item = this->item(nrow, 0);
    this->setCurrentItem(item);
    this->scrollToItem(item, QAbstractItemView::EnsureVisible);
    selectRow(nrow);
    /* QAbstractItemView::PositionAtCenter */
}

//-----------------------------------------------------------
//! Снять выделение со строк таблицы
// \param[out] nrow - номер строки таблицы
//
void TABLE::deselect_row(int nrow)
{
    auto *selection = this->selectionModel();
    if (!selection) return;

    auto *md = this->model();
    if (md)
    {
        for(int ncol = 0; ncol < m_labels.size(); ++ncol)
        {
            selection->select(md->index(nrow, ncol, QModelIndex()),
                               QItemSelectionModel::Deselect);
            //QItemSelectionModel::Current);
        }
    }
}

//-----------------------------------------------------------
//! Изменить запись в таблице
//
int TABLE::set_record(int nrow, const QStringList &data, const QVector<Qt::Alignment> &align, const QString &tip)
{
    if (nrow < 0 || nrow >= rowCount()) return -1;

    QFont fnt = font();
    fnt.setBold(true);

    int ncols = columnCount();
    for(int i = 0; i < ncols; ++i)
    {
        QString buf;
        if (i < data.size()) buf = data.at(i);

        TableItem *item = new TableItem();
        item->setText(buf);

        if (!tip.isEmpty())
            item->setToolTip(tip);
        else
            item->setToolTip(buf);

        if (!align.isEmpty() && i < align.size())
            item->setTextAlignment(align[i]);
        //// else
        ////     item->setTextAlignment(Qt::AlignCenter);

        item->setFont(fnt);
        setItem(nrow, i, item);
    }

    return nrow;
}

//-----------------------------------------------------------
//! Добавить запись в таблицу
// \param[in] data - список данных для отображения в строке таблицы
// \return номер новой записи
//
int TABLE::add_record(const QStringList &data)
{
    int nrow = rowCount();
    insertRow(nrow);
    if (data.size() == 0)
    {
        for(int ncol = 0; ncol < columnCount(); ++ncol)
        {
            setItem(nrow, ncol, new TableItem());
        }
        return nrow;
    }

    auto num = data.size();
    for(int ncol = 0; ncol < columnCount() && ncol < num; ++ncol)
    {
        TableItem *item = new TableItem();
        item->setText(data.at(ncol));
        item->setToolTip(data.at(ncol));
        setItem(nrow, ncol, item);
    }
    return nrow;
}

//-----------------------------------------------------------
//! Удалить запись таблицы
// \param[in] nrow - номер удаляемой строки таблицы
//
void TABLE::del_record(int nrow)
{
    if (nrow < 0) return;
    if (nrow > rowCount()-1) return;
    removeRow(nrow);
    //select_rec(nrow);
}

//-----------------------------------------------------------
//! Удалить несколько записей таблицы
// \param[in] row_list - список удаляемых строк таблицы
//
void TABLE::del_records(const QList<QModelIndex> & row_list)
{
    QList<QModelIndex> rowlist = row_list;
    //// qStableSort(rowlist);
    std::stable_sort(rowlist.begin(), rowlist.end());
    int nrow = rowlist.first().row();
    int cnt = rowlist.size();
    if (cnt == 0)
    {
        del_record(nrow);
        return;
    }

    QList<QModelIndex>::const_reverse_iterator it;
    it = rowlist.rbegin();
    while(it != rowlist.rend())
    {
        const QModelIndex & md = *it;
        //qDebug() << md.row();
        removeRow(md.row());
        ++it;
    }
    //qDebug() << "selected:" << nrow;
    //select_rec(nrow);
}

//-----------------------------------------------------------
//! Удалить несколько записей таблицы
// \param[in] row_list - список номеров удаляемых строк таблицы
//
void TABLE::del_records(const QList<int> & row_list)
{
    QList<int> rowlist = row_list;
    //// qStableSort(rowlist);
    std::stable_sort(rowlist.begin(), rowlist.end());
    int nrow = rowlist.first();
    int cnt = rowlist.size();
    if (cnt == 0)
    {
        del_record(nrow);
        return;
    }

    QList<int>::const_reverse_iterator it;
    it = rowlist.rbegin();
    while(it != rowlist.rend())
    {
        //qDebug() << (*it);
        removeRow((*it));
        ++it;
    }
    //qDebug() << "selected:" << nrow;
    //select_rec(nrow);
}

//-----------------------------------------------------------
QString TABLE::date_to_yyyymmdd(const QString & date_yyyy_mm_dd) const
{
    QString buf;
    QDate dt = QDate::fromString(date_yyyy_mm_dd, "yyyy-MM-dd");
    if (!dt.isValid()) return date_yyyy_mm_dd;
    //// QLocale lo;
    if (locale().language() == QLocale::Russian ||
        locale().country()  == QLocale::RussianFederation)
        buf = dt.toString("dd.MM.yyyy");
    else
        buf = dt.toString(Qt::TextDate);
    return buf;
}

//--- Constructor ------------------------------------------
TableItem::TableItem(bool date_rus_format) :
    m_date_rus(date_rus_format),
    m_mark(false)
{
    Qt::ItemFlags flag = flags();
    flag &= (~Qt::ItemIsEditable);
    setFlags(flag);
}

TableItem::TableItem(const QString &text, bool date_rus_format) :
    TableItem(date_rus_format)
{
     if (!text.isNull()) setText(text);
}


//-----------------------------------------------------------
bool TableItem::operator<(const QTableWidgetItem& other) const
{
    // const TableItem *item2 = dynamic_cast<const TableItem*>(&other);

    const QVariant d1 = data(Qt::DisplayRole);
    const QVariant d2 = other.data(Qt::DisplayRole); // d2 берем напрямую из аргумента

    if (d1.userType() != d2.userType()) {
        return d1.toString().trimmed().compare(d2.toString().trimmed(), Qt::CaseInsensitive) < 0;
    }

    switch (d1.userType())
    {
    case QMetaType::QString:
        return d1.toString().trimmed().compare(d2.toString().trimmed(), Qt::CaseInsensitive) < 0;

    case QMetaType::Int:       return d1.toInt() < d2.toInt();
    case QMetaType::UInt:      return d1.toUInt() < d2.toUInt();
    case QMetaType::LongLong:  return d1.toLongLong() < d2.toLongLong();
    case QMetaType::ULongLong: return d1.toULongLong() < d2.toULongLong();
    case QMetaType::Double:    return d1.toDouble() < d2.toDouble();
    case QMetaType::QChar:     return d1.toChar() < d2.toChar();
    case QMetaType::QTime:     return d1.toTime() < d2.toTime();
    case QMetaType::QDateTime: return d1.toDateTime() < d2.toDateTime();

    case QMetaType::QDate:
        return d1.toDate() < d2.toDate();

    case QMetaType::UnknownType:
        return d2.userType() == QMetaType::UnknownType;

    default:
        return d1.toString().trimmed().compare(d2.toString().trimmed(), Qt::CaseInsensitive) < 0;
    }
}



#if 0
//-----------------------------------------------------------
bool TableItem::operator<(const QTableWidgetItem & other) const
{
    bool rc = false;
    //QTableWidget *tbl = tableWidget();
    //Qt::SortOrder sort_order = tbl->horizontalHeader()->sortIndicatorOrder();
    //int sort_ncol  = tbl->horizontalHeader()->sortIndicatorSection();

    TableItem &item2 = (TableItem &)other;
    const QVariant d1 = data(Qt::DisplayRole);
    const QVariant d2 = item2.data(Qt::DisplayRole);

    QString str1 = d1.toString().trimmed();
    QString str2 = d2.toString().trimmed();

    //-----------------------------------------------------------
    // if (sort_ncol == COL_DATE)
    // {
    //    QDate dt1;
    //    QDate dt2;
    //    if (m_date_rus == true)
    //    {
    //       dt1 = QDate::fromString(str1, "dd.MM.yyyy");
    //       dt2 = QDate::fromString(str2, "dd.MM.yyyy");
    //    }
    //    else
    //    {
    //       dt1 = QDate::fromString(str1);
    //       dt2 = QDate::fromString(str2);
    //    }

    //    if (str1.isEmpty())
    //    {
    //       rc = true;
    //       if (str2.isEmpty()) rc = (sort_order ? false : true);
    //       return rc;
    //    }
    //    else if (str2.isEmpty())
    //    {
    //       return false;
    //    }

    //    rc = (dt1 >= dt2);
    //    return !rc;
    // }

    //-----------------------------------------------------------
    //switch(d1.metaType().id())
    switch(d1.type())
    {
    case QMetaType::QString:
        rc = str1.compare(str2, Qt::CaseInsensitive) < 0;
        break;
    case QMetaType::Int:
        rc = d1.toInt() < d2.toInt(); break;
    case QMetaType::UInt:
        rc = d1.toUInt() < d2.toUInt(); break;
    case QMetaType::LongLong:
        rc = d1.toLongLong() < d2.toLongLong(); break;
    case QMetaType::ULongLong:
        rc = d1.toULongLong() < d2.toULongLong(); break;
    case QMetaType::Double:
        rc = d1.toDouble() < d2.toDouble(); break;
    case QMetaType::QChar:
        rc = d1.toChar() < d2.toChar(); break;
    case QMetaType::QDate:
        rc = d1.toDate() > d2.toDate(); break;
    case QMetaType::QTime:
        rc = d1.toTime() < d2.toTime(); break;
    case QMetaType::QDateTime:
        rc = d1.toDateTime() < d2.toDateTime(); break;
    case QMetaType::UnknownType:
        //rc = (d2.metaType().id() == QMetaType::UnknownType); break;
        rc = (d2.type() == QVariant::Invalid); break;
    default:
        rc = str1.compare(str2, Qt::CaseInsensitive) < 0; break;
    }
    return !rc;
}
#endif
