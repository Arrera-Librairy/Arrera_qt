#include "arrera_qt/asettingrow.h"
#include "arrera_qt/alabel.h"
#include "arrera_qt/arrera_theme.h"

#include <QAbstractButton>
#include <QPainter>

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

ASettingRow::ASettingRow(QWidget *parent)
    : QWidget(parent)
    , _trailingWidget(nullptr)
    , _activatable(false)
    , _showDivider(false)
    , _hovered(false)
    , _pressed(false)
{
    init();
}

ASettingRow::ASettingRow(const QString &title, const QString &subtitle, QWidget *parent)
    : ASettingRow(parent)
{
    setTitle(title);
    setSubtitle(subtitle);
}

void ASettingRow::init()
{
    setMinimumHeight(56);
    setMouseTracking(true);

    _mainLayout = new QHBoxLayout(this);
    _mainLayout->setContentsMargins(16, 10, 16, 10);
    _mainLayout->setSpacing(14);

    _iconLabel = new QLabel(this);
    _iconLabel->setFixedSize(22, 22);
    _iconLabel->setAlignment(Qt::AlignCenter);
    _iconLabel->setVisible(false);

    _titleLabel = new ALabel(this);
    _titleLabel->setTypography(ALabel::Typography::Body);
    QFont titleFont = _titleLabel->font();
    titleFont.setWeight(QFont::Medium);
    _titleLabel->setFont(titleFont);

    _subtitleLabel = new ALabel(this);
    _subtitleLabel->setTypography(ALabel::Typography::Caption);
    _subtitleLabel->setWordWrap(true);
    _subtitleLabel->setVisible(false);

    _textLayout = new QVBoxLayout();
    _textLayout->setContentsMargins(0, 0, 0, 0);
    _textLayout->setSpacing(2);
    _textLayout->addWidget(_titleLabel);
    _textLayout->addWidget(_subtitleLabel);

    _mainLayout->addWidget(_iconLabel);
    _mainLayout->addLayout(_textLayout, 1);
    setLayout(_mainLayout);
}

QString ASettingRow::title() const
{
    return _titleLabel->text();
}

void ASettingRow::setTitle(const QString &title)
{
    if (_titleLabel->text() == title) {
        return;
    }
    _titleLabel->setText(title);
    emit titleChanged(title);
}

QString ASettingRow::subtitle() const
{
    return _subtitleLabel->text();
}

void ASettingRow::setSubtitle(const QString &subtitle)
{
    if (_subtitleLabel->text() == subtitle) {
        return;
    }
    _subtitleLabel->setText(subtitle);
    _subtitleLabel->setVisible(!subtitle.isEmpty());
    updateGeometry();
    emit subtitleChanged(subtitle);
}

QIcon ASettingRow::icon() const
{
    return _icon;
}

void ASettingRow::setIcon(const QIcon &icon)
{
    _icon = icon;
    _iconLabel->setVisible(!_icon.isNull());
    updateIconPixmap();
}

void ASettingRow::updateIconPixmap()
{
    if (_icon.isNull()) {
        _iconLabel->clear();
        return;
    }
    const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    _iconLabel->setPixmap(_icon.pixmap(20, 20, mode));
}

bool ASettingRow::isActivatable() const
{
    return _activatable;
}

void ASettingRow::setActivatable(bool activatable)
{
    if (_activatable == activatable) {
        return;
    }
    _activatable = activatable;
    setCursor((_activatable && isEnabled()) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
}

bool ASettingRow::showDivider() const
{
    return _showDivider;
}

void ASettingRow::setShowDivider(bool show)
{
    if (_showDivider == show) {
        return;
    }
    _showDivider = show;
    update();
}

void ASettingRow::setWidget(QWidget *widget)
{
    if (_trailingWidget == widget) {
        return;
    }
    if (_trailingWidget) {
        _mainLayout->removeWidget(_trailingWidget);
        _trailingWidget->deleteLater();
        _trailingWidget = nullptr;
    }
    _trailingWidget = widget;
    if (_trailingWidget) {
        _trailingWidget->setParent(this);
        _mainLayout->addWidget(_trailingWidget, 0, Qt::AlignVCenter);
    }
    updateGeometry();
}

QWidget *ASettingRow::widget() const
{
    return _trailingWidget;
}

void ASettingRow::enterEvent(QEnterEvent *event)
{
    _hovered = true;
    if (_activatable) {
        update();
    }
    QWidget::enterEvent(event);
}

void ASettingRow::leaveEvent(QEvent *event)
{
    _hovered = false;
    _pressed = false;
    if (_activatable) {
        update();
    }
    QWidget::leaveEvent(event);
}

void ASettingRow::mousePressEvent(QMouseEvent *event)
{
    if (_activatable && isEnabled() && event->button() == Qt::LeftButton) {
        _pressed = true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void ASettingRow::mouseReleaseEvent(QMouseEvent *event)
{
    if (_activatable && isEnabled() && event->button() == Qt::LeftButton) {
        const bool inside = rect().contains(event->pos());
        _pressed = false;
        update();
        if (inside) {
            if (auto *btn = qobject_cast<QAbstractButton *>(_trailingWidget)) {
                if (btn->isEnabled() && btn->isCheckable()) {
                    btn->toggle();
                }
            }
            emit activated();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void ASettingRow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        setCursor((_activatable && isEnabled()) ? Qt::PointingHandCursor : Qt::ArrowCursor);
        updateIconPixmap();
        update();
    } else if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        updateIconPixmap();
        update();
    }
}

QSize ASettingRow::sizeHint() const
{
    const QSize base = QWidget::sizeHint();
    return QSize(qMax(240, base.width()), qMax(56, base.height()));
}

QSize ASettingRow::minimumSizeHint() const
{
    return QSize(180, 56);
}

void ASettingRow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor windowText = palette().color(QPalette::WindowText);

    if (_activatable && isEnabled() && (_hovered || _pressed)) {
        const int alpha = _pressed ? 26 : 14;
        painter.setPen(Qt::NoPen);
        painter.setBrush(withAlpha(windowText, alpha));
        const qreal radius = static_cast<qreal>(tokens.radiusMedium);
        painter.drawRoundedRect(QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0), radius, radius);
    }

    if (_showDivider) {
        painter.setPen(QPen(tokens.outlineVariant, 1.0));
        painter.drawLine(QPointF(16.0, height() - 0.5), QPointF(width() - 16.0, height() - 0.5));
    }
}
