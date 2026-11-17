#ifndef ASLIDER_H
#define ASLIDER_H

#include <QEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>

class ASlider : public QSlider
{
    Q_OBJECT

public:
    explicit ASlider(QWidget *parent = nullptr);
    explicit ASlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void init();
    int valueFromMousePos(const QPoint &pos) const;
    qreal normalizedValue() const;

    bool _hovered;
    bool _dragging;
    bool _showFocusRing;
};

#endif // ASLIDER_H
