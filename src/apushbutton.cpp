#include "arrera_qt/apushbutton.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>

namespace {
QColor blendColors(const QColor &base, const QColor &overlay, qreal ratio)
{
    const qreal clamped = qBound(0.0, ratio, 1.0);
    const qreal inv = 1.0 - clamped;
    return QColor::fromRgbF(
        base.redF() * inv + overlay.redF() * clamped,
        base.greenF() * inv + overlay.greenF() * clamped,
        base.blueF() * inv + overlay.blueF() * clamped,
        base.alphaF() * inv + overlay.alphaF() * clamped
    );
}

QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

APushButton::APushButton(QWidget *parent)
    : QPushButton(parent)
    , _variant(Variant::Primary)
    , _cornerRadius(-1)
    , _hasCustomBaseCol(false)
    , _showFocusRing(false)
{
    init();
}

APushButton::APushButton(const QString &text, QWidget *parent)
    : APushButton(parent)
{
    setText(text);
}

APushButton::APushButton(const QIcon &icon, const QString &text, QWidget *parent)
    : APushButton(parent)
{
    setIcon(icon);
    setText(text);
}

void APushButton::init()
{
    setMinimumHeight(40);
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
    setIconSize(QSize(18, 18));

    _anim = new QVariantAnimation(this);
    _anim->setDuration(140);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });

    connect(this, &QPushButton::toggled, this, [this]() {
        updateThemeColors();
        animateToCurrentState();
    });

    updateThemeColors();
    _currentCol = targetBackgroundColor();
}

APushButton::Variant APushButton::variant() const
{
    return _variant;
}

void APushButton::setVariant(Variant variant)
{
    if (_variant == variant) {
        return;
    }
    _variant = variant;
    updateThemeColors();
    _currentCol = targetBackgroundColor();
    update();
    emit variantChanged(_variant);
}

int APushButton::cornerRadius() const
{
    return _cornerRadius;
}

void APushButton::setCornerRadius(int radius)
{
    if (_cornerRadius == radius) {
        return;
    }
    _cornerRadius = radius;
    update();
    emit cornerRadiusChanged(_cornerRadius);
}

QColor APushButton::baseColor() const
{
    return _baseCol;
}

void APushButton::setBaseColor(const QColor &col)
{
    _hasCustomBaseCol = col.isValid();
    if (_hasCustomBaseCol) {
        _baseCol = col;
    }
    updateThemeColors();
    _currentCol = targetBackgroundColor();
    update();
}

void APushButton::resetBaseColor()
{
    _hasCustomBaseCol = false;
    updateThemeColors();
    _currentCol = targetBackgroundColor();
    update();
}

void APushButton::updateThemeColors()
{
    const QColor windowCol = palette().color(QPalette::Window);
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);
    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const bool isDark = windowCol.lightness() < 128;

    if (_anim) {
        _anim->setDuration(tokens.animationDuration);
    }

    const bool activeChecked = isCheckable() && isChecked() && _variant != Variant::Destructive;

    if (activeChecked) {
        _baseCol = _hasCustomBaseCol ? _baseCol : highlight;
        _hoverCol = blendColors(_baseCol, highlightedText, 0.12);
        _pressedCol = blendColors(_baseCol, highlightedText, 0.24);
        _borderCol = Qt::transparent;
        _textCol = highlightedText;
        return;
    }

    switch (_variant) {
    case Variant::Primary:
        if (!_hasCustomBaseCol) {
            _baseCol = highlight;
        }
        _hoverCol = blendColors(_baseCol, highlightedText, 0.12);
        _pressedCol = blendColors(_baseCol, highlightedText, 0.24);
        _borderCol = Qt::transparent;
        _textCol = highlightedText;
        break;

    case Variant::Secondary: {
        const QColor tint = blendColors(windowText, highlight, 0.65);
        _baseCol = withAlpha(tint, isDark ? 46 : 34);
        _hoverCol = withAlpha(tint, isDark ? 68 : 52);
        _pressedCol = withAlpha(tint, isDark ? 92 : 74);
        _borderCol = Qt::transparent;
        _textCol = windowText;
        break;
    }

    case Variant::Outlined:
        _baseCol = withAlpha(windowText, 0);
        _hoverCol = withAlpha(windowText, isDark ? 22 : 16);
        _pressedCol = withAlpha(windowText, isDark ? 38 : 30);
        _borderCol = tokens.outline;
        _textCol = windowText;
        break;

    case Variant::Flat:
        _baseCol = withAlpha(windowText, 0);
        _hoverCol = withAlpha(windowText, isDark ? 24 : 18);
        _pressedCol = withAlpha(windowText, isDark ? 42 : 34);
        _borderCol = Qt::transparent;
        _textCol = windowText;
        break;

    case Variant::Destructive: {
        _baseCol = _hasCustomBaseCol ? _baseCol : tokens.danger;
        _hoverCol = blendColors(_baseCol, QColor(Qt::white), 0.12);
        _pressedCol = blendColors(_baseCol, QColor(Qt::black), 0.18);
        _borderCol = Qt::transparent;
        _textCol = tokens.onSemantic;
        break;
    }
    }
}

QColor APushButton::targetBackgroundColor() const
{
    if (!isEnabled()) {
        if (_variant == Variant::Flat || _variant == Variant::Outlined) {
            return Qt::transparent;
        }
        return withAlpha(palette().color(QPalette::WindowText), 28);
    }

    if (isDown()) {
        return _pressedCol;
    }
    if (underMouse()) {
        return _hoverCol;
    }
    return _baseCol;
}

QColor APushButton::targetBorderColor() const
{
    if (_variant != Variant::Outlined || (isCheckable() && isChecked())) {
        return Qt::transparent;
    }

    const QColor windowText = palette().color(QPalette::WindowText);
    if (!isEnabled()) {
        return withAlpha(windowText, 30);
    }
    if (isDown()) {
        return palette().color(QPalette::Highlight);
    }
    if (underMouse()) {
        return withAlpha(windowText, 90);
    }
    return _borderCol;
}

QColor APushButton::targetTextColor() const
{
    if (!isEnabled()) {
        return withAlpha(palette().color(QPalette::WindowText), 97);
    }
    return _textCol;
}

void APushButton::animateToCurrentState()
{
    const QColor target = targetBackgroundColor();
    if (!_currentCol.isValid()) {
        _currentCol = target;
        update();
        return;
    }
    _anim->stop();
    _anim->setStartValue(_currentCol);
    _anim->setEndValue(target);
    _anim->start();
}

void APushButton::changeEvent(QEvent *event)
{
    QPushButton::changeEvent(event);

    switch (event->type()) {
    case QEvent::PaletteChange:
        updateThemeColors();
        _anim->stop();
        _currentCol = targetBackgroundColor();
        update();
        break;
    case QEvent::EnabledChange:
        setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
        _anim->stop();
        _currentCol = targetBackgroundColor();
        update();
        break;
    case QEvent::FontChange:
        updateGeometry();
        update();
        break;
    default:
        break;
    }
}

void APushButton::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);
    if (isEnabled()) {
        animateToCurrentState();
    }
}

void APushButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    if (isEnabled()) {
        animateToCurrentState();
    }
}

void APushButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    if (isEnabled()) {
        animateToCurrentState();
    }
}

void APushButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    if (isEnabled()) {
        animateToCurrentState();
    }
}

void APushButton::keyPressEvent(QKeyEvent *event)
{
    QPushButton::keyPressEvent(event);
    if (isEnabled() && (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        animateToCurrentState();
    }
}

void APushButton::keyReleaseEvent(QKeyEvent *event)
{
    QPushButton::keyReleaseEvent(event);
    if (isEnabled() && (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        animateToCurrentState();
    }
}

void APushButton::focusInEvent(QFocusEvent *event)
{
    QPushButton::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void APushButton::focusOutEvent(QFocusEvent *event)
{
    QPushButton::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

QSize APushButton::sizeHint() const
{
    QFont f = font();
    f.setWeight(QFont::Medium);
    const QFontMetrics fm(f);

    const QString btnText = text();
    const bool hasIcon = !icon().isNull();
    const bool hasText = !btnText.isEmpty();

    const int hPadding = (hasIcon && !hasText) ? 12 : 20;
    const int spacing = (hasIcon && hasText) ? 8 : 0;

    int contentWidth = 0;
    if (hasIcon) {
        contentWidth += iconSize().width();
    }
    if (hasText) {
        contentWidth += fm.horizontalAdvance(btnText);
    }

    const int w = qMax(minimumWidth(), contentWidth + spacing + hPadding * 2);
    const int contentHeight = qMax(hasIcon ? iconSize().height() : 0, fm.height());
    const int h = qMax(40, contentHeight + 16);
    return QSize(w, h);
}

QSize APushButton::minimumSizeHint() const
{
    return sizeHint();
}

void APushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal radius = (_cornerRadius >= 0)
                             ? qMin<qreal>(_cornerRadius, height() / 2.0)
                             : height() / 2.0;

    // 1. Fond du bouton
    const QColor borderCol = targetBorderColor();
    QRectF bgRect = rect();
    if (borderCol.alpha() > 0) {
        bgRect.adjust(0.5, 0.5, -0.5, -0.5);
        painter.setPen(QPen(borderCol, 1.0));
    } else {
        painter.setPen(Qt::NoPen);
    }
    painter.setBrush(_currentCol);
    painter.drawRoundedRect(bgRect, radius, radius);

    // 2. Anneau de focus clavier discret
    if (_showFocusRing && hasFocus() && isEnabled()) {
        QRectF focusRect = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
        const qreal focusRadius = qMax<qreal>(0.0, radius - 1.5);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(palette().color(QPalette::Highlight), 200), 2.0));
        painter.drawRoundedRect(focusRect, focusRadius, focusRadius);
    }

    // 3. Icône et texte centrés
    const QColor textColor = targetTextColor();
    painter.setPen(textColor);

    QFont f = font();
    f.setWeight(QFont::Medium);
    painter.setFont(f);

    const QIcon btnIcon = icon();
    const QString btnText = text();
    const QSize iconSizeVal = iconSize();
    const QFontMetrics fm = painter.fontMetrics();
    constexpr int spacing = 8;
    const int textWidth = btnText.isEmpty() ? 0 : fm.horizontalAdvance(btnText);

    int totalContentWidth = 0;
    if (!btnIcon.isNull() && !btnText.isEmpty()) {
        totalContentWidth = iconSizeVal.width() + spacing + textWidth;
    } else if (!btnIcon.isNull()) {
        totalContentWidth = iconSizeVal.width();
    } else if (!btnText.isEmpty()) {
        totalContentWidth = textWidth;
    }

    int xStart = (width() - totalContentWidth) / 2;

    if (!btnIcon.isNull()) {
        const int iconY = (height() - iconSizeVal.height()) / 2;
        const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        btnIcon.paint(&painter, xStart, iconY, iconSizeVal.width(), iconSizeVal.height(), Qt::AlignCenter, mode);
        xStart += iconSizeVal.width() + spacing;
    }

    if (!btnText.isEmpty()) {
        QRect textRect = rect();
        textRect.setLeft(xStart);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, btnText);
    }
}
