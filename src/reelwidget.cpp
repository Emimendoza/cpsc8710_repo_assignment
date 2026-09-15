#include "reelwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

namespace {

void drawSkull(QPainter &p, const QRectF &r)
{
    p.setPen(QPen(QColor("#263044"), 4));
    p.setBrush(QColor("#f2eadc"));
    const QRectF head(r.left() + r.width() * .19, r.top() + r.height() * .10,
                      r.width() * .62, r.height() * .58);
    p.drawEllipse(head);
    p.drawRoundedRect(QRectF(r.left() + r.width() * .31, r.top() + r.height() * .55,
                             r.width() * .38, r.height() * .25), 6, 6);
    p.setBrush(QColor("#263044"));
    p.drawEllipse(QRectF(r.left() + r.width() * .31, r.top() + r.height() * .31, 15, 18));
    p.drawEllipse(QRectF(r.right() - r.width() * .31 - 15, r.top() + r.height() * .31, 15, 18));
    QPolygonF nose;
    nose << QPointF(r.center().x(), r.top() + r.height() * .48)
         << QPointF(r.center().x() - 6, r.top() + r.height() * .57)
         << QPointF(r.center().x() + 6, r.top() + r.height() * .57);
    p.drawPolygon(nose);
}

void drawBar(QPainter &p, const QRectF &r)
{
    const QRectF bar(r.left() + r.width() * .08, r.top() + r.height() * .29,
                     r.width() * .84, r.height() * .41);
    p.setPen(QPen(QColor("#f6c84c"), 5));
    p.setBrush(QColor("#171c2b"));
    p.drawRoundedRect(bar, 12, 12);
    QFont font = p.font();
    font.setBold(true);
    font.setPixelSize(static_cast<int>(r.height() * .30));
    font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    p.setFont(font);
    p.setPen(QColor("#fff4c0"));
    p.drawText(bar, Qt::AlignCenter, QStringLiteral("BAR"));
}

void drawSeven(QPainter &p, const QRectF &r)
{
    QFont font(QStringLiteral("DejaVu Sans"));
    font.setBold(true);
    font.setItalic(true);
    font.setPixelSize(static_cast<int>(r.height() * .82));
    p.setFont(font);
    p.setPen(QPen(QColor("#8e1831"), 7, Qt::SolidLine, Qt::RoundCap));
    p.drawText(r.translated(3, 5), Qt::AlignCenter, QStringLiteral("7"));
    p.setPen(QColor("#ff496c"));
    p.drawText(r, Qt::AlignCenter, QStringLiteral("7"));
}

void drawCherry(QPainter &p, const QRectF &r)
{
    p.setPen(QPen(QColor("#477c3c"), 6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(r.center().x(), r.top() + 14),
               QPointF(r.left() + r.width() * .34, r.top() + r.height() * .57));
    p.drawLine(QPointF(r.center().x(), r.top() + 14),
               QPointF(r.right() - r.width() * .27, r.top() + r.height() * .65));
    p.setBrush(QColor("#ec3156"));
    p.setPen(QPen(QColor("#9f1535"), 4));
    const qreal d = r.width() * .34;
    p.drawEllipse(QRectF(r.left() + r.width() * .12, r.bottom() - d - 7, d, d));
    p.drawEllipse(QRectF(r.right() - r.width() * .12 - d, r.bottom() - d + 2, d, d));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 130));
    p.drawEllipse(QRectF(r.left() + r.width() * .20, r.bottom() - d + 4, 10, 10));
}

void drawFruit(QPainter &p, const QRectF &r, bool lemon)
{
    p.save();
    p.translate(r.center());
    p.rotate(lemon ? -17 : 0);
    const QRectF fruit(-r.width() * .35, -r.height() * .31, r.width() * .70, r.height() * .62);
    p.setPen(QPen(lemon ? QColor("#d1a918") : QColor("#c15f15"), 4));
    p.setBrush(lemon ? QColor("#ffe24c") : QColor("#ff8a27"));
    p.drawEllipse(fruit);
    p.setPen(QPen(QColor("#629444"), 4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(0, fruit.top() + 3), QPointF(8, fruit.top() - 12));
    p.restore();
}

void drawBell(QPainter &p, const QRectF &r)
{
    QPainterPath bell;
    bell.moveTo(r.center().x(), r.top() + r.height() * .09);
    bell.cubicTo(r.left() + r.width() * .27, r.top() + r.height() * .16,
                 r.left() + r.width() * .27, r.top() + r.height() * .58,
                 r.left() + r.width() * .13, r.top() + r.height() * .72);
    bell.lineTo(r.right() - r.width() * .13, r.top() + r.height() * .72);
    bell.cubicTo(r.right() - r.width() * .27, r.top() + r.height() * .58,
                 r.right() - r.width() * .27, r.top() + r.height() * .16,
                 r.center().x(), r.top() + r.height() * .09);
    p.setPen(QPen(QColor("#ad7410"), 5));
    p.setBrush(QColor("#f9bf32"));
    p.drawPath(bell);
    p.drawRoundedRect(QRectF(r.left() + r.width() * .11, r.top() + r.height() * .69,
                             r.width() * .78, r.height() * .13), 7, 7);
    p.setBrush(QColor("#d98b12"));
    p.drawEllipse(QRectF(r.center().x() - 10, r.top() + r.height() * .79, 20, 20));
}

} // namespace

ReelWidget::ReelWidget(Symbol symbol, QWidget *parent)
    : QWidget(parent), symbol_(symbol)
{
    setAccessibleName(GameLogic::symbolName(symbol_));
    setAccessibleDescription(QStringLiteral("Slot reel showing %1").arg(GameLogic::symbolName(symbol_)));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ReelWidget::setSymbol(Symbol symbol)
{
    if (symbol_ == symbol) {
        return;
    }
    symbol_ = symbol;
    setAccessibleName(GameLogic::symbolName(symbol_));
    setAccessibleDescription(QStringLiteral("Slot reel showing %1").arg(GameLogic::symbolName(symbol_)));
    update();
}

Symbol ReelWidget::symbol() const
{
    return symbol_;
}

QSize ReelWidget::sizeHint() const
{
    return {190, 235};
}

QSize ReelWidget::minimumSizeHint() const
{
    return {140, 180};
}

void ReelWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF card = QRectF(rect()).adjusted(5, 5, -5, -5);
    QLinearGradient bg(card.topLeft(), card.bottomLeft());
    bg.setColorAt(0, QColor("#fffdf6"));
    bg.setColorAt(1, QColor("#e8e1d3"));
    p.setPen(QPen(QColor("#cfae57"), 3));
    p.setBrush(bg);
    p.drawRoundedRect(card, 16, 16);

    const QRectF art = card.adjusted(18, 17, -18, -48);
    switch (symbol_) {
    case Symbol::Skull: drawSkull(p, art); break;
    case Symbol::Bar: drawBar(p, art); break;
    case Symbol::Seven: drawSeven(p, art); break;
    case Symbol::Cherry: drawCherry(p, art); break;
    case Symbol::Lemon: drawFruit(p, art, true); break;
    case Symbol::Orange: drawFruit(p, art, false); break;
    case Symbol::Bell: drawBell(p, art); break;
    }

    QFont labelFont = font();
    labelFont.setBold(true);
    labelFont.setPixelSize(15);
    labelFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.2);
    p.setFont(labelFont);
    p.setPen(QColor("#394052"));
    p.drawText(QRectF(card.left(), card.bottom() - 39, card.width(), 27),
               Qt::AlignCenter, GameLogic::symbolName(symbol_).toUpper());
}
