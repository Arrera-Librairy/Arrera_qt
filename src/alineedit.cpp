#include "arrera_qt/alineedit.h"
#include "arrera_qt/arrera_theme.h"

#include <QEnterEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

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

class InlineActionButton : public QAbstractButton
{
public:
    enum class Kind { Clear, PasswordEye };

    explicit InlineActionButton(Kind kind, QWidget *parent = nullptr)
        : QAbstractButton(parent)
        , _kind(kind)
    {
        setFixedSize(24, 24);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::NoFocus);
        if (kind == Kind::Clear) {
            setToolTip(tr("Effacer"));
        } else {
            setCheckable(true);
            setToolTip(tr("Afficher / masquer"));
        }
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
            painter.setBrush(withAlpha(textCol, 40));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        } else if (_hovered) {
            painter.setBrush(withAlpha(textCol, 22));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        }

        const QColor iconCol = withAlpha(textCol, _hovered ? 220 : 155);
        painter.setPen(QPen(iconCol, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);

        if (_kind == Kind::Clear) {
            const qreal m = 8.0;
            painter.drawLine(QPointF(m, m), QPointF(width() - m, height() - m));
            painter.drawLine(QPointF(width() - m, m), QPointF(m, height() - m));
        } else {
            // Icône œil vectorielle
            const qreal cx = width() / 2.0;
            const qreal cy = height() / 2.0;
            QPainterPath eye;
            eye.moveTo(cx - 7.5, cy);
            eye.quadTo(cx, cy - 5.5, cx + 7.5, cy);
            eye.quadTo(cx, cy + 5.5, cx - 7.5, cy);
            painter.drawPath(eye);

            painter.setBrush(iconCol);
            painter.drawEllipse(QPointF(cx, cy), 2.1, 2.1);

            if (isChecked()) {
                painter.setBrush(Qt::NoBrush);
                painter.drawLine(QPointF(cx - 6.5, cy + 5.5), QPointF(cx + 6.5, cy - 5.5));
            }
        }
    }

private:
    Kind _kind;
    bool _hovered = false;
};
} // namespace

ALineEdit::ALineEdit(QWidget *parent)
    : QWidget(parent)
    , _requestedEchoMode(QLineEdit::Normal)
    , _passwordVisible(false)
    , _clearButtonEnabled(false)
    , _passwordToggleEnabled(true)
    , _hasError(false)
    , _hasCustomRadius(false)
    , _cornerRadius(12)
{
    init();
}

ALineEdit::ALineEdit(const QString &placeholder, QWidget *parent)
    : ALineEdit(parent)
{
    setPlaceholderText(placeholder);
}

void ALineEdit::init()
{
    setMinimumHeight(42);
    setCursor(Qt::IBeamCursor);
    setFocusPolicy(Qt::StrongFocus);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setFrame(false);
    _lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    _lineEdit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    setFocusProxy(_lineEdit);

    _clearBtn = new InlineActionButton(InlineActionButton::Kind::Clear, this);
    _clearBtn->setVisible(false);

    _eyeBtn = new InlineActionButton(InlineActionButton::Kind::PasswordEye, this);
    _eyeBtn->setVisible(false);

    _layout = new QHBoxLayout(this);
    _layout->setSpacing(6);
    _layout->addWidget(_lineEdit, 1);
    _layout->addWidget(_clearBtn);
    _layout->addWidget(_eyeBtn);
    setLayout(_layout);

    _anim = new QVariantAnimation(this);
    _anim->setDuration(Arrera_Theme::currentTokens().animationDuration);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });

    connect(_lineEdit, &QLineEdit::textChanged, this, [this](const QString &t) {
        updateActionButtons();
        emit textChanged(t);
    });
    connect(_lineEdit, &QLineEdit::textEdited, this, &ALineEdit::textEdited);
    connect(_lineEdit, &QLineEdit::returnPressed, this, &ALineEdit::returnPressed);
    connect(_lineEdit, &QLineEdit::editingFinished, this, &ALineEdit::editingFinished);

    connect(_clearBtn, &QAbstractButton::clicked, this, &ALineEdit::clear);
    connect(_eyeBtn, &QAbstractButton::toggled, this, [this](bool visible) {
        _passwordVisible = visible;
        _lineEdit->setEchoMode(_passwordVisible ? QLineEdit::Normal : _requestedEchoMode);
    });

    _lineEdit->installEventFilter(this);

    updateLayoutMargins();
    updateThemeColors();
    _currentCol = targetBackgroundColor();
}

QString ALineEdit::text() const
{
    return _lineEdit->text();
}

void ALineEdit::setText(const QString &text)
{
    _lineEdit->setText(text);
}

QString ALineEdit::placeholderText() const
{
    return _lineEdit->placeholderText();
}

void ALineEdit::setPlaceholderText(const QString &text)
{
    _lineEdit->setPlaceholderText(text);
}

QLineEdit::EchoMode ALineEdit::echoMode() const
{
    return _requestedEchoMode;
}

void ALineEdit::setEchoMode(QLineEdit::EchoMode mode)
{
    _requestedEchoMode = mode;
    if (mode != QLineEdit::Password && mode != QLineEdit::PasswordEchoOnEdit) {
        _passwordVisible = false;
        _eyeBtn->setChecked(false);
    }
    _lineEdit->setEchoMode(_passwordVisible ? QLineEdit::Normal : _requestedEchoMode);
    updateActionButtons();
}

bool ALineEdit::isReadOnly() const
{
    return _lineEdit->isReadOnly();
}

void ALineEdit::setReadOnly(bool readOnly)
{
    _lineEdit->setReadOnly(readOnly);
    updateActionButtons();
}

bool ALineEdit::isClearButtonEnabled() const
{
    return _clearButtonEnabled;
}

void ALineEdit::setClearButtonEnabled(bool enable)
{
    if (_clearButtonEnabled == enable) {
        return;
    }
    _clearButtonEnabled = enable;
    updateActionButtons();
}

bool ALineEdit::isPasswordToggleEnabled() const
{
    return _passwordToggleEnabled;
}

void ALineEdit::setPasswordToggleEnabled(bool enable)
{
    if (_passwordToggleEnabled == enable) {
        return;
    }
    _passwordToggleEnabled = enable;
    if (!enable && _passwordVisible) {
        _passwordVisible = false;
        _eyeBtn->setChecked(false);
        _lineEdit->setEchoMode(_requestedEchoMode);
    }
    updateActionButtons();
}

bool ALineEdit::hasError() const
{
    return _hasError;
}

void ALineEdit::setError(bool error)
{
    if (_hasError == error) {
        return;
    }
    _hasError = error;
    update();
    emit errorChanged(_hasError);
}

int ALineEdit::cornerRadius() const
{
    return _cornerRadius;
}

void ALineEdit::setCornerRadius(int radius)
{
    _hasCustomRadius = true;
    const int clamped = qMax(0, radius);
    if (_cornerRadius == clamped) {
        return;
    }
    _cornerRadius = clamped;
    update();
}

void ALineEdit::clear()
{
    _lineEdit->clear();
    _lineEdit->setFocus();
}

void ALineEdit::selectAll()
{
    _lineEdit->selectAll();
}

void ALineEdit::updateActionButtons()
{
    const bool isPasswordMode = (_requestedEchoMode == QLineEdit::Password
                                 || _requestedEchoMode == QLineEdit::PasswordEchoOnEdit);
    const bool showEye = _passwordToggleEnabled && isPasswordMode && isEnabled();
    const bool showClear = _clearButtonEnabled && !_lineEdit->text().isEmpty()
                           && !isReadOnly() && isEnabled();

    _eyeBtn->setVisible(showEye);
    _clearBtn->setVisible(showClear);
    updateLayoutMargins();
}

void ALineEdit::updateLayoutMargins()
{
    const bool hasTrailing = _clearBtn->isVisible() || _eyeBtn->isVisible();
    _layout->setContentsMargins(14, 0, hasTrailing ? 10 : 14, 0);
}

void ALineEdit::updateThemeColors()
{
    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const QColor textCol = palette().color(QPalette::Text);
    const QColor windowText = palette().color(QPalette::WindowText);
    const QColor highlight = palette().color(QPalette::Highlight);
    const QColor highlightedText = palette().color(QPalette::HighlightedText);

    if (_anim) {
        _anim->setDuration(tokens.animationDuration);
    }
    if (!_hasCustomRadius) {
        _cornerRadius = tokens.radiusMedium;
    }

    _baseCol = tokens.surfaceContainer;
    _hoverCol = blendColors(_baseCol, windowText, 0.05);
    _focusCol = tokens.surfaceElevated;
    _borderCol = tokens.outline;

    QPalette lePalette = _lineEdit->palette();
    lePalette.setColor(QPalette::Text, isEnabled() ? textCol : withAlpha(textCol, 100));
    lePalette.setColor(QPalette::PlaceholderText, withAlpha(textCol, isEnabled() ? 128 : 80));
    lePalette.setColor(QPalette::Highlight, highlight);
    lePalette.setColor(QPalette::HighlightedText, highlightedText);
    _lineEdit->setPalette(lePalette);

    _clearBtn->update();
    _eyeBtn->update();
}

QColor ALineEdit::targetBackgroundColor() const
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

void ALineEdit::animateToCurrentState()
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

void ALineEdit::changeEvent(QEvent *event)
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
        updateActionButtons();
        updateThemeColors();
        _anim->stop();
        _currentCol = targetBackgroundColor();
        update();
        break;
    case QEvent::FontChange:
        _lineEdit->setFont(font());
        updateGeometry();
        break;
    default:
        break;
    }
}

void ALineEdit::enterEvent(QEnterEvent *event)
{
    if (isEnabled() && !_lineEdit->hasFocus()) {
        animateToCurrentState();
    }
    QWidget::enterEvent(event);
}

void ALineEdit::leaveEvent(QEvent *event)
{
    if (isEnabled() && !_lineEdit->hasFocus()) {
        animateToCurrentState();
    }
    QWidget::leaveEvent(event);
}

void ALineEdit::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        _lineEdit->setFocus(Qt::MouseFocusReason);
    }
    QWidget::mousePressEvent(event);
}

bool ALineEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _lineEdit) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            animateToCurrentState();
        }
    }
    return QWidget::eventFilter(watched, event);
}

QSize ALineEdit::sizeHint() const
{
    const QFontMetrics fm(_lineEdit->font());
    return QSize(220, qMax(42, fm.height() + 18));
}

QSize ALineEdit::minimumSizeHint() const
{
    return QSize(110, 42);
}

void ALineEdit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const ArreraThemeTokens tokens = Arrera_Theme::currentTokens();
    const qreal radius = qMin<qreal>(_cornerRadius, height() / 2.0);

    painter.setBrush(_currentCol);

    if (!isEnabled()) {
        painter.setPen(QPen(withAlpha(palette().color(QPalette::WindowText), 28), 1.0));
    } else if (_hasError) {
        painter.setPen(QPen(tokens.danger, 2.0));
    } else if (_lineEdit->hasFocus()) {
        painter.setPen(QPen(palette().color(QPalette::Highlight), 2.0));
    } else {
        painter.setPen(QPen(_borderCol, 1.0));
    }

    const QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    painter.drawRoundedRect(r, radius, radius);
}
