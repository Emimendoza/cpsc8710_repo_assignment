#include "gamelogic.h"

#include <QRandomGenerator>

#include <algorithm>
#include <utility>

namespace {
constexpr int SymbolCount = 7;
}

GameLogic::GameLogic()
    : GameLogic([] {
          return static_cast<Symbol>(QRandomGenerator::global()->bounded(SymbolCount));
      })
{
}

GameLogic::GameLogic(SymbolGenerator generator)
    : generator_(std::move(generator))
{
}

const GameState &GameLogic::state() const
{
    return state_;
}

bool GameLogic::canRoll() const
{
    return !state_.gameOver && state_.coins >= GameConfig::MinimumBet
           && state_.bet >= GameConfig::MinimumBet && state_.bet <= state_.coins;
}

bool GameLogic::canIncreaseBet() const
{
    return !state_.gameOver
           && state_.bet < std::min(state_.coins, GameConfig::MaximumBet);
}

bool GameLogic::canDecreaseBet() const
{
    return !state_.gameOver && state_.bet > GameConfig::MinimumBet;
}

void GameLogic::increaseBet()
{
    if (canIncreaseBet()) {
        ++state_.bet;
    }
}

void GameLogic::decreaseBet()
{
    if (canDecreaseBet()) {
        --state_.bet;
    }
}

std::optional<SpinResult> GameLogic::spin()
{
    if (!canRoll()) {
        return std::nullopt;
    }

    SpinResult result;
    result.wager = state_.bet;
    const int coinsBefore = state_.coins;

    ++state_.rolls;
    state_.coins -= result.wager;
    state_.moneyLost += result.wager;

    for (Symbol &symbol : result.reels) {
        symbol = generator_();
    }
    state_.reels = result.reels;
    result.outcome = classify(result.reels);

    switch (result.outcome) {
    case SpinOutcome::SkullJackpotLoss:
        state_.moneyLost += state_.coins;
        state_.coins = 0;
        break;
    case SpinOutcome::ThreeBar:
        result.payout = GameConfig::BarPayoutMultiplier * result.wager;
        state_.coins += result.payout;
        state_.moneyWon += result.payout;
        break;
    case SpinOutcome::ThreeOfAKind:
        result.payout = GameConfig::MatchingPayoutMultiplier * result.wager;
        state_.coins += result.payout;
        state_.moneyWon += result.payout;
        break;
    case SpinOutcome::NoPayout:
        break;
    }

    result.coinChange = state_.coins - coinsBefore;
    normalizeAfterCoinChange();
    state_.score = calculateScore();
    return result;
}

qint64 GameLogic::calculateScore() const
{
    const qint64 netMoneyChange = state_.moneyWon - state_.moneyLost;
    return netMoneyChange * state_.rolls * GameConfig::ScoreRollMultiplier;
}

SpinOutcome GameLogic::classify(const std::array<Symbol, 3> &reels)
{
    const bool allMatch = reels[0] == reels[1] && reels[1] == reels[2];
    if (!allMatch) {
        return SpinOutcome::NoPayout;
    }
    if (reels[0] == Symbol::Skull) {
        return SpinOutcome::SkullJackpotLoss;
    }
    if (reels[0] == Symbol::Bar) {
        return SpinOutcome::ThreeBar;
    }
    return SpinOutcome::ThreeOfAKind;
}

QString GameLogic::symbolName(Symbol symbol)
{
    switch (symbol) {
    case Symbol::Skull: return QStringLiteral("Skull");
    case Symbol::Bar: return QStringLiteral("BAR");
    case Symbol::Seven: return QStringLiteral("Seven");
    case Symbol::Cherry: return QStringLiteral("Cherry");
    case Symbol::Lemon: return QStringLiteral("Lemon");
    case Symbol::Orange: return QStringLiteral("Orange");
    case Symbol::Bell: return QStringLiteral("Bell");
    }
    return {};
}

void GameLogic::normalizeAfterCoinChange()
{
    if (state_.coins < GameConfig::MinimumBet) {
        state_.gameOver = true;
        state_.bet = GameConfig::MinimumBet;
        return;
    }

    state_.bet = std::clamp(state_.bet, GameConfig::MinimumBet,
                            std::min(state_.coins, GameConfig::MaximumBet));
}
