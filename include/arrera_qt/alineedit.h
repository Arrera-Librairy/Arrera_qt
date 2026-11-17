#ifndef ALINEEDIT_H
#define ALINEEDIT_H

#include <QAbstractButton>
#include <QColor>
#include <QEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QWidget>

class ALineEdit : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(QLineEdit::EchoMode echoMode READ echoMode WRITE setEchoMode)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool clearButtonEnabled READ isClearButtonEnabled WRITE setClearButtonEnabled)
    Q_PROPERTY(bool passwordToggleEnabled READ isPasswordToggleEnabled WRITE setPasswordToggleEnabled)
    Q_PROPERTY(bool error READ hasError WRITE setError NOTIFY errorChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)

public:
    explicit ALineEdit(QWidget *parent = nullptr);
    explicit ALineEdit(const QString &placeholder, QWidget *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    QString placeholderText() const;
    void setPlaceholderText(const QString &text);

    QLineEdit::EchoMode echoMode() const;
    void setEchoMode(QLineEdit::EchoMode mode);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool isClearButtonEnabled() const;
    void setClearButtonEnabled(bool enable);

    bool isPasswordToggleEnabled() const;
    void setPasswordToggleEnabled(bool enable);

    bool hasError() const;
    void setError(bool error);

    int cornerRadius() const;
    void setCornerRadius(int radius);

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
    void editingFinished();
    void errorChanged(bool error);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void init();
    void updateThemeColors();
    void updateActionButtons();
    void updateLayoutMargins();
    void animateToCurrentState();
    QColor targetBackgroundColor() const;

    QLineEdit *_lineEdit;
    QHBoxLayout *_layout;
    QAbstractButton *_clearBtn;
    QAbstractButton *_eyeBtn;

    QLineEdit::EchoMode _requestedEchoMode;
    bool _passwordVisible;
    bool _clearButtonEnabled;
    bool _passwordToggleEnabled;
    bool _hasError;
    bool _hasCustomRadius;
    int _cornerRadius;

    QColor _baseCol;
    QColor _hoverCol;
    QColor _focusCol;
    QColor _borderCol;
    QColor _currentCol;

    QVariantAnimation *_anim;
};

#endif // ALINEEDIT_H
