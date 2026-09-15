#include "mainwindow.h"

#include "leverbutton.h"
#include "reelwidget.h"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QRandomGenerator>
#include <QShortcut>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

QLabel *makeHudLabel(QWidget *parent)
{
    auto *label = new QLabel(parent);
    label->setObjectName(QStringLiteral("hudLabel"));
    return label;
}

QToolButton *makeBetButton(Qt::ArrowType arrow, const QString &accessibleName,
                           const QString &toolTip, QWidget *parent)
{
    auto *button = new QToolButton(parent);
    button->setArrowType(arrow);
    button->setAccessibleName(accessibleName);
    button->setToolTip(toolTip);
    button->setFixedSize(48, 42);
    button->setFocusPolicy(Qt::StrongFocus);
    return button;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
    connectUi();
    refreshUi();
}

void MainWindow::buildUi()
{
    setWindowTitle(QStringLiteral("Lucky Seven Slots"));
    setFixedSize(900, 520);

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("central"));
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(34, 24, 34, 25);
    root->setSpacing(13);

    auto *top = new QHBoxLayout;
    coinsLabel_ = makeHudLabel(central);
    rollsLabel_ = makeHudLabel(central);
    rollsLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    top->addWidget(coinsLabel_);
    top->addStretch();
    top->addWidget(rollsLabel_);
    root->addLayout(top);

    auto *middle = new QHBoxLayout;
    middle->setSpacing(24);

    auto *machine = new QFrame(central);
    machine->setObjectName(QStringLiteral("machine"));
    machine->setAccessibleName(QStringLiteral("Slot machine reels"));
    auto *machineShadow = new QGraphicsDropShadowEffect(machine);
    machineShadow->setBlurRadius(28);
    machineShadow->setOffset(0, 8);
    machineShadow->setColor(QColor(0, 0, 0, 110));
    machine->setGraphicsEffect(machineShadow);

    auto *machineLayout = new QVBoxLayout(machine);
    machineLayout->setContentsMargins(18, 10, 18, 14);
    machineLayout->setSpacing(7);

    auto *title = new QLabel(QStringLiteral("★  LUCKY SEVEN SLOTS  ★"), machine);
    title->setObjectName(QStringLiteral("machineTitle"));
    title->setAlignment(Qt::AlignCenter);
    machineLayout->addWidget(title);

    auto *reelRow = new QHBoxLayout;
    reelRow->setSpacing(10);
    const auto initial = game_.state().reels;
    for (std::size_t i = 0; i < reels_.size(); ++i) {
        reels_[i] = new ReelWidget(initial[i], machine);
        reelRow->addWidget(reels_[i], 1);
    }
    machineLayout->addLayout(reelRow, 1);

    gameOverLabel_ = new QLabel(QStringLiteral("GAME OVER"), machine);
    gameOverLabel_->setObjectName(QStringLiteral("gameOver"));
    gameOverLabel_->setAlignment(Qt::AlignCenter);
    gameOverLabel_->setAccessibleName(QStringLiteral("Game over"));
    gameOverLabel_->hide();
    machineLayout->addWidget(gameOverLabel_);

    middle->addWidget(machine, 1);
    leverButton_ = new LeverButton(central);
    middle->addWidget(leverButton_, 0, Qt::AlignCenter);
    root->addLayout(middle, 1);

    statusLabel_ = new QLabel(QStringLiteral("Choose your bet and pull the lever"), central);
    statusLabel_->setObjectName(QStringLiteral("statusLabel"));
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setAccessibleName(QStringLiteral("Spin result"));
    statusLabel_->setMinimumHeight(28);
    root->addWidget(statusLabel_);

    auto *bottom = new QHBoxLayout;
    bottom->setSpacing(9);
    decreaseButton_ = makeBetButton(Qt::DownArrow, QStringLiteral("Decrease bet"),
                                    QStringLiteral("Decrease bet (Down arrow)"), central);
    increaseButton_ = makeBetButton(Qt::UpArrow, QStringLiteral("Increase bet"),
                                    QStringLiteral("Increase bet (Up arrow)"), central);
    betLabel_ = makeHudLabel(central);
    scoreLabel_ = makeHudLabel(central);
    scoreLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bottom->addWidget(decreaseButton_);
    bottom->addWidget(increaseButton_);
    bottom->addSpacing(10);
    bottom->addWidget(betLabel_);
    bottom->addStretch();
    bottom->addWidget(scoreLabel_);
    root->addLayout(bottom);

    animationTimer_ = new QTimer(this);
    animationTimer_->setInterval(65);

    new QShortcut(QKeySequence(Qt::Key_Down), this, [this] {
        if (decreaseButton_->isEnabled()) decreaseButton_->click();
    });
    new QShortcut(QKeySequence(Qt::Key_Up), this, [this] {
        if (increaseButton_->isEnabled()) increaseButton_->click();
    });

    setStyleSheet(QStringLiteral(R"(
        QWidget#central {
            background-color: #101421;
            color: #f8f1dd;
            font-family: "DejaVu Sans";
        }
        QLabel#hudLabel {
            color: #fff4c0;
            font-size: 23px;
            font-weight: 700;
        }
        QFrame#machine {
            background-color: #262d41;
            border: 3px solid #d0a83e;
            border-radius: 19px;
        }
        QLabel#machineTitle {
            color: #f7cf58;
            font-size: 18px;
            font-weight: 800;
            letter-spacing: 3px;
        }
        QLabel#statusLabel {
            color: #d9ddeb;
            font-size: 15px;
            font-weight: 600;
        }
        QLabel#gameOver {
            color: #ff5875;
            background-color: #361c2b;
            border: 1px solid #9b3450;
            border-radius: 7px;
            font-size: 19px;
            font-weight: 900;
            letter-spacing: 4px;
            padding: 4px;
        }
        QToolButton {
            color: #172033;
            background-color: #f0c64f;
            border: 2px solid #fff0a3;
            border-radius: 9px;
        }
        QToolButton:hover { background-color: #ffdc73; }
        QToolButton:pressed { background-color: #c89d2e; padding-top: 3px; }
        QToolButton:focus { border: 3px solid white; }
        QToolButton:disabled {
            color: #585e6d;
            background-color: #343a4b;
            border-color: #4b5265;
        }
    )"));
}

void MainWindow::connectUi()
{
    connect(decreaseButton_, &QToolButton::clicked, this, [this] {
        game_.decreaseBet();
        refreshUi();
    });
    connect(increaseButton_, &QToolButton::clicked, this, [this] {
        game_.increaseBet();
        refreshUi();
    });
    connect(leverButton_, &QPushButton::clicked, this, [this] { beginSpin(); });
    connect(animationTimer_, &QTimer::timeout, this, [this] { advanceAnimation(); });
}

void MainWindow::refreshUi()
{
    const GameState &state = game_.state();
    coinsLabel_->setText(QStringLiteral("Coins: %1").arg(state.coins));
    rollsLabel_->setText(QStringLiteral("Rolls: %1").arg(state.rolls));
    betLabel_->setText(QStringLiteral("Bet: %1").arg(state.bet));
    scoreLabel_->setText(QStringLiteral("Score: %1").arg(state.score));

    leverButton_->setEnabled(!animating_ && game_.canRoll());
    decreaseButton_->setEnabled(!animating_ && game_.canDecreaseBet());
    increaseButton_->setEnabled(!animating_ && game_.canIncreaseBet());
    gameOverLabel_->setVisible(state.gameOver);
}

void MainWindow::beginSpin()
{
    if (animating_ || !game_.canRoll()) {
        return;
    }

    pendingResult_ = game_.spin();
    if (!pendingResult_) {
        return;
    }

    animating_ = true;
    animationFrame_ = 0;
    statusLabel_->setText(QStringLiteral("Spinning…"));
    statusLabel_->setAccessibleDescription(QStringLiteral("The reels are spinning"));
    refreshUi();
    animationTimer_->start();
}

void MainWindow::advanceAnimation()
{
    ++animationFrame_;
    if (animationFrame_ >= 10) {
        finishSpin();
        return;
    }

    for (ReelWidget *reel : reels_) {
        reel->setSymbol(static_cast<Symbol>(QRandomGenerator::global()->bounded(7)));
    }
}

void MainWindow::finishSpin()
{
    animationTimer_->stop();
    if (!pendingResult_) {
        animating_ = false;
        refreshUi();
        return;
    }

    setReels(pendingResult_->reels);
    const QString message = resultMessage(*pendingResult_);
    statusLabel_->setText(message);
    statusLabel_->setAccessibleDescription(message);
    pendingResult_.reset();
    animating_ = false;
    refreshUi();
}

void MainWindow::setReels(const std::array<Symbol, 3> &symbols)
{
    for (std::size_t i = 0; i < reels_.size(); ++i) {
        reels_[i]->setSymbol(symbols[i]);
    }
}

QString MainWindow::resultMessage(const SpinResult &result) const
{
    switch (result.outcome) {
    case SpinOutcome::SkullJackpotLoss:
        return QStringLiteral("Triple skulls — all coins lost");
    case SpinOutcome::ThreeBar:
        return QStringLiteral("Three BARs! +%1 coins").arg(result.payout);
    case SpinOutcome::ThreeOfAKind:
        return QStringLiteral("Three %1s! +%2 coins")
            .arg(GameLogic::symbolName(result.reels[0]), QString::number(result.payout));
    case SpinOutcome::TwoOfAKind:
        return QStringLiteral("Pair — %1 coin bet returned").arg(result.payout);
    case SpinOutcome::NoPayout:
        return QStringLiteral("No payout");
    }
    return {};
}
