#ifndef AFRAME_H
#define AFRAME_H

#include <QColor>
#include <QEvent>
#include <QFrame>
#include <QPainter>

class AFrame : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(CardStyle cardStyle READ cardStyle WRITE setCardStyle NOTIFY cardStyleChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    enum class CardStyle {
        Filled,
        Elevated,
        Outlined
    };
    Q_ENUM(CardStyle)

    explicit AFrame(QWidget *parent = nullptr);

    CardStyle cardStyle() const;
    void setCardStyle(CardStyle style);

    int cornerRadius() const;
    void setCornerRadius(int radius);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &col);
    void resetBackgroundColor();

signals:
    void cardStyleChanged(CardStyle style);
    void cornerRadiusChanged(int radius);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateThemeColors();

    CardStyle _cardStyle;
    int _cornerRadius;
    bool _hasCustomBgCol;

    QColor _bgCol;
    QColor _borderCol;
};

#endif // AFRAME_H
