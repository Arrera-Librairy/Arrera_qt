#include "aapplicationbar.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyle>

AApplicationBar::AApplicationBar(QWidget *parent)
    : QFrame(parent)
{
    // Taille typique d'une barre de navigation Material 3
    setMinimumWidth(80);

    // Configuration d'un layout vertical prêt à accueillir vos boutons
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 24, 12, 24); // Marges confortables "Expressive"
    layout->setSpacing(16); // Espacement entre les futurs éléments
    setLayout(layout);

    updateThemeColors();
}

void AApplicationBar::updateThemeColors()
{
    // Dans Material 3, la barre de navigation utilise souvent une couleur
    // de fond légèrement différente du fond principal (Surface Container).
    // On utilise AlternateBase qui est configuré dans Arrera_Theme pour ça.
    _baseCol = palette().color(QPalette::AlternateBase);
}

void AApplicationBar::changeEvent(QEvent *event)
{
    QFrame::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        updateThemeColors();
        update();
    }
}

void AApplicationBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(_baseCol);
    painter.setPen(Qt::NoPen); // Bordure invisible

    // Forme arrondie sur tous les coins, sans bordure visible, avec une couleur unie
    int radius = 16;
    painter.drawRoundedRect(rect(), radius, radius);
}
