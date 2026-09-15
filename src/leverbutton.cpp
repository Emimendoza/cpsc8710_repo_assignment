#include "leverbutton.h"

#include <QPainter>
#include <QPaintEvent>

LeverButton::LeverButton(QWidget *parent)
    : QPushButton(parent)
{
    setText(QStringLiteral("&Pull lever"));
    setAccessibleName(QStringLiteral("Pull lever"));
    setAccessibleDescription(QStringLiteral("Spin all three slot machine reels"));
    setToolTip(QStringLiteral("Pull lever (Alt+P or Space)"));
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
}

QSize LeverButton::sizeHint() const
{
    return {145, 285};
}

QSize LeverButton::minimumSizeHint() const
{
    return {125, 250};
}

void LeverButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const bool lowered = isDown();
    const bool inactive = !isEnabled();
    const QColor edge = inactive ? QColor("#50566a") : QColor("#d5ad3f");
    const QColor metal = inactive ? QColor("#686e7f") : QColor("#d7d9de");
    const QColor red = inactive ? QColor("#754655") : QColor("#e94463");

    const QRectF base(20, height() - 74, width() - 40, 48);
    p.setPen(QPen(edge, 3));
    p.setBrush(QColor("#252b3b"));
    p.drawRoundedRect(base, 13, 13);

    const qreal centerX = width() / 2.0;
    const qreal knobY = lowered ? height() - 128.0 : 43.0;
    const qreal shaftBottom = base.top() + 6;
    p.setPen(QPen(QColor("#2e3445"), 12, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(centerX + 3, knobY + 18), QPointF(centerX + 3, shaftBottom));
    p.setPen(QPen(metal, 7, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(centerX, knobY + 18), QPointF(centerX, shaftBottom));

    QRadialGradient knob(QPointF(centerX - 9, knobY - 9), 34);
    knob.setColorAt(0, inactive ? QColor("#a77885") : QColor("#ff8297"));
    knob.setColorAt(1, red.darker(135));
    p.setPen(QPen(red.darker(155), 4));
    p.setBrush(knob);
    p.drawEllipse(QPointF(centerX, knobY), 29, 29);

    if (hasFocus()) {
        p.setPen(QPen(QColor("#fff0a3"), 2, Qt::DashLine));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(rect()).adjusted(3, 3, -3, -3), 12, 12);
    }

    QFont f = font();
    f.setBold(true);
    f.setPixelSize(13);
    p.setFont(f);
    p.setPen(inactive ? QColor("#777d8d") : QColor("#f1d77b"));
    p.drawText(QRectF(0, height() - 24, width(), 20), Qt::AlignCenter,
               QStringLiteral("PULL"));
}
