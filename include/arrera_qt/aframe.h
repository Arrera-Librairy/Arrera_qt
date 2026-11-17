#ifndef AFRAME_H
#define AFRAME_H

#include <QFrame>
#include <QPainter>
#include <QColor>
#include <QEvent>

class AFrame : public QFrame
{
    Q_OBJECT
public:
    explicit AFrame(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateThemeColors();

    QColor _baseCol;
};

#endif // AFRAME_H
