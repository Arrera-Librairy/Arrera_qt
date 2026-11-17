#ifndef ATOAST_H
#define ATOAST_H

#include <QAbstractButton>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

class AToast : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Type toastType READ toastType WRITE setToastType)

public:
    enum class Type {
        Info,
        Success,
        Warning,
        Danger
    };
    Q_ENUM(Type)

    explicit AToast(QWidget *parent = nullptr);
    explicit AToast(const QString &message, Type type = Type::Info, QWidget *parent = nullptr);

    static AToast *showToast(QWidget *parent,
                             const QString &message,
                             Type type = Type::Info,
                             int durationMs = 3500);

    QString message() const;
    void setMessage(const QString &message);

    Type toastType() const;
    void setToastType(Type type);

    void setAction(const QString &actionText);
    void clearAction();

    int duration() const;
    void setDuration(int durationMs);

    void showAnimated();
    void dismiss();

signals:
    void actionTriggered();
    void dismissed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void init();
    void updateGeometryOnParent();
    void updateThemeColors();
    QColor accentColor() const;

    Type _type;
    int _durationMs;
    bool _dismissing;

    QHBoxLayout *_layout;
    QWidget *_badgeWidget;
    QLabel *_messageLabel;
    QAbstractButton *_actionBtn;
    QAbstractButton *_closeBtn;

    QTimer *_autoHideTimer;
    QGraphicsOpacityEffect *_opacityEffect;
    QPropertyAnimation *_fadeAnim;
};

#endif // ATOAST_H
