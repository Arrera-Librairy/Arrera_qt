#ifndef ASEARCHBAR_H
#define ASEARCHBAR_H

#include <QAbstractButton>
#include <QColor>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QWidget>

class ASearchBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool clearButtonEnabled READ isClearButtonEnabled WRITE setClearButtonEnabled)
    Q_PROPERTY(bool searchIconVisible READ isSearchIconVisible WRITE setSearchIconVisible)
    Q_PROPERTY(QIcon searchIcon READ searchIcon WRITE setSearchIcon)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit ASearchBar(QWidget *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    QString placeholderText() const;
    void setPlaceholderText(const QString &text);

    bool isClearButtonEnabled() const;
    void setClearButtonEnabled(bool enable);

    bool isSearchIconVisible() const;
    void setSearchIconVisible(bool visible);

    QIcon searchIcon() const;
    void setSearchIcon(const QIcon &icon);

    QColor backgroundColor() const { return _baseCol; }
    void setBackgroundColor(const QColor &col);
    void resetBackgroundColor();

    QLineEdit *lineEdit() const { return _lineEdit; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void clear();
    void selectAll();

signals:
    void textChanged(const QString &text);
    void textEdited(const QString &text);
    void returnPressed();
    void searchRequested(const QString &text);
    void cleared();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateThemeColors();
    void updateLineEditFont();
    void updateClearButtonVisibility();
    void updateLayoutMargins();
    void animateToCurrentState();
    QColor targetBackgroundColor() const;

    QLineEdit *_lineEdit;
    QHBoxLayout *_layout;
    QWidget *_searchIconWidget;
    QAbstractButton *_clearBtn;

    bool _clearButtonEnabled;
    bool _searchIconVisible;
    bool _hasCustomBaseCol;
    QIcon _customSearchIcon;

    QColor _baseCol;
    QColor _hoverCol;
    QColor _focusCol;
    QColor _borderCol;
    QColor _currentCol;

    QVariantAnimation *_anim;
};

#endif // ASEARCHBAR_H
