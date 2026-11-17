#include "arrera_qt/alabel.h"

#include <QApplication>

ALabel::ALabel(QWidget *parent)
    : QLabel(parent)
    , _typography(Typography::Body)
    , _muted(false)
    , _hasExplicitMuted(false)
{
    const QFont base = QApplication::font();
    _basePointSize = base.pointSizeF();
    _basePixelSize = base.pixelSize();
    applyTypography();
    applyTextColor();
}

ALabel::ALabel(const QString &text, QWidget *parent)
    : ALabel(parent)
{
    setText(text);
}

ALabel::ALabel(const QString &text, Typography typography, QWidget *parent)
    : ALabel(parent)
{
    setText(text);
    setTypography(typography);
}

ALabel::Typography ALabel::typography() const
{
    return _typography;
}

void ALabel::setTypography(Typography typography)
{
    if (_typography == typography) {
        return;
    }
    _typography = typography;
    if (!_hasExplicitMuted) {
        _muted = (_typography == Typography::Caption);
    }
    applyTypography();
    applyTextColor();
    emit typographyChanged(_typography);
}

bool ALabel::isMuted() const
{
    return _muted;
}

void ALabel::setMuted(bool muted)
{
    _hasExplicitMuted = true;
    if (_muted == muted) {
        return;
    }
    _muted = muted;
    applyTextColor();
    emit mutedChanged(_muted);
}

void ALabel::applyTypography()
{
    QFont f = font();

    auto scaleFont = [&](qreal deltaPt, int minPt, QFont::Weight weight) {
        f.setWeight(weight);
        if (_basePointSize > 0.0) {
            f.setPointSizeF(qMax<qreal>(minPt, _basePointSize + deltaPt));
        } else if (_basePixelSize > 0) {
            f.setPixelSize(qMax(11, _basePixelSize + qRound(deltaPt * 1.33)));
        }
    };

    switch (_typography) {
    case Typography::Title:
        scaleFont(5.0, 16, QFont::Bold);
        break;
    case Typography::Subtitle:
        scaleFont(2.0, 13, QFont::DemiBold);
        break;
    case Typography::Body:
        scaleFont(0.0, 10, QFont::Normal);
        break;
    case Typography::Caption:
        scaleFont(-1.5, 10, QFont::Normal);
        break;
    }

    setFont(f);
    updateGeometry();
}

void ALabel::applyTextColor()
{
    QColor col = QApplication::palette().color(QPalette::WindowText);

    if (!isEnabled()) {
        col.setAlpha(97);
    } else if (_muted) {
        col.setAlpha(165);
    } else {
        col.setAlpha(255);
    }

    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::WindowText, col);
    setPalette(pal);
}

void ALabel::changeEvent(QEvent *event)
{
    QLabel::changeEvent(event);
    switch (event->type()) {
    case QEvent::ApplicationPaletteChange:
    case QEvent::PaletteChange:
    case QEvent::ParentChange:
    case QEvent::EnabledChange:
        applyTextColor();
        break;
    default:
        break;
    }
}
