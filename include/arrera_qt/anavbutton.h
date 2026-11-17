#ifndef ANAVBUTTON_H
#define ANAVBUTTON_H

#include <QAbstractButton>
#include <QColor>
#include <QEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QVariantAnimation>

class ANavButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(LayoutStyle layoutStyle READ layoutStyle WRITE setLayoutStyle NOTIFY layoutStyleChanged)

public:
    enum class LayoutStyle {
        Horizontal,
        Vertical
    };
    Q_ENUM(LayoutStyle)

    explicit ANavButton(QWidget *parent = nullptr);
    explicit ANavButton(const QString &text, QWidget *parent = nullptr);
    ANavButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);

    LayoutStyle layoutStyle() const;
    void setLayoutStyle(LayoutStyle style);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void layoutStyleChanged(LayoutStyle style);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void init();
    void animateChecked(qreal target);

    LayoutStyle _layoutStyle;
    qreal _checkedProgress;
    bool _hovered;
    bool _showFocusRing;
    QVariantAnimation *_anim;
};

#endif // ANAVBUTTON_H
