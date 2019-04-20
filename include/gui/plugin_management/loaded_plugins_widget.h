//  MIT License
//
//  Copyright (c) 2019 Marc Fyrbiak
//  Copyright (c) 2019 Sebastian Wallat
//  Copyright (c) 2019 Max Hoffmann
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

#ifndef LOADED_PLUGINS_WIDGET_H
#define LOADED_PLUGINS_WIDGET_H

#include <QScrollArea>

class QFrame;
class QVBoxLayout;

class loaded_plugins_widget : public QScrollArea
{
    Q_OBJECT

public:
    explicit loaded_plugins_widget(QWidget* parent = nullptr);

public Q_SLOTS:
    void handle_plugin_loaded(const QString& name, const QString& path);
    void handle_plugin_unloaded(const QString& name, const QString& path);

private:
    QFrame* m_container;
    QVBoxLayout* m_layout;
    QFrame* m_spacer;
};

#endif    // LOADED_PLUGINS_WIDGET_H
