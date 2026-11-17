#include "arrera_qt/ascrollbar.h"

namespace {
constexpr int kBarBreadth = 10;

QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

AScrollBar::AScrollBar(QWidget *parent)
    : QScrollBar(Qt::Vertical, parent)
    , _hovered(false)
    , _dragging(false)
    , _dragStartValue(0)
{
    init();
}

AScrollBar::AScrollBar(Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent)
    , _hovered(false)
    , _dragging(false)
    , _dragStartValue(0)
{
    init();
}

void AScrollBar::init()
{
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

QSize AScrollBar::sizeHint() const
{
    if (orientation() == Qt::Vertical) {
        return QSize(kBarBreadth, 100);
    }
    return QSize(100, kBarBreadth);
}

QRectF AScrollBar::handleRect() const
{
    const int span = maximum() - minimum();
    if (span <= 0) {
        return QRectF();
    }

    const qreal thickness = (_hovered || _dragging) ? 6.0 : 4.0;
    const qreal margin = 2.0;

    if (orientation() == Qt::Vertical) {
        const qreal grooveLen = qMax<qreal>(1.0, height() - margin * 2.0);
        const qreal totalUnits = static_cast<qreal>(span + pageStep());
        const qreal handleLen = qBound(24.0, grooveLen * (static_cast<qreal>(pageStep()) / qMax<qreal>(1.0, totalUnits)), grooveLen);
        const qreal ratio = static_cast<qreal>(value() - minimum()) / static_cast<qreal>(span);
        const qreal y = margin + (grooveLen - handleLen) * ratio;
        const qreal x = width() - thickness - 2.0;
        return QRectF(x, y, thickness, handleLen);
    }

    const qreal grooveLen = qMax<qreal>(1.0, width() - margin * 2.0);
    const qreal totalUnits = static_cast<qreal>(span + pageStep());
    const qreal handleLen = qBound(24.0, grooveLen * (static_cast<qreal>(pageStep()) / qMax<qreal>(1.0, totalUnits)), grooveLen);
    const qreal ratio = static_cast<qreal>(value() - minimum()) / static_cast<qreal>(span);
    const qreal x = margin + (grooveLen - handleLen) * ratio;
    const qreal y = height() - thickness - 2.0;
    return QRectF(x, y, handleLen, thickness);
}

void AScrollBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QScrollBar::mousePressEvent(event);
        return;
    }

    const int span = maximum() - minimum();
    if (span <= 0) {
        return;
    }

    const QRectF hRect = handleRect();
    if (hRect.adjusted(-4.0, -4.0, 4.0, 4.0).contains(event->position())) {
        _dragging = true;
        _dragStartValue = value();
        _dragStartPos = event->pos();
        setSliderDown(true);
        update();
    } else {
        // Saut direct vers la position cliquée
        if (orientation() == Qt::Vertical) {
            const qreal grooveLen = qMax<qreal>(1.0, height() - hRect.height() - 4.0);
            const qreal ratio = qBound(0.0, (event->position().y() - 2.0 - hRect.height() / 2.0) / grooveLen, 1.0);
            setValue(minimum() + qRound(ratio * span));
        } else {
            const qreal grooveLen = qMax<qreal>(1.0, width() - hRect.width() - 4.0);
            const qreal ratio = qBound(0.0, (event->position().x() - 2.0 - hRect.width() / 2.0) / grooveLen, 1.0);
            setValue(minimum() + qRound(ratio * span));
        }
    }
    event->accept();
}

void AScrollBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!_dragging) {
        QScrollBar::mouseMoveEvent(event);
        return;
    }

    const int span = maximum() - minimum();
    if (span <= 0) {
        return;
    }

    const QRectF hRect = handleRect();
    if (orientation() == Qt::Vertical) {
        const qreal grooveLen = qMax<qreal>(1.0, height() - hRect.height() - 4.0);
        const int deltaPx = event->pos().y() - _dragStartPos.y();
        const int deltaVal = qRound((static_cast<qreal>(deltaPx) / grooveLen) * span);
        setValue(qBound(minimum(), _dragStartValue + deltaVal, maximum()));
    } else {
        const qreal grooveLen = qMax<qreal>(1.0, width() - hRect.width() - 4.0);
        const int deltaPx = event->pos().x() - _dragStartPos.x();
        const int deltaVal = qRound((static_cast<qreal>(deltaPx) / grooveLen) * span);
        setValue(qBound(minimum(), _dragStartValue + deltaVal, maximum()));
    }
    event->accept();
}

void AScrollBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (_dragging && event->button() == Qt::LeftButton) {
        _dragging = false;
        setSliderDown(false);
        update();
        event->accept();
        return;
    }
    QScrollBar::mouseReleaseEvent(event);
}

void AScrollBar::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QScrollBar::enterEvent(event);
}

void AScrollBar::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QScrollBar::leaveEvent(event);
}

void AScrollBar::changeEvent(QEvent *event)
{
    QScrollBar::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::EnabledChange) {
        update();
    }
}

void AScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    const QRectF hRect = handleRect();
    if (hRect.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QColor windowText = palette().color(QPalette::WindowText);
    const int alpha = _dragging ? 175 : (_hovered ? 128 : 72);

    painter.setPen(Qt::NoPen);
    painter.setBrush(withAlpha(windowText, alpha));
    const qreal radius = qMin(hRect.width(), hRect.height()) / 2.0;
    painter.drawRoundedRect(hRect, radius, radius);
}
