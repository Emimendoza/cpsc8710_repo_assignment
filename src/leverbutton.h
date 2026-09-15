#pragma once

#include <QPushButton>

class LeverButton final : public QPushButton {
public:
    explicit LeverButton(QWidget *parent = nullptr);

    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
};
