#include "selection_details_widget/selection_details_widget.h"

#include "gui_globals.h"
#include "netlist/gate.h"
#include "netlist/net.h"
#include "netlist/netlist.h"
#include "searchbar/searchbar.h"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QShortcut>

namespace hal
{
    SelectionDetailsWidget::SelectionDetailsWidget(QWidget* parent) : ContentWidget("Details", parent)
    {
        m_stacked_widget = new QStackedWidget(this);

        m_empty_widget = new QWidget(this);
        m_stacked_widget->addWidget(m_empty_widget);

        m_gate_details = new GateDetailsWidget(this);
        m_stacked_widget->addWidget(m_gate_details);

        m_net_details = new NetDetailsWidget(this);
        m_stacked_widget->addWidget(m_net_details);

        m_module_details = new ModuleDetailsWidget(this);
        m_stacked_widget->addWidget(m_module_details);

        m_item_deleted_label = new QLabel(this);
        m_item_deleted_label->setText("Currently selected item has been removed. Please consider relayouting the Graph.");
        m_item_deleted_label->setWordWrap(true);
        m_item_deleted_label->setAlignment(Qt::AlignmentFlag::AlignTop);
        m_stacked_widget->addWidget(m_item_deleted_label);

        m_stacked_widget->setCurrentWidget(m_empty_widget);

        m_searchbar = new Searchbar(this);
        m_searchbar->hide();

        m_content_layout->addWidget(m_stacked_widget);
        m_content_layout->addWidget(m_searchbar);

        //    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        //    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //    m_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
        //    m_table_widget->setSelectionMode(QAbstractItemView::NoSelection);
        //    m_table_widget->setShowGrid(false);
        //    m_table_widget->setAlternatingRowColors(true);
        //    m_table_widget->horizontalHeader()->setStretchLastSection(true);
        //    m_table_widget->viewport()->setFocusPolicy(Qt::NoFocus);

        connect(&g_selection_relay, &SelectionRelay::selection_changed, this, &SelectionDetailsWidget::handle_selection_update);
    }

    void SelectionDetailsWidget::handle_selection_update(void* sender)
    {
        //called update methods with id = 0 to reset widget to the internal state of not updating because its not visible
        //when all details widgets are finished maybe think about more elegant way

        if (sender == this)
        {
            return;
        }

        unsigned int number_selected_items = g_selection_relay.m_selected_gates.size() + g_selection_relay.m_selected_modules.size() + g_selection_relay.m_selected_nets.size();

        if(number_selected_items != 1)
        {
            m_module_details->update(0);
            m_stacked_widget->setCurrentWidget(m_empty_widget);
            set_name("Details");
            return;
        }

        if (!g_selection_relay.m_selected_modules.isEmpty())
        {
            m_module_details->update(*g_selection_relay.m_selected_modules.begin());
            m_stacked_widget->setCurrentWidget(m_module_details);
            set_name("Module Details");
        }
        else if (!g_selection_relay.m_selected_gates.isEmpty())
        {
            m_searchbar->hide();
            m_module_details->update(0);
            m_gate_details->update(*g_selection_relay.m_selected_gates.begin());
            m_stacked_widget->setCurrentWidget(m_gate_details);
            set_name("Gate Details");
        }
        else if (!g_selection_relay.m_selected_nets.isEmpty())
        {
            m_searchbar->hide();
            m_module_details->update(0);
            m_net_details->update(*g_selection_relay.m_selected_nets.begin());
            m_stacked_widget->setCurrentWidget(m_net_details);
            set_name("Net Details");
        }
    }

    QList<QShortcut *> SelectionDetailsWidget::create_shortcuts()
    {
        QShortcut* search_shortcut = new QShortcut(QKeySequence("Ctrl+f"),this);
        connect(search_shortcut, &QShortcut::activated, this, &SelectionDetailsWidget::toggle_searchbar);

        return (QList<QShortcut*>() << search_shortcut);
    }

    void SelectionDetailsWidget::toggle_searchbar()
    {
        if(m_stacked_widget->currentWidget() != m_module_details)
            return;

        if(m_searchbar->isHidden())
        {
            m_searchbar->show();
            m_searchbar->setFocus();
        }
        else
        {
            m_searchbar->hide();
            setFocus();
        }
    }
}
