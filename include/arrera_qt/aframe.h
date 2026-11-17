#ifndef AFRAME_H
#define AFRAME_H

#include <QFrame>
#include <QPainter>

class AFrame : public QFrame
{
    Q_OBJECT
public:
    explicit AFrame(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // AFRAME_H
