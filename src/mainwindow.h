#pragma once

#include "gamelogic.h"

#include <QMainWindow>

#include <array>
#include <optional>

class QLabel;
class QTimer;
class QToolButton;
class LeverButton;
class ReelWidget;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    GameLogic game_;
    QLabel *coinsLabel_ = nullptr;
    QLabel *rollsLabel_ = nullptr;
    QLabel *betLabel_ = nullptr;
    QLabel *scoreLabel_ = nullptr;
    QLabel *statusLabel_ = nullptr;
    QLabel *gameOverLabel_ = nullptr;
    QToolButton *decreaseButton_ = nullptr;
    QToolButton *increaseButton_ = nullptr;
    LeverButton *leverButton_ = nullptr;
    std::array<ReelWidget *, 3> reels_ {};
    QTimer *animationTimer_ = nullptr;
    std::optional<SpinResult> pendingResult_;
    int animationFrame_ = 0;
    bool animating_ = false;

    void buildUi();
    void connectUi();
    void refreshUi();
    void beginSpin();
    void advanceAnimation();
    void finishSpin();
    void setReels(const std::array<Symbol, 3> &symbols);
    [[nodiscard]] QString resultMessage(const SpinResult &result) const;
};
