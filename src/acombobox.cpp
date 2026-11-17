#include "arrera_qt/acombobox.h"
#include "arrera_qt/arrera_theme.h"

#include <QAbstractItemView>
#include <QFontMetrics>
#include <QPainterPath>

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

AComboBox::AComboBox(QWidget *parent)
    : QComboBox(parent)
    , _popupOpen(false)
    , _showFocusRing(false)
    , _hasCustomRadius(false)
    , _cornerRadius(12)
{
    setMinimumHeight(40);
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });

    updateThemeColors();
    _currentCol = targetBackgroundColor();
}

int AComboBox::cornerRadius() const
{
    return _cornerRadius;
}

void AComboBox::setCornerRadius(int radius)
{
    _hasCustomRadius = true;
    const int clamped = qMax(0, radius);
    if (_cornerRadius == clamped) {
        return;
    }
    _cornerRadius = clamped;
    update();
}

void AComboBox::updateThemeColors()
{
    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);

    if (_anim) {
        _anim->setDuration(tokens.animationDuration);
    }
    if (!_hasCustomRadius) {
        _cornerRadius = tokens.radiusMedium;
    }

    _baseCol = tokens.surfaceContainer;
    _hoverCol = blendColors(_baseCol, windowText, 0.06);
    _openCol = tokens.surfaceElevated;
    _borderCol = tokens.outline;

    updatePopupStyle();
}

void AComboBox::updatePopupStyle()
{
    if (!view()) {
        return;
    }

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor textCol = palette().color(QPalette::Text);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);
    const QColor hoverBg = withAlpha(palette().color(QPalette::WindowText), 20);

    view()->setPalette(palette());

    const QString qss = QStringLiteral(
        "QAbstractItemView {"
        "  background-color: %1;"
        "  color: %2;"
        "  border: 1px solid %3;"
        "  border-radius: %4px;"
        "  padding: 4px;"
        "  outline: 0;"
        "}"
        "QAbstractItemView::item {"
        "  min-height: 28px;"
        "  padding: 4px 10px;"
        "  border-radius: %5px;"
        "}"
        "QAbstractItemView::item:hover {"
        "  background-color: %6;"
        "}"
        "QAbstractItemView::item:selected {"
        "  background-color: %7;"
        "  color: %8;"
        "}"
    )
    .arg(tokens.surfaceElevated.name(QColor::HexArgb),
         textCol.name(QColor::HexArgb),
         tokens.outline.name(QColor::HexArgb))
    .arg(tokens.radiusMedium)
    .arg(tokens.radiusSmall)
    .arg(hoverBg.name(QColor::HexArgb),
         highlight.name(QColor::HexArgb),
         highlightedText.name(QColor::HexArgb));

    view()->setStyleSheet(qss);
}

QColor AComboBox::targetBackgroundColor() const
{
    if (!isEnabled()) {
        return withAlpha(palette().color(QPalette::WindowText), 20);
    }
    if (_popupOpen) {
        return _openCol;
    }
    if (underMouse()) {
        return _hoverCol;
    }
    return _baseCol;
}

void AComboBox::animateToCurrentState()
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

void AComboBox::showPopup()
{
    _popupOpen = true;
    updatePopupStyle();
    animateToCurrentState();
    QComboBox::showPopup();
}

void AComboBox::hidePopup()
{
    _popupOpen = false;
    animateToCurrentState();
    QComboBox::hidePopup();
}

void AComboBox::enterEvent(QEnterEvent *event)
{
    if (isEnabled()) {
        animateToCurrentState();
    }
    QComboBox::enterEvent(event);
}

void AComboBox::leaveEvent(QEvent *event)
{
    if (isEnabled()) {
        animateToCurrentState();
    }
    QComboBox::leaveEvent(event);
}

void AComboBox::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void AComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void AComboBox::changeEvent(QEvent *event)
{
    QComboBox::changeEvent(event);
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
        break;
    default:
        break;
    }
}

QSize AComboBox::sizeHint() const
{
    const QSize base = QComboBox::sizeHint();
    return QSize(qMax(120, base.width() + 18), qMax(40, base.height() + 10));
}

QSize AComboBox::minimumSizeHint() const
{
    return QSize(100, 40);
}

void AComboBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const qreal radius = qMin<qreal>(_cornerRadius, height() / 2.0);

    // 1. Fond & bordure
    painter.setBrush(_currentCol);
    if (!isEnabled()) {
        painter.setPen(QPen(withAlpha(windowText, 28), 1.0));
    } else if (_popupOpen || (_showFocusRing && hasFocus())) {
        painter.setPen(QPen(highlight, 2.0));
    } else {
        painter.setPen(QPen(_borderCol, 1.0));
    }

    const QRectF bgRect = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    painter.drawRoundedRect(bgRect, radius, radius);

    // 2. Icône + texte de l'élément actif
    int leftOffset = 14;
    const QIcon currentItemIcon = (currentIndex() >= 0) ? itemIcon(currentIndex()) : QIcon();
    if (!currentItemIcon.isNull()) {
        const QSize iSize = iconSize();
        const int iconY = (height() - iSize.height()) / 2;
        const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        currentItemIcon.paint(&painter, leftOffset, iconY, iSize.width(), iSize.height(), Qt::AlignCenter, mode);
        leftOffset += iSize.width() + 8;
    }

    const int chevronZoneWidth = 34;
    const QRect textRect(leftOffset, 0, qMax(0, width() - leftOffset - chevronZoneWidth), height());
    const QColor textCol = isEnabled() ? palette().color(QPalette::Text) : withAlpha(windowText, 97);
    painter.setPen(textCol);
    painter.setFont(font());

    const QFontMetrics fm(font());
    const QString elided = fm.elidedText(currentText(), Qt::ElideRight, textRect.width());
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

    // 3. Chevron vectoriel (bas / haut si menu ouvert)
    const QColor chevronCol = isEnabled() ? withAlpha(windowText, 180) : withAlpha(windowText, 85);
    painter.setPen(QPen(chevronCol, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    const qreal cx = width() - 18.0;
    const qreal cy = height() / 2.0;
    QPainterPath chevron;
    if (_popupOpen) {
        chevron.moveTo(cx - 4.5, cy + 2.0);
        chevron.lineTo(cx, cy - 2.5);
        chevron.lineTo(cx + 4.5, cy + 2.0);
    } else {
        chevron.moveTo(cx - 4.5, cy - 2.0);
        chevron.lineTo(cx, cy + 2.5);
        chevron.lineTo(cx + 4.5, cy - 2.0);
    }
    painter.drawPath(chevron);
}
