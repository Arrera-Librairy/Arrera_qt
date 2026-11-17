#include "arrera_qt/aframe.h"

AFrame::AFrame(QWidget *parent) : QFrame(parent)
{
    updateThemeColors();
}

void AFrame::updateThemeColors()
{
    QColor windowCol = palette().color(QPalette::Window);
    QColor baseCol = palette().color(QPalette::Base);

    // Utilise Base (surface surélevée façon carte GTK) si elle se détache de Window,
    // sinon bascule sur AlternateBase (ex. theme_default clair où Window == Base).
    _baseCol = (baseCol != windowCol) ? baseCol : palette().color(QPalette::AlternateBase);
}

void AFrame::changeEvent(QEvent *event)
{
    QFrame::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        updateThemeColors();
        update();
    }
}

void AFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(_baseCol);
    painter.setPen(Qt::NoPen);

    constexpr qreal radius = 16.0;
    painter.drawRoundedRect(rect(), radius, radius);
}
