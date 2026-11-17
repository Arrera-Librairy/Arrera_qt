#ifndef ASCROLLAREA_H
#define ASCROLLAREA_H

#include <QScrollArea>

class AScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit AScrollArea(QWidget *parent = nullptr);
};

#endif // ASCROLLAREA_H
