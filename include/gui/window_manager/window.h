#pragma once

#include <QWidget>

class overlay;
class Window_effect_layer;
class window_toolbar;
class workspace;

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr);

    void update_layout();

    void lock(overlay* const o, QGraphicsEffect* const e);
    void unlock();

    void set_active_widget(QWidget* widget);

    void repolish();

    window_toolbar* m_toolbar;
    workspace* const m_workspace;

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Window_effect_layer* m_effect_layer;
    QWidget* m_active_widget;
    overlay* m_overlay;
    QGraphicsEffect* m_effect;
};
