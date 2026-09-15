#pragma once

#include "gameconfig.h"

#include <QMetaType>
#include <QString>
#include <QtTypes>

#include <array>
#include <functional>
#include <optional>

enum class Symbol {
    Skull,
    Bar,
    Seven,
    Cherry,
    Lemon,
    Orange,
    Bell
};

enum class SpinOutcome {
    SkullJackpotLoss,
    ThreeBar,
    ThreeOfAKind,
    TwoOfAKind,
    NoPayout
};

struct GameState {
    int coins = GameConfig::StartingCoins;
    int bet = GameConfig::MinimumBet;
    int rolls = 0;
    qint64 moneyWon = 0;
    qint64 moneyLost = 0;
    qint64 score = 0;
    bool gameOver = false;
    std::array<Symbol, 3> reels {Symbol::Cherry, Symbol::Seven, Symbol::Bar};
};

struct SpinResult {
    std::array<Symbol, 3> reels;
    SpinOutcome outcome = SpinOutcome::NoPayout;
    int wager = 0;
    int payout = 0;
    int coinChange = 0;
};

class GameLogic {
public:
    using SymbolGenerator = std::function<Symbol()>;

    GameLogic();
    explicit GameLogic(SymbolGenerator generator);

    [[nodiscard]] const GameState &state() const;
    [[nodiscard]] bool canRoll() const;
    [[nodiscard]] bool canIncreaseBet() const;
    [[nodiscard]] bool canDecreaseBet() const;

    void increaseBet();
    void decreaseBet();
    std::optional<SpinResult> spin();
    [[nodiscard]] qint64 calculateScore() const;

    static SpinOutcome classify(const std::array<Symbol, 3> &reels);
    static int threeMatchPayoutMultiplier(Symbol symbol);
    static QString symbolName(Symbol symbol);

private:
    GameState state_;
    SymbolGenerator generator_;

    void normalizeAfterCoinChange();
};

Q_DECLARE_METATYPE(Symbol)
Q_DECLARE_METATYPE(SpinOutcome)
