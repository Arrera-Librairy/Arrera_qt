#include "arrera_qt/acheckbox.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>
#include <QPainterPath>

namespace {
constexpr int kBoxSize = 20;
constexpr int kHaloSize = 30;
constexpr int kSpacing = 10;

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

ACheckBox::ACheckBox(QWidget *parent)
    : QCheckBox(parent)
    , _progress(0.0)
    , _hovered(false)
    , _showFocusRing(false)
{
    init();
}

ACheckBox::ACheckBox(const QString &text, QWidget *parent)
    : ACheckBox(parent)
{
    setText(text);
}

void ACheckBox::init()
{
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _progress = v.toReal();
        update();
    });

    connect(this, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        animateProgress(state == Qt::Unchecked ? 0.0 : 1.0);
    });
}

void ACheckBox::animateProgress(qreal target)
{
    _anim->stop();
    _anim->setStartValue(_progress);
    _anim->setEndValue(target);
    _anim->start();
}

bool ACheckBox::hitButton(const QPoint &pos) const
{
    return rect().contains(pos);
}

void ACheckBox::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QCheckBox::enterEvent(event);
}

void ACheckBox::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QCheckBox::leaveEvent(event);
}

void ACheckBox::focusInEvent(QFocusEvent *event)
{
    QCheckBox::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ACheckBox::focusOutEvent(QFocusEvent *event)
{
    QCheckBox::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ACheckBox::changeEvent(QEvent *event)
{
    QCheckBox::changeEvent(event);
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

QSize ACheckBox::sizeHint() const
{
    int w = kHaloSize;
    int h = kHaloSize;

    if (!text().isEmpty()) {
        const QFontMetrics fm(font());
        w += kSpacing + fm.horizontalAdvance(text());
        h = qMax(h, fm.height() + 6);
    }

    return QSize(w, h);
}

QSize ACheckBox::minimumSizeHint() const
{
    return sizeHint();
}

void ACheckBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const qreal boxX = (kHaloSize - kBoxSize) / 2.0;
    const qreal boxY = (height() - kBoxSize) / 2.0;
    const QRectF boxRect(boxX, boxY, kBoxSize, kBoxSize);
    const qreal boxRadius = 6.0;

    // Halo au survol / clic
    if (isEnabled() && (_hovered || isDown())) {
        const QColor haloColor = isDown()
                                     ? withAlpha(highlight, 42)
                                     : withAlpha(windowText, 18);
        painter.setPen(Qt::NoPen);
        painter.setBrush(haloColor);
        const QRectF haloRect(0.0, (height() - kHaloSize) / 2.0, kHaloSize, kHaloSize);
        painter.drawEllipse(haloRect);
    }

    // Anneau focus clavier
    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 195), 2.0));
        painter.drawRoundedRect(boxRect.adjusted(-2.0, -2.0, 2.0, 2.0), boxRadius + 2.0, boxRadius + 2.0);
    }

    // Boîte
    QColor fillCol = blendColors(Qt::transparent, highlight, _progress);
    QColor borderCol = blendColors(tokens.outline, highlight, _progress);

    if (!isEnabled()) {
        fillCol = (_progress > 0.5) ? withAlpha(windowText, 70) : Qt::transparent;
        borderCol = withAlpha(windowText, 55);
    } else if (_hovered && _progress < 0.5) {
        borderCol = withAlpha(windowText, 165);
    }

    painter.setBrush(fillCol);
    painter.setPen(QPen(borderCol, 1.8));
    painter.drawRoundedRect(boxRect.adjusted(0.9, 0.9, -0.9, -0.9), boxRadius, boxRadius);

    // Coche ou tiret (PartiallyChecked)
    if (_progress > 0.05) {
        QColor checkCol = isEnabled() ? highlightedText : palette().color(QPalette::Window);
        checkCol.setAlphaF(checkCol.alphaF() * _progress);
        painter.setPen(QPen(checkCol, 2.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);

        if (checkState() == Qt::PartiallyChecked) {
            const qreal cy = boxY + kBoxSize / 2.0;
            painter.drawLine(QPointF(boxX + 5.0, cy), QPointF(boxX + kBoxSize - 5.0, cy));
        } else {
            QPainterPath tick;
            tick.moveTo(boxX + 4.8, boxY + 10.2);
            tick.lineTo(boxX + 8.5, boxY + 13.8);
            tick.lineTo(boxX + 15.2, boxY + 6.4);
            painter.drawPath(tick);
        }
    }

    // Texte
    if (!text().isEmpty()) {
        painter.setPen(isEnabled() ? windowText : withAlpha(windowText, 97));
        painter.setFont(font());
        const int textLeft = kHaloSize + kSpacing - 4;
        const QRect textRect(textLeft, 0, width() - textLeft, height());
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}
