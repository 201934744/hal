#include "graph_widget/graph_navigation_widget_v2.h"

#include "core/log.h"

#include "netlist/gate.h"
#include "netlist/module.h"

#include "gui_globals.h"
#include "gui_utils/netlist.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTreeWidgetItem>

#include <assert.h>
#include <QDebug>

graph_navigation_widget_v2::graph_navigation_widget_v2(QWidget* parent) : QTreeWidget(parent)
{
    setSelectionMode(QAbstractItemView::MultiSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    header()->setStretchLastSection(false);
    setColumnCount(5);
    setHeaderLabels({"Name", "ID", "Type", "Pin", "Parent Module"});
    setAllColumnsShowFocus(true);

    connect(this, &graph_navigation_widget_v2::itemSelectionChanged, this, &graph_navigation_widget_v2::handle_selection_changed);
    connect(this, &graph_navigation_widget_v2::itemDoubleClicked, this, &graph_navigation_widget_v2::handle_item_double_clicked);

    // FIXME for some reason, arrow key navigation does not work on MacOS
}

void graph_navigation_widget_v2::setup(bool direction)
{ 
    clear();

    switch (g_selection_relay.m_focus_type)
    {
        case selection_relay::item_type::none:
        {
            return;
        }
        case selection_relay::item_type::gate:
        {
            std::shared_ptr<gate> g = g_netlist->get_gate_by_id(g_selection_relay.m_focus_id);

            assert(g);

            std::string pin_type   = (direction ? g->get_output_pins() : g->get_input_pins())[g_selection_relay.m_subfocus_index];
            std::shared_ptr<net> n = (direction ? g->get_fan_out_net(pin_type) : g->get_fan_in_net(pin_type));

            assert(n);

            m_origin = hal::node{hal::node_type::gate, g->get_id()};
            m_via_net = n;

            fill_table(direction);

            return;
        }
        case selection_relay::item_type::net:
        {
            std::shared_ptr<net> n = g_netlist->get_net_by_id(g_selection_relay.m_focus_id);

            assert(n);
            assert(n->get_num_of_sources());

            m_origin = hal::node{hal::node_type::gate, 0};
            m_via_net = n;
            
            fill_table(direction);

            return;
        }
        case selection_relay::item_type::module:
        {
            // TODO ???
            return;
        }
    }
}

void graph_navigation_widget_v2::setup(hal::node origin, std::shared_ptr<net> via_net, bool direction)
{
    clear();
    fill_table(direction);
    m_via_net = via_net;
    m_origin = origin;
}

void graph_navigation_widget_v2::hide_when_focus_lost(bool hide)
{
    
}

void graph_navigation_widget_v2::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "KeyDebug:" << "dn:" << (event->key() == Qt::Key_Down) << "/ up:" << (event->key() == Qt::Key_Up);
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Right)
    {
        commit_selection();
        m_via_net = nullptr;
        return;
    }

    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Left)
    {
        Q_EMIT close_requested();
        Q_EMIT reset_focus();
        m_via_net = nullptr;
        return;
    }

    return QTreeWidget::keyPressEvent(event);
}

void graph_navigation_widget_v2::fill_table(bool direction)
{
    int row = 0;
    QMap<u32, QTreeWidgetItem*> created_parents;

    // iterate over all sources, respective destinations, of the via net
    for (const endpoint& e : (direction ? m_via_net->get_destinations() : m_via_net->get_sources()))
    {
        std::shared_ptr<gate> g = e.get_gate();
        if (!g)
        {
            // skip non-gate endpoints
            continue;
        }

        // find the highest module in the hierarchy that we should offer for this
        // gate (if we have a module A that contains both the origin and the target
        // gate, then we don't want to offer navigating to that module or any
        // modules further up the hierarchy)
        std::shared_ptr<module> common_ancestor;
        if (m_origin.id == 0)
        {
            // we're navigating from a net
        }
        else if (m_origin.type == hal::node_type::gate)
        {
            std::shared_ptr<gate> origin = g_netlist->get_gate_by_id(m_origin.id);
            assert(origin);
            common_ancestor = gui_utility::first_common_ancestor({}, {origin, g});
        }
        else if (m_origin.type == hal::node_type::module)
        {
            std::shared_ptr<module> origin = g_netlist->get_module_by_id(m_origin.id);
            assert(origin);
            common_ancestor = gui_utility::first_common_ancestor({origin}, {origin});
        }
        else
        {
            // malformed node type
            assert(false);
        }

        // make item for the gate
        QTreeWidgetItem* item = new QTreeWidgetItem({
            QString::fromStdString(g->get_name()),
            QString::number(g->get_id()),
            QString::fromStdString(g->get_type()->get_name()),
            QString::fromStdString(e.get_pin()),
            QString::fromStdString(g->get_module()->get_name())
        });
        // TODO is there a better way?
        item->setData(1, Qt::ItemDataRole::UserRole, g->get_id());

        // recurse up the hierarchy until one prior to the common ancestor and
        // create entries for the parent gates, reusing ones that have already
        // been created for other gates
        std::shared_ptr<module> parent = g->get_module();
        bool reused_item;
        while(parent != common_ancestor) {
            qDebug() << QString::fromStdString(parent->get_name());
            QTreeWidgetItem* parent_item;
            auto it = created_parents.find(parent->get_id());
            if (it != created_parents.end())
            {
                // reuse the existing item so we can append more child items to it
                // instead of duplicating the entry for the parent
                parent_item = *it;
                reused_item = true;
            }
            else
            {
                QString portname = QString::fromStdString(direction ?
                    parent->get_input_port_name(m_via_net) :
                    parent->get_output_port_name(m_via_net)
                );
                QString type = QString::fromStdString(parent->get_type());
                if (type.isEmpty())
                {
                    type = "<empty-type>";
                }
                type+= " Module";
                auto parents_parent = parent->get_parent_module();
                QString parents_parent_name = QString::fromStdString(parents_parent ? parents_parent->get_name() : "(none)");
                // lazy-init the item for this module if it's not in the map yet
                parent_item = new QTreeWidgetItem({
                    QString::fromStdString(parent->get_name()),
                    QString::number(parent->get_id()),
                    type,
                    portname,
                    parents_parent_name
                });
                // TODO is there a better way?
                parent_item->setData(1, Qt::ItemDataRole::UserRole, parent->get_id());

                created_parents.insert(parent->get_id(), parent_item);
                reused_item = false;
            }
            parent_item->addChild(item);
            parent = parent->get_parent_module();
            item = parent_item;
            assert(parent); // we'll always hit the common ancestor before we run out of parents
        }
        if (!reused_item)
        {
            // insert the top level item into the view if we haven't already done so
            // in an earlier iteration
            insertTopLevelItem(row++, item);
        }
    }
    expandAll();
    resize_to_fit();
}

void graph_navigation_widget_v2::resize_to_fit()
{
    // Qt apparently needs these 2 pixels extra, otherwise you get scollbars

    int width = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 2;
    for (int i = 0; i < columnCount(); i++)
    {
        resizeColumnToContents(i);
        width += columnWidth(i);
    }

    int height = header()->height() + 2;
    height += sum_row_heights(this->invisibleRootItem());

    int MAXIMUM_ALLOWED_HEIGHT = std::min(500, static_cast<QWidget*>(parent())->height());
    int MAXIMUM_ALLOWED_WIDTH = std::min(700, static_cast<QWidget*>(parent())->width());
    setFixedWidth((width > MAXIMUM_ALLOWED_WIDTH) ? MAXIMUM_ALLOWED_WIDTH : width);
    setFixedHeight((height > MAXIMUM_ALLOWED_HEIGHT) ? MAXIMUM_ALLOWED_HEIGHT : height);
}

int graph_navigation_widget_v2::sum_row_heights(const QTreeWidgetItem *itm, bool top)
{
    int row_heights = 0;
    if (!top)
         row_heights = visualItemRect(itm).height();
    for(int i = 0; i < itm->childCount(); i++)
        row_heights += sum_row_heights(itm->child(i), false);
    return row_heights;
}

void graph_navigation_widget_v2::handle_item_double_clicked(QTreeWidgetItem* item)
{
    Q_UNUSED(item)
    commit_selection();
}

void graph_navigation_widget_v2::commit_selection()
{
    if (selectedItems().isEmpty())
    {
        // if we don't have a selection, fake a selection on the item that is
        // currently focused (this allows navigating to a single gate with just
        // the arrow keys)
        setItemSelected(currentItem(), true);
    }

    QSet<u32> target_gates;
    QSet<u32> target_modules;

    for (auto i : selectedItems())
    {
        u32 id = i->data(1, Qt::ItemDataRole::UserRole).toUInt();
        // TODO this is evil
        bool isModule = i->childCount() > 0;
        if (isModule)
        {
            target_modules.insert(id);
        }
        else
        {
            target_gates.insert(id);
        }
    }

    Q_EMIT navigation_requested(m_origin, m_via_net->get_id(), target_gates, target_modules);
}

void graph_navigation_widget_v2::handle_selection_changed()
{
    auto new_selection = selectedItems().toSet();
    auto deselected = m_previous_selection - new_selection;
    auto selected = new_selection - m_previous_selection;
    for (auto& itm : deselected)
    {
        int count = itm->childCount();
        for (int i = 0; i < count; i++)
        {
            // TODO style this on "sunny"
            itm->child(i)->setDisabled(false);
        }
    }
    for (auto& itm : selected)
    {
        int count = itm->childCount();
        for (int i = 0; i < count; i++)
        {
            itm->child(i)->setDisabled(true);
            itm->child(i)->setSelected(false);
        }
    }
    m_previous_selection = new_selection;
}