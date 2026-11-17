#include "arrera_qt/aprogressbar.h"
#include "arrera_qt/arrera_theme.h"

#include <QFontMetrics>
#include <QHideEvent>
#include <QPainterPath>
#include <QShowEvent>

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

AProgressBar::AProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , _animatedRatio(0.0)
    , _indeterminatePos(0.0)
{
    setTextVisible(false);

    _valueAnim = new QVariantAnimation(this);
    _valueAnim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _valueAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_valueAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _animatedRatio = v.toReal();
        update();
    });

    _indeterminateAnim = new QVariantAnimation(this);
    _indeterminateAnim->setDuration(1400);
    _indeterminateAnim->setStartValue(0.0);
    _indeterminateAnim->setEndValue(1.0);
    _indeterminateAnim->setLoopCount(-1);
    _indeterminateAnim->setEasingCurve(QEasingCurve::InOutSine);
    connect(_indeterminateAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _indeterminatePos = v.toReal();
        update();
    });

    connect(this, &QProgressBar::valueChanged, this, [this]() {
        if (isIndeterminate()) {
            return;
        }
        const qreal target = currentTargetRatio();
        if (!isVisible()) {
            _animatedRatio = target;
            update();
            return;
        }
        _valueAnim->stop();
        _valueAnim->setStartValue(_animatedRatio);
        _valueAnim->setEndValue(target);
        _valueAnim->start();
    });
}

bool AProgressBar::isIndeterminate() const
{
    return minimum() == 0 && maximum() == 0;
}

qreal AProgressBar::currentTargetRatio() const
{
    const int span = maximum() - minimum();
    if (span <= 0) {
        return 0.0;
    }
    return qBound(0.0, static_cast<qreal>(value() - minimum()) / static_cast<qreal>(span), 1.0);
}

void AProgressBar::syncIndeterminateAnimation()
{
    const bool shouldRun = isIndeterminate() && isVisible() && isEnabled();
    if (shouldRun && _indeterminateAnim->state() != QAbstractAnimation::Running) {
        _indeterminateAnim->start();
    } else if (!shouldRun && _indeterminateAnim->state() == QAbstractAnimation::Running) {
        _indeterminateAnim->stop();
    }
}

void AProgressBar::showEvent(QShowEvent *event)
{
    QProgressBar::showEvent(event);
    if (!isIndeterminate()) {
        _animatedRatio = currentTargetRatio();
    }
    syncIndeterminateAnimation();
}

void AProgressBar::hideEvent(QHideEvent *event)
{
    QProgressBar::hideEvent(event);
    syncIndeterminateAnimation();
}

void AProgressBar::changeEvent(QEvent *event)
{
    QProgressBar::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        _valueAnim->setDuration(Arrera_Theme::currentTokens().animationDuration);
        update();
    } else if (event->type() == QEvent::EnabledChange) {
        syncIndeterminateAnimation();
        update();
    }
}

QSize AProgressBar::sizeHint() const
{
    if (isTextVisible()) {
        const QFontMetrics fm(font());
        return QSize(200, qMax(22, fm.height() + 8));
    }
    return QSize(200, 8);
}

QSize AProgressBar::minimumSizeHint() const
{
    return QSize(60, isTextVisible() ? 22 : 8);
}

void AProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    syncIndeterminateAnimation();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const qreal barHeight = isTextVisible() ? qMin<qreal>(height(), 24.0) : qMin<qreal>(height(), 10.0);
    const qreal barY = (height() - barHeight) / 2.0;
    const QRectF trackRect(0.5, barY + 0.5, width() - 1.0, barHeight - 1.0);
    const qreal radius = trackRect.height() / 2.0;

    // Piste de fond
    painter.setBrush(isEnabled() ? tokens.surfaceContainer : withAlpha(windowText, 24));
    painter.setPen(QPen(tokens.outlineVariant, 1.0));
    painter.drawRoundedRect(trackRect, radius, radius);

    const QColor activeCol = isEnabled() ? highlight : withAlpha(windowText, 80);

    if (isIndeterminate()) {
        // Barre coulissante en mode indéterminé
        const qreal segW = qMax<qreal>(28.0, trackRect.width() * 0.34);
        const qreal travel = trackRect.width() + segW;
        const qreal segX = trackRect.left() - segW + travel * _indeterminatePos;

        painter.save();
        QPainterPath clip;
        clip.addRoundedRect(trackRect, radius, radius);
        painter.setClipPath(clip);

        const QRectF indRect(segX, trackRect.top(), segW, trackRect.height());
        painter.setPen(Qt::NoPen);
        painter.setBrush(activeCol);
        painter.drawRoundedRect(indRect, radius, radius);
        painter.restore();
    } else {
        const qreal fillW = trackRect.width() * _animatedRatio;
        if (fillW > 1.0) {
            const qreal fillX = invertedAppearance() ? (trackRect.right() - fillW) : trackRect.left();
            const QRectF fillRect(fillX, trackRect.top(), fillW, trackRect.height());

            painter.save();
            QPainterPath clip;
            clip.addRoundedRect(trackRect, radius, radius);
            painter.setClipPath(clip);

            painter.setPen(Qt::NoPen);
            painter.setBrush(activeCol);
            painter.drawRoundedRect(fillRect, radius, radius);
            painter.restore();
        }
    }

    if (isTextVisible() && !isIndeterminate()) {
        QFont f = font();
        f.setWeight(QFont::DemiBold);
        painter.setFont(f);

        const QString txt = text();
        const QColor textCol = (_animatedRatio > 0.55) ? highlightedText : windowText;
        painter.setPen(isEnabled() ? textCol : withAlpha(windowText, 97));
        painter.drawText(trackRect, Qt::AlignCenter, txt);
    }
}
