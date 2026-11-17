#include "arrera_qt/aslider.h"
#include "arrera_qt/arrera_theme.h"

#include <QStyle>

namespace {
constexpr int kBreadth = 32;
constexpr qreal kTrackThickness = 8.0;
constexpr qreal kThumbPadding = 14.0;

QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

ASlider::ASlider(QWidget *parent)
    : QSlider(Qt::Horizontal, parent)
    , _hovered(false)
    , _dragging(false)
    , _showFocusRing(false)
{
    init();
}

ASlider::ASlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
    , _hovered(false)
    , _dragging(false)
    , _showFocusRing(false)
{
    init();
}

void ASlider::init()
{
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
    setMouseTracking(true);
}

qreal ASlider::normalizedValue() const
{
    const int span = maximum() - minimum();
    if (span <= 0) {
        return 0.0;
    }
    return static_cast<qreal>(value() - minimum()) / static_cast<qreal>(span);
}

int ASlider::valueFromMousePos(const QPoint &pos) const
{
    const int span = maximum() - minimum();
    if (span <= 0) {
        return minimum();
    }

    if (orientation() == Qt::Horizontal) {
        const qreal usable = qMax<qreal>(1.0, width() - kThumbPadding * 2.0);
        qreal ratio = (pos.x() - kThumbPadding) / usable;
        if (invertedAppearance()) {
            ratio = 1.0 - ratio;
        }
        return minimum() + qRound(qBound(0.0, ratio, 1.0) * span);
    }

    const qreal usable = qMax<qreal>(1.0, height() - kThumbPadding * 2.0);
    qreal ratio = 1.0 - (pos.y() - kThumbPadding) / usable;
    if (invertedAppearance()) {
        ratio = 1.0 - ratio;
    }
    return minimum() + qRound(qBound(0.0, ratio, 1.0) * span);
}

void ASlider::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        _dragging = true;
        setSliderDown(true);
        const int newVal = valueFromMousePos(event->pos());
        setValue(newVal);
        emit sliderMoved(newVal);
        update();
        event->accept();
        return;
    }
    QSlider::mousePressEvent(event);
}

void ASlider::mouseMoveEvent(QMouseEvent *event)
{
    if (isEnabled() && _dragging) {
        const int newVal = valueFromMousePos(event->pos());
        setValue(newVal);
        emit sliderMoved(newVal);
        event->accept();
        return;
    }
    QSlider::mouseMoveEvent(event);
}

void ASlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _dragging) {
        _dragging = false;
        setSliderDown(false);
        update();
        event->accept();
        return;
    }
    QSlider::mouseReleaseEvent(event);
}

void ASlider::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QSlider::enterEvent(event);
}

void ASlider::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QSlider::leaveEvent(event);
}

void ASlider::focusInEvent(QFocusEvent *event)
{
    QSlider::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ASlider::focusOutEvent(QFocusEvent *event)
{
    QSlider::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ASlider::changeEvent(QEvent *event)
{
    QSlider::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    } else if (event->type() == QEvent::PaletteChange) {
        update();
    }
}

QSize ASlider::sizeHint() const
{
    if (orientation() == Qt::Horizontal) {
        return QSize(180, kBreadth);
    }
    return QSize(kBreadth, 180);
}

QSize ASlider::minimumSizeHint() const
{
    if (orientation() == Qt::Horizontal) {
        return QSize(80, kBreadth);
    }
    return QSize(kBreadth, 80);
}

void ASlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const qreal ratio = invertedAppearance() ? (1.0 - normalizedValue()) : normalizedValue();
    const QColor trackInactive = isEnabled() ? tokens.surfaceContainer : withAlpha(windowText, 24);
    const QColor trackBorder = isEnabled() ? tokens.outlineVariant : withAlpha(windowText, 30);
    const QColor trackActive = isEnabled() ? highlight : withAlpha(windowText, 85);

    QPointF thumbCenter;

    if (orientation() == Qt::Horizontal) {
        const qreal cy = height() / 2.0;
        const qreal trackW = qMax<qreal>(1.0, width() - kThumbPadding * 2.0);
        const QRectF trackRect(kThumbPadding, cy - kTrackThickness / 2.0, trackW, kTrackThickness);

        painter.setBrush(trackInactive);
        painter.setPen(QPen(trackBorder, 1.0));
        painter.drawRoundedRect(trackRect, kTrackThickness / 2.0, kTrackThickness / 2.0);

        const qreal thumbX = kThumbPadding + trackW * ratio;
        thumbCenter = QPointF(thumbX, cy);

        const QRectF activeRect = invertedAppearance()
                                      ? QRectF(thumbX, trackRect.top(), trackRect.right() - thumbX, kTrackThickness)
                                      : QRectF(trackRect.left(), trackRect.top(), thumbX - trackRect.left(), kTrackThickness);
        if (activeRect.width() > 1.0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(trackActive);
            painter.drawRoundedRect(activeRect, kTrackThickness / 2.0, kTrackThickness / 2.0);
        }
    } else {
        const qreal cx = width() / 2.0;
        const qreal trackH = qMax<qreal>(1.0, height() - kThumbPadding * 2.0);
        const QRectF trackRect(cx - kTrackThickness / 2.0, kThumbPadding, kTrackThickness, trackH);

        painter.setBrush(trackInactive);
        painter.setPen(QPen(trackBorder, 1.0));
        painter.drawRoundedRect(trackRect, kTrackThickness / 2.0, kTrackThickness / 2.0);

        const qreal thumbY = kThumbPadding + trackH * (1.0 - ratio);
        thumbCenter = QPointF(cx, thumbY);

        const QRectF activeRect = invertedAppearance()
                                      ? QRectF(trackRect.left(), trackRect.top(), kTrackThickness, thumbY - trackRect.top())
                                      : QRectF(trackRect.left(), thumbY, kTrackThickness, trackRect.bottom() - thumbY);
        if (activeRect.height() > 1.0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(trackActive);
            painter.drawRoundedRect(activeRect, kTrackThickness / 2.0, kTrackThickness / 2.0);
        }
    }

    // Halo au survol / drag
    if (isEnabled() && (_hovered || _dragging)) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(withAlpha(highlight, _dragging ? 56 : 32));
        painter.drawEllipse(thumbCenter, 15.0, 15.0);
    }

    // Anneau focus clavier
    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 200), 2.0));
        painter.drawEllipse(thumbCenter, 12.5, 12.5);
    }

    // Curseur (thumb)
    const qreal thumbRadius = _dragging ? 10.5 : (_hovered ? 9.5 : 8.5);
    painter.setPen(QPen(highlightedText, 2.0));
    painter.setBrush(trackActive);
    painter.drawEllipse(thumbCenter, thumbRadius, thumbRadius);
}
