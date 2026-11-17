#include "arrera_qt/ascrollarea.h"
#include "arrera_qt/ascrollbar.h"

AScrollArea::AScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBar(new AScrollBar(Qt::Vertical, this));
    setHorizontalScrollBar(new AScrollBar(Qt::Horizontal, this));

    if (viewport()) {
        viewport()->setAutoFillBackground(false);
    }
}
