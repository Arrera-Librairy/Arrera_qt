#ifndef ASETTINGROW_H
#define ASETTINGROW_H

#include <QColor>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QWidget>

class ALabel;

class ASettingRow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool activatable READ isActivatable WRITE setActivatable)
    Q_PROPERTY(bool showDivider READ showDivider WRITE setShowDivider)

public:
    explicit ASettingRow(QWidget *parent = nullptr);
    ASettingRow(const QString &title, const QString &subtitle = QString(), QWidget *parent = nullptr);

    QString title() const;
    void setTitle(const QString &title);

    QString subtitle() const;
    void setSubtitle(const QString &subtitle);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    bool isActivatable() const;
    void setActivatable(bool activatable);

    bool showDivider() const;
    void setShowDivider(bool show);

    void setWidget(QWidget *widget);
    QWidget *widget() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void activated();
    void titleChanged(const QString &title);
    void subtitleChanged(const QString &subtitle);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void init();
    void updateIconPixmap();

    QHBoxLayout *_mainLayout;
    QVBoxLayout *_textLayout;
    QLabel *_iconLabel;
    ALabel *_titleLabel;
    ALabel *_subtitleLabel;
    QWidget *_trailingWidget;

    QIcon _icon;
    bool _activatable;
    bool _showDivider;
    bool _hovered;
    bool _pressed;
};

#endif // ASETTINGROW_H
