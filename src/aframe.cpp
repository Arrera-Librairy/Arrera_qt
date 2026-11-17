#include "arrera_qt/aframe.h"

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

AFrame::AFrame(QWidget *parent)
    : QFrame(parent)
    , _cardStyle(CardStyle::Filled)
    , _cornerRadius(16)
    , _hasCustomBgCol(false)
{
    setFrameShape(QFrame::NoFrame);
    updateThemeColors();
}

AFrame::CardStyle AFrame::cardStyle() const
{
    return _cardStyle;
}

void AFrame::setCardStyle(CardStyle style)
{
    if (_cardStyle == style) {
        return;
    }
    _cardStyle = style;
    updateThemeColors();
    update();
    emit cardStyleChanged(_cardStyle);
}

int AFrame::cornerRadius() const
{
    return _cornerRadius;
}

void AFrame::setCornerRadius(int radius)
{
    const int clamped = qMax(0, radius);
    if (_cornerRadius == clamped) {
        return;
    }
    _cornerRadius = clamped;
    update();
    emit cornerRadiusChanged(_cornerRadius);
}

QColor AFrame::backgroundColor() const
{
    return _bgCol;
}

void AFrame::setBackgroundColor(const QColor &col)
{
    _hasCustomBgCol = col.isValid();
    if (_hasCustomBgCol) {
        _bgCol = col;
    }
    updateThemeColors();
    update();
}

void AFrame::resetBackgroundColor()
{
    _hasCustomBgCol = false;
    updateThemeColors();
    update();
}

void AFrame::updateThemeColors()
{
    const QColor windowCol = palette().color(QPalette::Window);
    const QColor baseCol = palette().color(QPalette::Base);
    const QColor altBaseCol = palette().color(QPalette::AlternateBase);
    const QColor windowText = palette().color(QPalette::WindowText);
    const bool isDark = windowCol.lightness() < 128;

    switch (_cardStyle) {
    case CardStyle::Filled:
        if (!_hasCustomBgCol) {
            // Utilise Base (surface surélevée) si elle se détache de Window,
            // sinon bascule sur AlternateBase (ex. theme_default clair où Window == Base).
            _bgCol = (baseCol != windowCol) ? baseCol : altBaseCol;
        }
        _borderCol = Qt::transparent;
        break;

    case CardStyle::Elevated:
        if (!_hasCustomBgCol) {
            _bgCol = baseCol;
        }
        _borderCol = withAlpha(windowText, isDark ? 36 : 22);
        break;

    case CardStyle::Outlined:
        if (!_hasCustomBgCol) {
            _bgCol = Qt::transparent;
        }
        _borderCol = withAlpha(windowText, isDark ? 58 : 46);
        break;
    }
}

void AFrame::changeEvent(QEvent *event)
{
    QFrame::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        updateThemeColors();
        update();
    }
}

void AFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const bool isDark = palette().color(QPalette::Window).lightness() < 128;
    const qreal radius = static_cast<qreal>(_cornerRadius);

    QRectF cardRect = rect();

    if (_cardStyle == CardStyle::Elevated) {
        // Ombre douce style GTK4 .card sans offscreen QGraphicsEffect
        cardRect.adjust(1.0, 0.5, -1.0, -2.5);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, isDark ? 55 : 14));
        painter.drawRoundedRect(cardRect.translated(0.0, 2.0), radius, radius);

        painter.setBrush(QColor(0, 0, 0, isDark ? 75 : 22));
        painter.drawRoundedRect(cardRect.translated(0.0, 1.0), radius, radius);
    }

    if (_borderCol.alpha() > 0) {
        if (_cardStyle != CardStyle::Elevated) {
            cardRect.adjust(0.5, 0.5, -0.5, -0.5);
        }
        painter.setPen(QPen(_borderCol, 1.0));
    } else {
        painter.setPen(Qt::NoPen);
    }

    painter.setBrush(_bgCol);
    painter.drawRoundedRect(cardRect, radius, radius);
}
