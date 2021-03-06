//  MIT License
//
//  Copyright (c) 2019 Ruhr-University Bochum, Germany, Chair for Embedded Security. All Rights reserved.
//  Copyright (c) 2019 Marc Fyrbiak, Sebastian Wallat, Max Hoffmann ("ORIGINAL AUTHORS"). All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#pragma once

#include "content_widget/content_widget.h"

#include "def.h"

#include "graph_widget/contexts/graph_context.h"

#include <QListWidget>
#include <QPoint>

namespace hal
{
    class GraphContext;
    class GraphTabWidget;

    class ContextManagerWidget : public ContentWidget
    {
        Q_OBJECT
        Q_PROPERTY(QString new_view_icon_path READ new_view_icon_path WRITE set_new_view_icon_path)
        Q_PROPERTY(QString new_view_icon_style READ new_view_icon_style WRITE set_new_view_icon_style)
        Q_PROPERTY(QString rename_icon_path READ rename_icon_path WRITE set_rename_icon_path)
        Q_PROPERTY(QString rename_icon_style READ rename_icon_style WRITE set_rename_icon_style)
        Q_PROPERTY(QString delete_icon_path READ delete_icon_path WRITE set_delete_icon_path)
        Q_PROPERTY(QString delete_icon_style READ delete_icon_style WRITE set_delete_icon_style)
        Q_PROPERTY(QString duplicate_icon_path READ duplicate_icon_path WRITE set_duplicate_icon_path)
        Q_PROPERTY(QString duplicate_icon_style READ duplicate_icon_style WRITE set_duplicate_icon_style)
        Q_PROPERTY(QString timestamp_icon_path READ timestamp_icon_path WRITE set_timestamp_icon_path)
        Q_PROPERTY(QString timestamp_icon_style READ timestamp_icon_style WRITE set_timestamp_icon_style)


    public:
        ContextManagerWidget(GraphTabWidget* tab_view, QWidget* parent = nullptr);
        void resizeEvent(QResizeEvent* event) override;
        void handle_create_context_clicked();

        void select_view_context(GraphContext* context);

        virtual void setup_toolbar(Toolbar* Toolbar) Q_DECL_OVERRIDE;

        QString new_view_icon_path() const;
        QString new_view_icon_style() const;
        QString rename_icon_path() const;
        QString rename_icon_style() const;
        QString delete_icon_path() const;
        QString delete_icon_style() const;
        QString duplicate_icon_path() const;
        QString duplicate_icon_style() const;
        QString timestamp_icon_path() const;
        QString timestamp_icon_style() const;

        void set_new_view_icon_path(const QString &path);
        void set_new_view_icon_style(const QString &style);
        void set_rename_icon_path(const QString &path);
        void set_rename_icon_style(const QString &style);
        void set_delete_icon_path(const QString &path);
        void set_delete_icon_style(const QString &style);
        void set_duplicate_icon_path(const QString &path);
        void set_duplicate_icon_style(const QString &style);
        void set_timestamp_icon_path(const QString &path);
        void set_timestamp_icon_style(const QString &style);


    public Q_SLOTS:
        void handle_context_created(GraphContext* context);
        void handle_context_renamed(GraphContext* context);
        void handle_context_removed(GraphContext* context);

    private:
        GraphTabWidget* m_tab_view;

        QListWidget* m_list_widget;

        QAction* m_new_view_action;
        QString m_new_view_icon_path;
        QString m_new_view_icon_style;

        QAction* m_rename_action;
        QString m_rename_icon_path;
        QString m_rename_icon_style;

        QAction* m_duplicate_action;
        QString m_duplicate_icon_path;
        QString m_duplicate_icon_style;

        QAction* m_delete_action;
        QString m_delete_icon_path;
        QString m_delete_icon_style;

        QAction* m_timestamp_action;
        QString m_timestamp_icon_path;
        QString m_timestamp_icon_style;

        u32 m_context_counter = 0;

        std::map<QListWidgetItem*, GraphContext*> m_assigned_pointers;

        bool m_show_timestamps = true;

        void handle_context_menu_request(const QPoint& point);

        void handle_open_context_clicked();
        void handle_rename_context_clicked();
        void handle_duplicate_context_clicked();
        void handle_delete_context_clicked();
        void handle_toggle_timestamps_clicked();

        void handle_item_double_clicked(QListWidgetItem*);

        void handle_selection_changed();
    };
}
