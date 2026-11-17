#include "arrera_qt/asegmentedbutton.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

ASegmentedButton::ASegmentedButton(QWidget *parent)
    : QWidget(parent)
    , _currentIndex(-1)
    , _hoveredIndex(-1)
    , _pressedIndex(-1)
    , _showFocusRing(false)
    , _indicatorX(0.0)
    , _indicatorW(0.0)
    , _startX(0.0)
    , _startW(0.0)
    , _targetX(0.0)
    , _targetW(0.0)
{
    setMinimumHeight(40);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        const qreal t = v.toReal();
        _indicatorX = _startX + (_targetX - _startX) * t;
        _indicatorW = _startW + (_targetW - _startW) * t;
        update();
    });
}

int ASegmentedButton::addSegment(const QString &text, const QVariant &data)
{
    return addSegment(QIcon(), text, data);
}

int ASegmentedButton::addSegment(const QIcon &icon, const QString &text, const QVariant &data)
{
    _segments.append({icon, text, data});
    const int newIdx = _segments.size() - 1;
    if (_currentIndex < 0) {
        _currentIndex = 0;
    }
    updateGeometry();
    updateIndicatorImmediate();
    update();
    return newIdx;
}

void ASegmentedButton::clear()
{
    _segments.clear();
    _currentIndex = -1;
    _hoveredIndex = -1;
    _pressedIndex = -1;
    updateGeometry();
    update();
}

int ASegmentedButton::count() const
{
    return _segments.size();
}

int ASegmentedButton::currentIndex() const
{
    return _currentIndex;
}

QVariant ASegmentedButton::currentData() const
{
    if (_currentIndex < 0 || _currentIndex >= _segments.size()) {
        return QVariant();
    }
    return _segments.at(_currentIndex).data;
}

QString ASegmentedButton::segmentText(int index) const
{
    if (index < 0 || index >= _segments.size()) {
        return QString();
    }
    return _segments.at(index).text;
}

QVariant ASegmentedButton::segmentData(int index) const
{
    if (index < 0 || index >= _segments.size()) {
        return QVariant();
    }
    return _segments.at(index).data;
}

void ASegmentedButton::setCurrentData(const QVariant &data)
{
    for (int i = 0; i < _segments.size(); ++i) {
        if (_segments.at(i).data == data) {
            setCurrentIndex(i);
            return;
        }
    }
}

void ASegmentedButton::setCurrentIndex(int index)
{
    if (index < 0 || index >= _segments.size() || index == _currentIndex) {
        return;
    }
    _currentIndex = index;
    animateIndicatorTo(index);
    emit currentIndexChanged(_currentIndex);
}

QRectF ASegmentedButton::segmentRect(int index) const
{
    if (_segments.isEmpty() || index < 0 || index >= _segments.size()) {
        return QRectF();
    }
    const qreal padding = 3.0;
    const qreal innerW = qMax<qreal>(0.0, width() - padding * 2.0);
    const qreal segW = innerW / _segments.size();
    return QRectF(padding + index * segW, padding, segW, height() - padding * 2.0);
}

int ASegmentedButton::segmentAt(const QPoint &pos) const
{
    if (_segments.isEmpty() || !rect().contains(pos)) {
        return -1;
    }
    const qreal segW = qMax<qreal>(1.0, (width() - 6.0) / _segments.size());
    const int idx = static_cast<int>((pos.x() - 3.0) / segW);
    return qBound(0, idx, static_cast<int>(_segments.size()) - 1);
}

void ASegmentedButton::updateIndicatorImmediate()
{
    const QRectF r = segmentRect(_currentIndex);
    _indicatorX = r.x();
    _indicatorW = r.width();
}

void ASegmentedButton::animateIndicatorTo(int index)
{
    const QRectF target = segmentRect(index);
    if (!isVisible() || _indicatorW <= 0.0) {
        _indicatorX = target.x();
        _indicatorW = target.width();
        update();
        return;
    }
    _anim->stop();
    _startX = _indicatorX;
    _startW = _indicatorW;
    _targetX = target.x();
    _targetW = target.width();
    _anim->setStartValue(0.0);
    _anim->setEndValue(1.0);
    _anim->start();
}

void ASegmentedButton::mouseMoveEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        const int idx = segmentAt(event->pos());
        if (idx != _hoveredIndex) {
            _hoveredIndex = idx;
            update();
        }
    }
    QWidget::mouseMoveEvent(event);
}

void ASegmentedButton::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        _pressedIndex = segmentAt(event->pos());
        update();
    }
    QWidget::mousePressEvent(event);
}

void ASegmentedButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        const int releaseIdx = segmentAt(event->pos());
        if (releaseIdx >= 0 && releaseIdx == _pressedIndex) {
            setCurrentIndex(releaseIdx);
            emit segmentClicked(releaseIdx);
        }
        _pressedIndex = -1;
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void ASegmentedButton::leaveEvent(QEvent *event)
{
    _hoveredIndex = -1;
    _pressedIndex = -1;
    update();
    QWidget::leaveEvent(event);
}

void ASegmentedButton::keyPressEvent(QKeyEvent *event)
{
    if (!isEnabled() || _segments.isEmpty()) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Left) {
        setCurrentIndex(qMax(0, _currentIndex - 1));
        return;
    }
    if (event->key() == Qt::Key_Right) {
        setCurrentIndex(qMin(static_cast<int>(_segments.size()) - 1, _currentIndex + 1));
        return;
    }

    QWidget::keyPressEvent(event);
}

void ASegmentedButton::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ASegmentedButton::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ASegmentedButton::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (_anim->state() != QAbstractAnimation::Running) {
        updateIndicatorImmediate();
    }
}

void ASegmentedButton::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
        if (_anim) {
            _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
        }
        update();
        break;
    case QEvent::EnabledChange:
        setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
        break;
    case QEvent::FontChange:
        updateGeometry();
        break;
    default:
        break;
    }
}

QSize ASegmentedButton::sizeHint() const
{
    if (_segments.isEmpty()) {
        return QSize(160, 40);
    }

    const QFontMetrics fm(font());
    int maxSegW = 56;
    for (const Segment &seg : _segments) {
        int w = 24;
        if (!seg.icon.isNull()) {
            w += 16 + (seg.text.isEmpty() ? 0 : 6);
        }
        if (!seg.text.isEmpty()) {
            w += fm.horizontalAdvance(seg.text);
        }
        maxSegW = qMax(maxSegW, w);
    }

    return QSize(maxSegW * _segments.size() + 6, qMax(40, fm.height() + 18));
}

QSize ASegmentedButton::minimumSizeHint() const
{
    return QSize(qMax(100, static_cast<int>(_segments.size()) * 48), 40);
}

void ASegmentedButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const QRectF outerRect = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal outerRadius = outerRect.height() / 2.0;

    // 1. Fond conteneur pilule
    painter.setBrush(isEnabled() ? tokens.surfaceContainer : withAlpha(windowText, 20));
    painter.setPen(QPen(isEnabled() ? tokens.outlineVariant : withAlpha(windowText, 28), 1.0));
    painter.drawRoundedRect(outerRect, outerRadius, outerRadius);

    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 195), 2.0));
        painter.drawRoundedRect(outerRect.adjusted(0.5, 0.5, -0.5, -0.5), outerRadius, outerRadius);
    }

    if (_segments.isEmpty()) {
        return;
    }

    // 2. Séparateurs verticaux discrets entre segments non sélectionnés
    painter.setPen(QPen(tokens.outlineVariant, 1.0));
    for (int i = 1; i < _segments.size(); ++i) {
        if (i == _currentIndex || i - 1 == _currentIndex) {
            continue;
        }
        const QRectF r = segmentRect(i);
        painter.drawLine(QPointF(r.left(), r.top() + 8.0), QPointF(r.left(), r.bottom() - 8.0));
    }

    // 3. Survol d'un segment inactif
    if (_hoveredIndex >= 0 && _hoveredIndex != _currentIndex && isEnabled()) {
        const QRectF hr = segmentRect(_hoveredIndex);
        painter.setPen(Qt::NoPen);
        painter.setBrush(withAlpha(windowText, _pressedIndex == _hoveredIndex ? 32 : 18));
        painter.drawRoundedRect(hr, hr.height() / 2.0, hr.height() / 2.0);
    }

    // 4. Pilule active coulissante
    if (_currentIndex >= 0 && _indicatorW > 0.0) {
        const QRectF indRect(_indicatorX, 3.0, _indicatorW, height() - 6.0);
        painter.setPen(Qt::NoPen);
        painter.setBrush(isEnabled() ? highlight : withAlpha(windowText, 60));
        painter.drawRoundedRect(indRect, indRect.height() / 2.0, indRect.height() / 2.0);
    }

    // 5. Icônes et textes
    QFont f = font();
    f.setWeight(QFont::Medium);
    painter.setFont(f);
    const QFontMetrics fm(f);

    for (int i = 0; i < _segments.size(); ++i) {
        const Segment &seg = _segments.at(i);
        const QRectF r = segmentRect(i);
        const bool isActive = (i == _currentIndex);

        QColor textCol = isActive ? highlightedText : windowText;
        if (!isEnabled()) {
            textCol = withAlpha(windowText, 97);
        }
        painter.setPen(textCol);

        const int iconSizePx = 16;
        const int spacing = (!seg.icon.isNull() && !seg.text.isEmpty()) ? 6 : 0;
        const int textW = seg.text.isEmpty() ? 0 : fm.horizontalAdvance(seg.text);
        const int totalW = (!seg.icon.isNull() ? iconSizePx : 0) + spacing + textW;

        qreal x = r.center().x() - totalW / 2.0;
        if (!seg.icon.isNull()) {
            const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
            seg.icon.paint(&painter, static_cast<int>(x), static_cast<int>(r.center().y() - iconSizePx / 2.0),
                           iconSizePx, iconSizePx, Qt::AlignCenter, mode);
            x += iconSizePx + spacing;
        }
        if (!seg.text.isEmpty()) {
            const QRectF tr(x, r.top(), textW + 4, r.height());
            painter.drawText(tr, Qt::AlignVCenter | Qt::AlignLeft, seg.text);
        }
    }
}
