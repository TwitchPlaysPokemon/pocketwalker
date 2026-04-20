#pragma once
#include <QPushButton>
#include <QKeyEvent>

class KeyBindButton : public QPushButton
{
    Q_OBJECT

public:
    explicit KeyBindButton(QWidget* parent = nullptr);

    int key() const { return current_key; }
    void setKey(int key);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool event(QEvent* event) override;

private:
    void setWaiting(bool waiting);
    void updateLabel();

    int current_key = 0;
    bool waiting = false;
};
