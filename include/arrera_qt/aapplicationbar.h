#ifndef AAPPLICATIONBAR_H
#define AAPPLICATIONBAR_H

#include <QFrame>
#include <QVBoxLayout>
#include <QColor>
#include <QEvent>

class AApplicationBar : public QFrame
{
    Q_OBJECT
public:
    explicit AApplicationBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateThemeColors();

    QColor _baseCol;
};

#endif // AAPPLICATIONBAR_H
