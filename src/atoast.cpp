#include "arrera_qt/atoast.h"
#include "arrera_qt/arrera_theme.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPushButton>

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}

class ToastCloseButton : public QAbstractButton
{
public:
    explicit ToastCloseButton(QWidget *parent = nullptr)
        : QAbstractButton(parent)
    {
        setFixedSize(24, 24);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::NoFocus);
        setToolTip(tr("Fermer"));
    }

protected:
    void enterEvent(QEnterEvent *event) override
    {
        _hovered = true;
        update();
        QAbstractButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        _hovered = false;
        update();
        QAbstractButton::leaveEvent(event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QColor textCol = palette().color(QPalette::WindowText);
        if (_hovered) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(withAlpha(textCol, 22));
            painter.drawEllipse(rect());
        }

        painter.setPen(QPen(withAlpha(textCol, _hovered ? 225 : 155), 1.6, Qt::SolidLine, Qt::RoundCap));
        const qreal m = 8.0;
        painter.drawLine(QPointF(m, m), QPointF(width() - m, height() - m));
        painter.drawLine(QPointF(width() - m, m), QPointF(m, height() - m));
    }

private:
    bool _hovered = false;
};

class ToastBadge : public QWidget
{
public:
    explicit ToastBadge(AToast *owner)
        : QWidget(owner)
        , _owner(owner)
    {
        setFixedSize(10, 10);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
        QColor dotCol;
        switch (_owner->toastType()) {
        case AToast::Type::Success:
            dotCol = tokens.success;
            break;
        case AToast::Type::Warning:
            dotCol = tokens.warning;
            break;
        case AToast::Type::Danger:
            dotCol = tokens.danger;
            break;
        case AToast::Type::Info:
        default:
            dotCol = palette().color(QPalette::Highlight);
            break;
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(dotCol);
        painter.drawEllipse(rect());
    }

private:
    AToast *_owner;
};
} // namespace

AToast::AToast(QWidget *parent)
    : QWidget(parent)
    , _type(Type::Info)
    , _durationMs(3500)
    , _dismissing(false)
{
    init();
}

AToast::AToast(const QString &message, Type type, QWidget *parent)
    : AToast(parent)
{
    setMessage(message);
    setToastType(type);
}

void AToast::init()
{
    setAttribute(Qt::WA_StyledBackground, false);

    _badgeWidget = new ToastBadge(this);

    _messageLabel = new QLabel(this);
    QFont f = _messageLabel->font();
    f.setWeight(QFont::Medium);
    _messageLabel->setFont(f);

    auto *actionBtn = new QPushButton(this);
    actionBtn->setFlat(true);
    actionBtn->setCursor(Qt::PointingHandCursor);
    actionBtn->setVisible(false);
    QFont af = actionBtn->font();
    af.setWeight(QFont::DemiBold);
    actionBtn->setFont(af);
    _actionBtn = actionBtn;

    _closeBtn = new ToastCloseButton(this);

    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(16, 10, 12, 10);
    _layout->setSpacing(10);
    _layout->addWidget(_badgeWidget);
    _layout->addWidget(_messageLabel, 1);
    _layout->addWidget(_actionBtn);
    _layout->addWidget(_closeBtn);
    setLayout(_layout);

    _opacityEffect = new QGraphicsOpacityEffect(this);
    _opacityEffect->setOpacity(0.0);
    setGraphicsEffect(_opacityEffect);

    _fadeAnim = new QPropertyAnimation(_opacityEffect, "opacity", this);
    _fadeAnim->setDuration(180);
    _fadeAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(_fadeAnim, &QPropertyAnimation::finished, this, [this]() {
        if (_dismissing) {
            hide();
            emit dismissed();
            if (testAttribute(Qt::WA_DeleteOnClose)) {
                deleteLater();
            }
        }
    });

    _autoHideTimer = new QTimer(this);
    _autoHideTimer->setSingleShot(true);
    connect(_autoHideTimer, &QTimer::timeout, this, &AToast::dismiss);

    connect(_closeBtn, &QAbstractButton::clicked, this, &AToast::dismiss);
    connect(_actionBtn, &QAbstractButton::clicked, this, [this]() {
        emit actionTriggered();
        dismiss();
    });

    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }

    updateThemeColors();
    hide();
}

AToast *AToast::showToast(QWidget *parent, const QString &message, Type type, int durationMs)
{
    QWidget *host = parent ? parent->window() : nullptr;
    auto *toast = new AToast(message, type, host);
    toast->setAttribute(Qt::WA_DeleteOnClose, true);
    toast->setDuration(durationMs);
    toast->showAnimated();
    return toast;
}

QString AToast::message() const
{
    return _messageLabel->text();
}

void AToast::setMessage(const QString &message)
{
    _messageLabel->setText(message);
    updateGeometryOnParent();
}

AToast::Type AToast::toastType() const
{
    return _type;
}

void AToast::setToastType(Type type)
{
    if (_type == type) {
        return;
    }
    _type = type;
    _badgeWidget->update();
    update();
}

void AToast::setAction(const QString &actionText)
{
    _actionBtn->setText(actionText);
    _actionBtn->setVisible(!actionText.isEmpty());
    updateGeometryOnParent();
}

void AToast::clearAction()
{
    _actionBtn->setText(QString());
    _actionBtn->setVisible(false);
    updateGeometryOnParent();
}

int AToast::duration() const
{
    return _durationMs;
}

void AToast::setDuration(int durationMs)
{
    _durationMs = qMax(0, durationMs);
}

QColor AToast::accentColor() const
{
    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    switch (_type) {
    case Type::Success:
        return tokens.success;
    case Type::Warning:
        return tokens.warning;
    case Type::Danger:
        return tokens.danger;
    case Type::Info:
    default:
        return palette().color(QPalette::Highlight);
    }
}

void AToast::updateThemeColors()
{
    const QPalette &appPal = QApplication::palette();
    const QColor windowText = appPal.color(QPalette::WindowText);
    const QColor highlight = appPal.color(QPalette::Highlight);

    QPalette msgPal = _messageLabel->palette();
    msgPal.setColor(QPalette::WindowText, windowText);
    _messageLabel->setPalette(msgPal);

    _actionBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background: transparent; border: none; color: %1; padding: 2px 6px; }"
        "QPushButton:hover { text-decoration: underline; }"
    ).arg(highlight.name()));

    _badgeWidget->update();
    _closeBtn->update();
}

void AToast::updateGeometryOnParent()
{
    QWidget *host = parentWidget();
    if (!host) {
        adjustSize();
        return;
    }

    adjustSize();
    const int maxW = qMax(220, host->width() - 48);
    const int w = qBound(220, sizeHint().width(), maxW);
    const int h = qMax(44, sizeHint().height());

    const int x = (host->width() - w) / 2;
    const int y = qMax(16, host->height() - h - 24);
    setGeometry(x, y, w, h);
}

void AToast::showAnimated()
{
    _dismissing = false;
    _autoHideTimer->stop();
    updateThemeColors();
    updateGeometryOnParent();

    show();
    raise();

    _fadeAnim->stop();
    _fadeAnim->setStartValue(_opacityEffect->opacity());
    _fadeAnim->setEndValue(1.0);
    _fadeAnim->start();

    if (_durationMs > 0) {
        _autoHideTimer->start(_durationMs);
    }
}

void AToast::dismiss()
{
    if (_dismissing || !isVisible()) {
        return;
    }
    _dismissing = true;
    _autoHideTimer->stop();

    _fadeAnim->stop();
    _fadeAnim->setStartValue(_opacityEffect->opacity());
    _fadeAnim->setEndValue(0.0);
    _fadeAnim->start();
}

bool AToast::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible()) {
        updateGeometryOnParent();
    }
    return QWidget::eventFilter(watched, event);
}

void AToast::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        updateThemeColors();
        update();
    }
}

void AToast::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const bool isDark = palette().color(QPalette::Window).lightness() < 128;

    QRectF cardRect = QRectF(rect()).adjusted(1.0, 0.5, -1.0, -2.5);
    const qreal radius = cardRect.height() / 2.0;

    // Ombre douce
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, isDark ? 80 : 24));
    painter.drawRoundedRect(cardRect.translated(0.0, 2.0), radius, radius);

    // Pilule flottante
    painter.setBrush(tokens.surfaceElevated);
    painter.setPen(QPen(tokens.outline, 1.0));
    painter.drawRoundedRect(cardRect, radius, radius);
}
