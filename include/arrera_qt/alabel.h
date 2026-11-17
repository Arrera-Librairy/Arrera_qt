#ifndef ALABEL_H
#define ALABEL_H

#include <QEvent>
#include <QLabel>

class ALabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(Typography typography READ typography WRITE setTypography NOTIFY typographyChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
    enum class Typography {
        Title,
        Subtitle,
        Body,
        Caption
    };
    Q_ENUM(Typography)

    explicit ALabel(QWidget *parent = nullptr);
    explicit ALabel(const QString &text, QWidget *parent = nullptr);
    ALabel(const QString &text, Typography typography, QWidget *parent = nullptr);

    Typography typography() const;
    void setTypography(Typography typography);

    bool isMuted() const;
    void setMuted(bool muted);

signals:
    void typographyChanged(Typography typography);
    void mutedChanged(bool muted);

protected:
    void changeEvent(QEvent *event) override;

private:
    void applyTypography();
    void applyTextColor();

    Typography _typography;
    bool _muted;
    bool _hasExplicitMuted;
    qreal _basePointSize;
    int _basePixelSize;
};

#endif // ALABEL_H
