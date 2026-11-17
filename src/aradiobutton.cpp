#include "arrera_qt/aradiobutton.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>

namespace {
constexpr int kOuterSize = 20;
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

ARadioButton::ARadioButton(QWidget *parent)
    : QRadioButton(parent)
    , _progress(0.0)
    , _hovered(false)
    , _showFocusRing(false)
{
    init();
}

ARadioButton::ARadioButton(const QString &text, QWidget *parent)
    : ARadioButton(parent)
{
    setText(text);
}

void ARadioButton::init()
{
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _progress = v.toReal();
        update();
    });

    connect(this, &QRadioButton::toggled, this, [this](bool checked) {
        animateProgress(checked ? 1.0 : 0.0);
    });
}

void ARadioButton::animateProgress(qreal target)
{
    _anim->stop();
    _anim->setStartValue(_progress);
    _anim->setEndValue(target);
    _anim->start();
}

bool ARadioButton::hitButton(const QPoint &pos) const
{
    return rect().contains(pos);
}

void ARadioButton::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QRadioButton::enterEvent(event);
}

void ARadioButton::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QRadioButton::leaveEvent(event);
}

void ARadioButton::focusInEvent(QFocusEvent *event)
{
    QRadioButton::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ARadioButton::focusOutEvent(QFocusEvent *event)
{
    QRadioButton::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ARadioButton::changeEvent(QEvent *event)
{
    QRadioButton::changeEvent(event);
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

QSize ARadioButton::sizeHint() const
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

QSize ARadioButton::minimumSizeHint() const
{
    return sizeHint();
}

void ARadioButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);

    const qreal cx = kHaloSize / 2.0;
    const qreal cy = height() / 2.0;
    const qreal outerRadius = kOuterSize / 2.0;

    // Halo au survol / clic
    if (isEnabled() && (_hovered || isDown())) {
        const QColor haloColor = isDown()
                                     ? withAlpha(highlight, 42)
                                     : withAlpha(windowText, 18);
        painter.setPen(Qt::NoPen);
        painter.setBrush(haloColor);
        painter.drawEllipse(QPointF(cx, cy), kHaloSize / 2.0, kHaloSize / 2.0);
    }

    // Anneau focus clavier
    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 195), 2.0));
        painter.drawEllipse(QPointF(cx, cy), outerRadius + 2.5, outerRadius + 2.5);
    }

    // Cercle extérieur
    QColor ringCol = blendColors(tokens.outline, highlight, _progress);
    if (!isEnabled()) {
        ringCol = withAlpha(windowText, 60);
    } else if (_hovered && _progress < 0.5) {
        ringCol = withAlpha(windowText, 165);
    }

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(ringCol, 2.0));
    painter.drawEllipse(QPointF(cx, cy), outerRadius - 1.0, outerRadius - 1.0);

    // Point central animé
    if (_progress > 0.05) {
        const qreal dotRadius = 5.0 * _progress;
        QColor dotCol = isEnabled() ? highlight : withAlpha(windowText, 75);
        painter.setPen(Qt::NoPen);
        painter.setBrush(dotCol);
        painter.drawEllipse(QPointF(cx, cy), dotRadius, dotRadius);
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
