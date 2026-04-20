#include "key_bind_button.h"
#include <QMouseEvent>

KeyBindButton::KeyBindButton(QWidget* parent)
    : QPushButton(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void KeyBindButton::setKey(int key)
{
    current_key = key;
    setWaiting(false);
}

void KeyBindButton::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    setWaiting(true);
    setFocus();
}

void KeyBindButton::keyPressEvent(QKeyEvent* event)
{
    if (!waiting)
    {
        QPushButton::keyPressEvent(event);
        return;
    }

    const int key = event->key();

    if (key == Qt::Key_Escape)
    {
        setWaiting(false);
        return;
    }

    if (key == Qt::Key_Shift ||
        key == Qt::Key_Control ||
        key == Qt::Key_Alt ||
        key == Qt::Key_Meta)
        return;

    current_key = key;
    setWaiting(false);
}

void KeyBindButton::focusOutEvent(QFocusEvent* event)
{
    setWaiting(false);
    QPushButton::focusOutEvent(event);
}

bool KeyBindButton::event(QEvent* event)
{
    if (waiting && event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Tab)
        {
            keyPressEvent(keyEvent);
            return true;
        }
    }

    return QPushButton::event(event);
}

void KeyBindButton::setWaiting(bool w)
{
    waiting = w;
    updateLabel();
}

void KeyBindButton::updateLabel()
{
    if (waiting)
        setText("[Press Key]");
    else if (current_key == 0)
        setText("(unbound)");
    else
        setText(QKeySequence(current_key).toString());
}
