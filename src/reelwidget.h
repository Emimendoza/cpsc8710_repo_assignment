#pragma once

#include "gamelogic.h"

#include <QWidget>

class ReelWidget final : public QWidget {
public:
    explicit ReelWidget(Symbol symbol, QWidget *parent = nullptr);

    void setSymbol(Symbol symbol);
    [[nodiscard]] Symbol symbol() const;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Symbol symbol_;
};
