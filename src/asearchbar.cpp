#include "arrera_qt/asearchbar.h"

#include <QEnterEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>

namespace {
QColor blendColors(const QColor &base, const QColor &overlay, qreal ratio)
{
    const qreal clamped = qBound(0.0, ratio, 1.0);
    const qreal inv = 1.0 - clamped;
    return QColor::fromRgbF(
        base.redF() * inv + overlay.redF() * clamped,
        base.greenF() * inv + overlay.greenF() * clamped,
        base.blueF() * inv + overlay.blueF() * clamped,
        base.alphaF() * inv + overlay.alphaF() * clamped
    );
}

QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}

class SearchIconWidget : public QWidget
{
public:
    explicit SearchIconWidget(const QIcon *customIcon, QWidget *parent = nullptr)
        : QWidget(parent)
        , _customIcon(customIcon)
    {
        setFixedSize(20, 20);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if (_customIcon && !_customIcon->isNull()) {
            const QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
            _customIcon->paint(&painter, rect(), Qt::AlignCenter, mode);
            return;
        }

        const QColor textCol = palette().color(QPalette::WindowText);
        const QColor iconCol = withAlpha(textCol, isEnabled() ? 155 : 95);

        QPen pen(iconCol, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        // Loupe vectorielle centrée (20x20)
        painter.drawEllipse(QRectF(3.5, 3.5, 9.5, 9.5));
        painter.drawLine(QPointF(11.6, 11.6), QPointF(16.2, 16.2));
    }

private:
    const QIcon *_customIcon;
};

class ClearSearchButton : public QAbstractButton
{
public:
    explicit ClearSearchButton(QWidget *parent = nullptr)
        : QAbstractButton(parent)
    {
        setFixedSize(26, 26);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::NoFocus);
        setToolTip(tr("Effacer"));
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

        if (isDown()) {
            painter.setBrush(withAlpha(textCol, 42));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        } else if (_hovered) {
            painter.setBrush(withAlpha(textCol, 24));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        }

        const QColor iconCol = withAlpha(textCol, _hovered ? 220 : 155);
        painter.setPen(QPen(iconCol, 1.7, Qt::SolidLine, Qt::RoundCap));

        const qreal m = 8.5;
        painter.drawLine(QPointF(m, m), QPointF(width() - m, height() - m));
        painter.drawLine(QPointF(width() - m, m), QPointF(m, height() - m));
    }

private:
    bool _hovered = false;
};
} // namespace

ASearchBar::ASearchBar(QWidget *parent)
    : QWidget(parent)
    , _clearButtonEnabled(true)
    , _searchIconVisible(true)
    , _hasCustomBaseCol(false)
{
    setMinimumHeight(48);
    setCursor(Qt::IBeamCursor);
    setFocusPolicy(Qt::StrongFocus);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setFrame(false);
    _lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    _lineEdit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    setFocusProxy(_lineEdit);

    _searchIconWidget = new SearchIconWidget(&_customSearchIcon, this);
    _clearBtn = new ClearSearchButton(this);
    _clearBtn->setVisible(false);

    _layout = new QHBoxLayout(this);
    _layout->setSpacing(10);
    _layout->addWidget(_searchIconWidget);
    _layout->addWidget(_lineEdit, 1);
    _layout->addWidget(_clearBtn);
    setLayout(_layout);

    updateLayoutMargins();
    updateLineEditFont();
    updateThemeColors();
    _currentCol = targetBackgroundColor();

    _anim = new QVariantAnimation(this);
    _anim->setDuration(150);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });

    connect(_lineEdit, &QLineEdit::textChanged, this, [this](const QString &t) {
        updateClearButtonVisibility();
        emit textChanged(t);
    });
    connect(_lineEdit, &QLineEdit::textEdited, this, &ASearchBar::textEdited);
    connect(_lineEdit, &QLineEdit::returnPressed, this, [this]() {
        emit returnPressed();
        emit searchRequested(_lineEdit->text());
    });
    connect(_clearBtn, &QAbstractButton::clicked, this, &ASearchBar::clear);

    _lineEdit->installEventFilter(this);
}

QString ASearchBar::text() const
{
    return _lineEdit->text();
}

void ASearchBar::setText(const QString &text)
{
    _lineEdit->setText(text);
}

QString ASearchBar::placeholderText() const
{
    return _lineEdit->placeholderText();
}

void ASearchBar::setPlaceholderText(const QString &text)
{
    _lineEdit->setPlaceholderText(text);
}

bool ASearchBar::isClearButtonEnabled() const
{
    return _clearButtonEnabled;
}

void ASearchBar::setClearButtonEnabled(bool enable)
{
    if (_clearButtonEnabled == enable) {
        return;
    }
    _clearButtonEnabled = enable;
    updateClearButtonVisibility();
}

bool ASearchBar::isSearchIconVisible() const
{
    return _searchIconVisible;
}

void ASearchBar::setSearchIconVisible(bool visible)
{
    if (_searchIconVisible == visible) {
        return;
    }
    _searchIconVisible = visible;
    _searchIconWidget->setVisible(_searchIconVisible);
    updateLayoutMargins();
}

QIcon ASearchBar::searchIcon() const
{
    return _customSearchIcon;
}

void ASearchBar::setSearchIcon(const QIcon &icon)
{
    _customSearchIcon = icon;
    _searchIconWidget->update();
}

void ASearchBar::setBackgroundColor(const QColor &col)
{
    _hasCustomBaseCol = col.isValid();
    if (_hasCustomBaseCol) {
        _baseCol = col;
    }
    updateThemeColors();
    _currentCol = targetBackgroundColor();
    update();
}

void ASearchBar::resetBackgroundColor()
{
    _hasCustomBaseCol = false;
    updateThemeColors();
    _currentCol = targetBackgroundColor();
    update();
}

void ASearchBar::clear()
{
    if (_lineEdit->text().isEmpty()) {
        return;
    }
    _lineEdit->clear();
    _lineEdit->setFocus();
    emit cleared();
}

void ASearchBar::selectAll()
{
    _lineEdit->selectAll();
}

void ASearchBar::updateLayoutMargins()
{
    const int leftMargin = _searchIconVisible ? 16 : 20;
    const int rightMargin = _clearBtn->isVisible() ? 12 : 20;
    _layout->setContentsMargins(leftMargin, 0, rightMargin, 0);
}

void ASearchBar::updateClearButtonVisibility()
{
    const bool shouldShow = _clearButtonEnabled && !_lineEdit->text().isEmpty() && isEnabled();
    if (_clearBtn->isVisible() != shouldShow) {
        _clearBtn->setVisible(shouldShow);
        updateLayoutMargins();
    }
}

void ASearchBar::updateLineEditFont()
{
    QFont f = font();
    if (f.pointSizeF() > 0) {
        f.setPointSizeF(f.pointSizeF() + 1.0);
    } else if (f.pixelSize() > 0) {
        f.setPixelSize(f.pixelSize() + 1);
    }
    _lineEdit->setFont(f);
}

void ASearchBar::updateThemeColors()
{
    const QColor alternateBase = palette().color(QPalette::AlternateBase);
    const QColor base = palette().color(QPalette::Base);
    const QColor textCol = palette().color(QPalette::Text);
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    if (!_hasCustomBaseCol) {
        _baseCol = alternateBase;
    }

    _hoverCol = blendColors(_baseCol, windowText, 0.05);
    _focusCol = base;

    QPalette lePalette = _lineEdit->palette();
    lePalette.setColor(QPalette::Text, isEnabled() ? textCol : withAlpha(textCol, 100));
    lePalette.setColor(QPalette::PlaceholderText, withAlpha(textCol, isEnabled() ? 128 : 80));
    lePalette.setColor(QPalette::Highlight, highlight);
    lePalette.setColor(QPalette::HighlightedText, highlightedText);
    _lineEdit->setPalette(lePalette);

    _searchIconWidget->update();
    _clearBtn->update();
}

QColor ASearchBar::targetBackgroundColor() const
{
    if (!isEnabled()) {
        return withAlpha(palette().color(QPalette::WindowText), 20);
    }
    if (_lineEdit->hasFocus()) {
        return _focusCol;
    }
    if (underMouse()) {
        return _hoverCol;
    }
    return _baseCol;
}

void ASearchBar::animateToCurrentState()
{
    const QColor target = targetBackgroundColor();
    if (!_currentCol.isValid()) {
        _currentCol = target;
        update();
        return;
    }
    _anim->stop();
    _anim->setStartValue(_currentCol);
    _anim->setEndValue(target);
    _anim->start();
}

void ASearchBar::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
        updateThemeColors();
        _anim->stop();
        _currentCol = targetBackgroundColor();
        update();
        break;
    case QEvent::EnabledChange:
        setCursor(isEnabled() ? Qt::IBeamCursor : Qt::ArrowCursor);
        updateClearButtonVisibility();
        updateThemeColors();
        _anim->stop();
        _currentCol = targetBackgroundColor();
        update();
        break;
    case QEvent::FontChange:
        updateLineEditFont();
        updateGeometry();
        break;
    default:
        break;
    }
}

void ASearchBar::enterEvent(QEnterEvent *event)
{
    if (isEnabled() && !_lineEdit->hasFocus()) {
        animateToCurrentState();
    }
    QWidget::enterEvent(event);
}

void ASearchBar::leaveEvent(QEvent *event)
{
    if (isEnabled() && !_lineEdit->hasFocus()) {
        animateToCurrentState();
    }
    QWidget::leaveEvent(event);
}

void ASearchBar::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        _lineEdit->setFocus(Qt::MouseFocusReason);
    }
    QWidget::mousePressEvent(event);
}

bool ASearchBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _lineEdit) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            animateToCurrentState();
        }
    }
    return QWidget::eventFilter(watched, event);
}

QSize ASearchBar::sizeHint() const
{
    const QFontMetrics fm(_lineEdit->font());
    return QSize(260, qMax(48, fm.height() + 22));
}

QSize ASearchBar::minimumSizeHint() const
{
    return QSize(140, 48);
}

void ASearchBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(_currentCol);

    if (!isEnabled()) {
        painter.setPen(QPen(withAlpha(palette().color(QPalette::WindowText), 28), 1.0));
    } else if (_lineEdit->hasFocus()) {
        painter.setPen(QPen(palette().color(QPalette::Highlight), 2.0));
    } else {
        painter.setPen(QPen(withAlpha(palette().color(QPalette::WindowText), 32), 1.0));
    }

    // Pilule Material 3 / GTK4
    const QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    painter.drawRoundedRect(r, r.height() / 2.0, r.height() / 2.0);
}
