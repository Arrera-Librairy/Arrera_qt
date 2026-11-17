#ifndef ASWITCH_H
#define ASWITCH_H

#include <QAbstractButton>
#include <QColor>
#include <QEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QVariantAnimation>

class ASwitch : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor)

public:
    explicit ASwitch(QWidget *parent = nullptr);
    explicit ASwitch(const QString &text, QWidget *parent = nullptr);

    QColor activeColor() const;
    void setActiveColor(const QColor &col);
    void resetActiveColor();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;
    void nextCheckState() override;

private:
    void init();
    void updateThemeColors();
    void animateProgress(qreal target);

    qreal _progress;
    bool _hovered;
    bool _showFocusRing;
    bool _hasCustomActiveCol;
    QColor _customActiveCol;

    QVariantAnimation *_anim;
};

#endif // ASWITCH_H
