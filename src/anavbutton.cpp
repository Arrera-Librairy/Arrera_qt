#include "arrera_qt/anavbutton.h"
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

ANavButton::ANavButton(QWidget *parent)
    : QAbstractButton(parent)
    , _layoutStyle(LayoutStyle::Horizontal)
    , _checkedProgress(0.0)
    , _hovered(false)
    , _showFocusRing(false)
{
    init();
}

ANavButton::ANavButton(const QString &text, QWidget *parent)
    : ANavButton(parent)
{
    setText(text);
}

ANavButton::ANavButton(const QIcon &icon, const QString &text, QWidget *parent)
    : ANavButton(parent)
{
    setIcon(icon);
    setText(text);
}

void ANavButton::init()
{
    setCheckable(true);
    setAutoExclusive(true);
    setFocusPolicy(Qt::StrongFocus);
    setIconSize(QSize(20, 20));
    setCursor(isEnabled() ? Qt::PointingHandCursor : Qt::ArrowCursor);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    _anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _checkedProgress = v.toReal();
        update();
    });

    connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        animateChecked(checked ? 1.0 : 0.0);
    });
}

ANavButton::LayoutStyle ANavButton::layoutStyle() const
{
    return _layoutStyle;
}

void ANavButton::setLayoutStyle(LayoutStyle style)
{
    if (_layoutStyle == style) {
        return;
    }
    _layoutStyle = style;
    updateGeometry();
    update();
    emit layoutStyleChanged(_layoutStyle);
}

void ANavButton::animateChecked(qreal target)
{
    _anim->stop();
    _anim->setStartValue(_checkedProgress);
    _anim->setEndValue(target);
    _anim->start();
}

void ANavButton::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ANavButton::leaveEvent(QEvent *event)
{
    _hovered = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void ANavButton::focusInEvent(QFocusEvent *event)
{
    QAbstractButton::focusInEvent(event);
    const Qt::FocusReason reason = event->reason();
    _showFocusRing = (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason);
    update();
}

void ANavButton::focusOutEvent(QFocusEvent *event)
{
    QAbstractButton::focusOutEvent(event);
    _showFocusRing = false;
    update();
}

void ANavButton::changeEvent(QEvent *event)
{
    QAbstractButton::changeEvent(event);
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

QSize ANavButton::sizeHint() const
{
    const QFontMetrics fm(font());
    const bool hasIcon = !icon().isNull();
    const bool hasText = !text().isEmpty();

    if (_layoutStyle == LayoutStyle::Vertical) {
        const int textW = hasText ? fm.horizontalAdvance(text()) : 0;
        const int w = qMax(64, textW + 16);
        int h = 12;
        if (hasIcon) {
            h += 32;
        }
        if (hasText) {
            h += (hasIcon ? 4 : 0) + fm.height();
        }
        return QSize(w, qMax(56, h));
    }

    int contentW = 0;
    if (hasIcon) {
        contentW += iconSize().width();
    }
    if (hasText) {
        contentW += (hasIcon ? 12 : 0) + fm.horizontalAdvance(text());
    }
    return QSize(qMax(64, contentW + 28), qMax(42, fm.height() + 18));
}

QSize ANavButton::minimumSizeHint() const
{
    return sizeHint();
}

void ANavButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    const QColor activePill = highlight;
    const QColor hoverPill = withAlpha(windowText, isDown() ? 34 : 20);
    const QIcon::Mode iconMode = isEnabled() ? QIcon::Normal : QIcon::Disabled;

    QFont f = font();
    f.setWeight(_checkedProgress > 0.5 ? QFont::DemiBold : QFont::Medium);
    painter.setFont(f);

    if (_layoutStyle == LayoutStyle::Vertical) {
        // Style Material 3 Navigation Rail : pilule 56x32 centrée autour de l'icône
        const qreal pillW = 56.0;
        const qreal pillH = 32.0;
        const qreal pillX = (width() - pillW) / 2.0;
        const qreal pillY = text().isEmpty() ? (height() - pillH) / 2.0 : 6.0;
        const QRectF pillRect(pillX, pillY, pillW, pillH);

        if (_checkedProgress > 0.01) {
            // Effet d'expansion subtil de la pilule active
            const qreal inset = (1.0 - _checkedProgress) * 10.0;
            const QRectF animPill = pillRect.adjusted(inset, 0.0, -inset, 0.0);
            QColor col = isEnabled() ? activePill : withAlpha(windowText, 50);
            col.setAlphaF(col.alphaF() * _checkedProgress);
            painter.setPen(Qt::NoPen);
            painter.setBrush(col);
            painter.drawRoundedRect(animPill, pillH / 2.0, pillH / 2.0);
        } else if ((_hovered || isDown()) && isEnabled()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(hoverPill);
            painter.drawRoundedRect(pillRect, pillH / 2.0, pillH / 2.0);
        }

        if (_showFocusRing && hasFocus() && isEnabled()) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(withAlpha(highlight, 195), 2.0));
            painter.drawRoundedRect(pillRect.adjusted(-2.0, -2.0, 2.0, 2.0), pillH / 2.0 + 2.0, pillH / 2.0 + 2.0);
        }

        if (!icon().isNull()) {
            const QSize iSize = iconSize();
            const int iconX = (width() - iSize.width()) / 2;
            const int iconY = static_cast<int>(pillY + (pillH - iSize.height()) / 2.0);
            icon().paint(&painter, iconX, iconY, iSize.width(), iSize.height(), Qt::AlignCenter, iconMode);
        }

        if (!text().isEmpty()) {
            const QColor textCol = isEnabled()
                                       ? blendColors(withAlpha(windowText, 195), windowText, _checkedProgress)
                                       : withAlpha(windowText, 97);
            painter.setPen(textCol);
            const QRect textRect(4, static_cast<int>(pillY + pillH + 4), width() - 8, height() - static_cast<int>(pillY + pillH + 4));
            painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, text());
        }
        return;
    }

    // Style Horizontal (GTK4 Sidebar / Navigation Drawer pill)
    const qreal radius = height() / 2.0;
    const QRectF bgRect = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);

    if (_checkedProgress > 0.01) {
        QColor bg = isEnabled() ? activePill : withAlpha(windowText, 50);
        if (_hovered && isEnabled()) {
            bg = blendColors(bg, highlightedText, 0.10);
        }
        bg.setAlphaF(bg.alphaF() * _checkedProgress);
        painter.setPen(Qt::NoPen);
        painter.setBrush(bg);
        painter.drawRoundedRect(bgRect, radius, radius);
    } else if ((_hovered || isDown()) && isEnabled()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(hoverPill);
        painter.drawRoundedRect(bgRect, radius, radius);
    }

    if (_showFocusRing && hasFocus() && isEnabled()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(withAlpha(highlight, 195), 2.0));
        painter.drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    }

    QColor textCol = blendColors(windowText, highlightedText, _checkedProgress);
    if (!isEnabled()) {
        textCol = withAlpha(windowText, 97);
    }
    painter.setPen(textCol);

    int x = 14;
    if (!icon().isNull()) {
        const QSize iSize = iconSize();
        const int iconY = (height() - iSize.height()) / 2;
        if (text().isEmpty()) {
            x = (width() - iSize.width()) / 2;
        }
        icon().paint(&painter, x, iconY, iSize.width(), iSize.height(), Qt::AlignCenter, iconMode);
        x += iSize.width() + 12;
    }

    if (!text().isEmpty()) {
        const QRect textRect(x, 0, qMax(0, width() - x - 14), height());
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}
