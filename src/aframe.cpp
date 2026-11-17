#include "arrera_qt/aframe.h"
#include "arrera_qt/arrera_theme.h"

AFrame::AFrame(QWidget *parent)
    : QFrame(parent)
    , _cardStyle(CardStyle::Filled)
    , _cornerRadius(16)
    , _hasCustomRadius(false)
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
    _hasCustomRadius = true;
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
    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();

    if (!_hasCustomRadius) {
        _cornerRadius = tokens.radiusLarge;
    }

    switch (_cardStyle) {
    case CardStyle::Filled:
        if (!_hasCustomBgCol) {
            _bgCol = (tokens.surfaceElevated != windowCol)
                         ? tokens.surfaceElevated
                         : tokens.surfaceContainer;
        }
        _borderCol = Qt::transparent;
        break;

    case CardStyle::Elevated:
        if (!_hasCustomBgCol) {
            _bgCol = tokens.surfaceElevated;
        }
        _borderCol = tokens.outlineVariant;
        break;

    case CardStyle::Outlined:
        if (!_hasCustomBgCol) {
            _bgCol = Qt::transparent;
        }
        _borderCol = tokens.outline;
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
