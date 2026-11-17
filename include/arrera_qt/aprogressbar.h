#ifndef APROGRESSBAR_H
#define APROGRESSBAR_H

#include <QEvent>
#include <QPainter>
#include <QProgressBar>
#include <QVariantAnimation>

class AProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit AProgressBar(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    bool isIndeterminate() const;
    void syncIndeterminateAnimation();
    qreal currentTargetRatio() const;

    qreal _animatedRatio;
    qreal _indeterminatePos;
    QVariantAnimation *_valueAnim;
    QVariantAnimation *_indeterminateAnim;
};

#endif // APROGRESSBAR_H
