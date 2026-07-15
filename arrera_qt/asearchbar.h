#ifndef ASEARCHBAR_H
#define ASEARCHBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QVariantAnimation>
#include <QColor>
#include <QIcon>
#include <QHBoxLayout>

class ASearchBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
public:
    explicit ASearchBar(QWidget *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    QString placeholderText() const;
    void setPlaceholderText(const QString &text);

    QColor backgroundColor() const { return _baseCol; }
    void setBackgroundColor(const QColor &col) { _baseCol = col; _currentCol = col; update(); }

    QLineEdit* lineEdit() const { return _lineEdit; }

signals:
    void textChanged(const QString &text);
    void returnPressed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateThemeColors();

    QLineEdit *_lineEdit;

    QColor _baseCol;
    QColor _hoverCol;
    QColor _focusCol;
    QColor _currentCol;

    QVariantAnimation* _anim;
};

#endif // ASEARCHBAR_H
