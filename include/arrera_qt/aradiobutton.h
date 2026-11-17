#ifndef ARADIOBUTTON_H
#define ARADIOBUTTON_H

#include <QEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QRadioButton>
#include <QVariantAnimation>

class ARadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit ARadioButton(QWidget *parent = nullptr);
    explicit ARadioButton(const QString &text, QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool hitButton(const QPoint &pos) const override;

private:
    void init();
    void animateProgress(qreal target);

    qreal _progress;
    bool _hovered;
    bool _showFocusRing;
    QVariantAnimation *_anim;
};

#endif // ARADIOBUTTON_H
