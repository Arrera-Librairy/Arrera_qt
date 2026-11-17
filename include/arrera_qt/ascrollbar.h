#ifndef ASCROLLBAR_H
#define ASCROLLBAR_H

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>

class AScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    explicit AScrollBar(QWidget *parent = nullptr);
    explicit AScrollBar(Qt::Orientation orientation, QWidget *parent = nullptr);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void init();
    QRectF handleRect() const;

    bool _hovered;
    bool _dragging;
    int _dragStartValue;
    QPoint _dragStartPos;
};

#endif // ASCROLLBAR_H
