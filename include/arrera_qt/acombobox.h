#ifndef ACOMBOBOX_H
#define ACOMBOBOX_H

#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QVariantAnimation>

class AComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)

public:
    explicit AComboBox(QWidget *parent = nullptr);

    int cornerRadius() const;
    void setCornerRadius(int radius);

    void showPopup() override;
    void hidePopup() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateThemeColors();
    void updatePopupStyle();
    void animateToCurrentState();
    QColor targetBackgroundColor() const;

    bool _popupOpen;
    bool _showFocusRing;
    bool _hasCustomRadius;
    int _cornerRadius;

    QColor _baseCol;
    QColor _hoverCol;
    QColor _openCol;
    QColor _borderCol;
    QColor _currentCol;

    QVariantAnimation *_anim;
};

#endif // ACOMBOBOX_H
