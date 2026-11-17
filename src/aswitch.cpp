#include "arrera_qt/aswitch.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>

namespace {
constexpr int kTrackWidth = 50;
constexpr int kTrackHeight = 28;
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

ASwitch::ASwitch(QWidget *parent)
    : QAbstractButton(parent)
    , _progress(0.0)
    , _hovered(false)
    , _showFocusRing(false)
    , _hasCustomActiveCol(false)
{
    init();
}

ASwitch::ASwitch(const QString &text, QWidget *parent)
    : ASwitch(parent)
{
    setText(text);
}

void ASwitch::init()
{
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _progress = v.toReal();
        update();
    });

    connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        animateProgress(checked ? 1.0 : 0.0);
    });

    updateThemeColors();
}

QColor ASwitch::activeColor() const
{
    return _hasCustomActiveCol ? _customActiveCol : palette().color(QPalette::Highlight);
}

void ASwitch::setActiveColor(const QColor &col)
{
    _hasCustomActiveCol = col.isValid();
    if (_hasCustomActiveCol) {
        _customActiveCol = col;
    }
    update();
}

void ASwitch::resetActiveColor()
{
    _hasCustomActiveCol = false;
    update();
}

void ASwitch::updateThemeColors()
{
    if (_anim) {
        _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    }
}

void ASwitch::animateProgress(qreal target)
{
    _anim->stop();
    _anim->setStartValue(_progress);
    _anim->setEndValue(target);
    _anim->start();
}

void ASwitch::nextCheckState()
{
    setChecked(!isChecked());
}

void ASwitch::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ASwitch::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void ASwitch::focusInEvent(QFocusEvent *event)
{
    QAbstractButton::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ASwitch::focusOutEvent(QFocusEvent *event)
{
    QAbstractButton::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ASwitch::changeEvent(QEvent *event)
{
    QAbstractButton::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
        updateThemeColors();
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

QSize ASwitch::sizeHint() const
{
    int w = kTrackWidth + 4;
    int h = kTrackHeight + 4;

    if (!text().isEmpty()) {
        const QFontMetrics fm(font());
        w += kSpacing + fm.horizontalAdvance(text());
        h = qMax(h, fm.height() + 6);
    }

    return QSize(w, h);
}

QSize ASwitch::minimumSizeHint() const
{
    return sizeHint();
}

void ASwitch::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = activeColor();
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const qreal trackX = 2.0;
    const qreal trackY = (height() - kTrackHeight) / 2.0;
    const QRectF trackRect(trackX, trackY, kTrackWidth, kTrackHeight);
    const qreal trackRadius = kTrackHeight / 2.0;

    // Anneau focus clavier
    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 190), 2.0));
        painter.drawRoundedRect(trackRect.adjusted(-2.0, -2.0, 2.0, 2.0), trackRadius + 2.0, trackRadius + 2.0);
    }

    // Couleurs track & bordure
    QColor offTrack = tokens.surfaceContainer;
    if (_hovered && isEnabled()) {
        offTrack = blendColors(offTrack, windowText, 0.06);
    }
    QColor onTrack = highlight;
    if (_hovered && isEnabled()) {
        onTrack = blendColors(onTrack, highlightedText, 0.10);
    }

    QColor trackCol = blendColors(offTrack, onTrack, _progress);
    QColor borderCol = blendColors(tokens.outline, onTrack, _progress);

    if (!isEnabled()) {
        trackCol = withAlpha(windowText, 24);
        borderCol = withAlpha(windowText, 40);
    }

    painter.setBrush(trackCol);
    painter.setPen(QPen(borderCol, 1.5));
    painter.drawRoundedRect(trackRect.adjusted(0.75, 0.75, -0.75, -0.75), trackRadius, trackRadius);

    // Curseur (thumb) animé (taille M3 Expressive : 16px off -> 22px on, +2px au clic)
    qreal thumbDiameter = 16.0 + 6.0 * _progress;
    if (isDown() && isEnabled()) {
        thumbDiameter += 2.0;
    } else if (_hovered && isEnabled() && _progress < 0.5) {
        thumbDiameter += 1.5;
    }

    const qreal offCenterX = trackX + trackRadius;
    const qreal onCenterX = trackX + kTrackWidth - trackRadius;
    const qreal centerX = offCenterX + (onCenterX - offCenterX) * _progress;
    const qreal centerY = trackY + trackRadius;

    QColor offThumb = withAlpha(windowText, 165);
    QColor onThumb = highlightedText;
    QColor thumbCol = blendColors(offThumb, onThumb, _progress);
    if (!isEnabled()) {
        thumbCol = withAlpha(windowText, 95);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(thumbCol);
    painter.drawEllipse(QPointF(centerX, centerY), thumbDiameter / 2.0, thumbDiameter / 2.0);

    // Texte optionnel à droite
    if (!text().isEmpty()) {
        painter.setPen(isEnabled() ? windowText : withAlpha(windowText, 97));
        painter.setFont(font());
        const QRect textRect(static_cast<int>(trackX + kTrackWidth + kSpacing), 0,
                             width() - static_cast<int>(trackX + kTrackWidth + kSpacing), height());
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}
