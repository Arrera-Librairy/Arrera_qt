#ifndef ASEGMENTEDBUTTON_H
#define ASEGMENTEDBUTTON_H

#include <QColor>
#include <QEvent>
#include <QFocusEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QList>
#include <QMouseEvent>
#include <QPainter>
#include <QVariant>
#include <QVariantAnimation>
#include <QWidget>

class ASegmentedButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    explicit ASegmentedButton(QWidget *parent = nullptr);

    int addSegment(const QString &text, const QVariant &data = QVariant());
    int addSegment(const QIcon &icon, const QString &text, const QVariant &data = QVariant());
    void clear();

    int count() const;
    int currentIndex() const;
    QVariant currentData() const;
    QString segmentText(int index) const;
    QVariant segmentData(int index) const;

    void setCurrentData(const QVariant &data);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void setCurrentIndex(int index);

signals:
    void currentIndexChanged(int index);
    void segmentClicked(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    struct Segment {
        QIcon icon;
        QString text;
        QVariant data;
    };

    int segmentAt(const QPoint &pos) const;
    QRectF segmentRect(int index) const;
    void updateIndicatorImmediate();
    void animateIndicatorTo(int index);

    QList<Segment> _segments;
    int _currentIndex;
    int _hoveredIndex;
    int _pressedIndex;
    bool _showFocusRing;

    qreal _indicatorX;
    qreal _indicatorW;
    qreal _startX;
    qreal _startW;
    qreal _targetX;
    qreal _targetW;

    QVariantAnimation *_anim;
};

#endif // ASEGMENTEDBUTTON_H
