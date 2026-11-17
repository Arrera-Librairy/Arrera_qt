#ifndef APUSHBUTTON_H
#define APUSHBUTTON_H

#include <QColor>
#include <QEvent>
#include <QFocusEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QVariantAnimation>

class APushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor)

public:
    enum class Variant {
        Primary,
        Secondary,
        Outlined,
        Flat,
        Destructive
    };
    Q_ENUM(Variant)

    explicit APushButton(QWidget *parent = nullptr);
    explicit APushButton(const QString &text, QWidget *parent = nullptr);
    APushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);

    Variant variant() const;
    void setVariant(Variant variant);

    int cornerRadius() const;
    void setCornerRadius(int radius);

    QColor baseColor() const;
    void setBaseColor(const QColor &col);
    void resetBaseColor();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void variantChanged(Variant variant);
    void cornerRadiusChanged(int radius);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void init();
    void updateThemeColors();
    void animateToCurrentState();
    QColor targetBackgroundColor() const;
    QColor targetBorderColor() const;
    QColor targetTextColor() const;

    Variant _variant;
    int _cornerRadius;
    bool _hasCustomBaseCol;
    bool _showFocusRing;

    QColor _baseCol;
    QColor _hoverCol;
    QColor _pressedCol;
    QColor _currentCol;
    QColor _borderCol;
    QColor _textCol;

    QVariantAnimation *_anim;
};

#endif // APUSHBUTTON_H
