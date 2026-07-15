#include "asearchbar.h"
#include <QPainter>
#include <QEvent>
#include <QEnterEvent>
#include <QStyle>

ASearchBar::ASearchBar(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(48);
    setCursor(Qt::IBeamCursor);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setFrame(false);
    _lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    _lineEdit->setStyleSheet("QLineEdit { background: transparent; border: none; }");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(24, 0, 24, 0); // 24px margins since there is no icon
    layout->setSpacing(0);
    layout->addWidget(_lineEdit);
    setLayout(layout);

    updateThemeColors();

    _anim = new QVariantAnimation(this);
    _anim->setDuration(150);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });

    connect(_lineEdit, &QLineEdit::textChanged, this, &ASearchBar::textChanged);
    connect(_lineEdit, &QLineEdit::returnPressed, this, &ASearchBar::returnPressed);

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

void ASearchBar::updateThemeColors()
{
    QColor alternateBase = palette().color(QPalette::AlternateBase);
    QColor base = palette().color(QPalette::Base);
    QColor textCol = palette().color(QPalette::Text);

    _baseCol = alternateBase;
    
    if (_baseCol.lightness() > 128) {
        _hoverCol = _baseCol.darker(105);
    } else {
        _hoverCol = _baseCol.lighter(115);
    }

    _focusCol = base;

    _currentCol = _lineEdit->hasFocus() ? _focusCol : (underMouse() ? _hoverCol : _baseCol);

    QPalette lePalette = _lineEdit->palette();
    lePalette.setColor(QPalette::Text, textCol);
    
    QColor placeholderCol = textCol;
    placeholderCol.setAlpha(128);
    lePalette.setColor(QPalette::PlaceholderText, placeholderCol);
    _lineEdit->setPalette(lePalette);
    
    QFont font = this->font();
    font.setPointSize(font.pointSize() + 1);
    _lineEdit->setFont(font);
}

void ASearchBar::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        updateThemeColors();
        update();
    }
}

void ASearchBar::enterEvent(QEnterEvent *event)
{
    if (!_lineEdit->hasFocus()) {
        _anim->setStartValue(_currentCol);
        _anim->setEndValue(_hoverCol);
        _anim->start();
    }
    QWidget::enterEvent(event);
}

void ASearchBar::leaveEvent(QEvent *event)
{
    if (!_lineEdit->hasFocus()) {
        _anim->setStartValue(_currentCol);
        _anim->setEndValue(_baseCol);
        _anim->start();
    }
    QWidget::leaveEvent(event);
}

bool ASearchBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _lineEdit) {
        if (event->type() == QEvent::FocusIn) {
            _anim->setStartValue(_currentCol);
            _anim->setEndValue(_focusCol);
            _anim->start();
        } else if (event->type() == QEvent::FocusOut) {
            _anim->setStartValue(_currentCol);
            _anim->setEndValue(underMouse() ? _hoverCol : _baseCol);
            _anim->start();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ASearchBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(_currentCol);
    
    if (_lineEdit->hasFocus()) {
        painter.setPen(QPen(palette().color(QPalette::Highlight), 2));
    } else {
        painter.setPen(Qt::NoPen);
    }

    // Material 3 highly rounded rectangle (pill shape)
    QRectF r = rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(r, r.height() / 2.0, r.height() / 2.0);
}
