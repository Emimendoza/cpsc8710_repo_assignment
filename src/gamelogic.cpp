#include "gamelogic.h"

#include <QRandomGenerator>

#include <algorithm>
#include <cmath>
#include <utility>

namespace {
constexpr int SymbolCount = 7;
}

GameLogic::GameLogic()
    : GameLogic([] {
          return static_cast<Symbol>(QRandomGenerator::global()->bounded(SymbolCount));
      }, GameConfig::RiggedMode)
{
}

GameLogic::GameLogic(SymbolGenerator generator, bool riggedMode)
    : generator_(std::move(generator)), riggedMode_(riggedMode)
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

    result.reels[0] = generator_();
    if (riggedMode_) {
        result.reels.fill(result.reels[0]);
    } else {
        result.reels[1] = generator_();
        result.reels[2] = generator_();
    }
    state_.reels = result.reels;
    result.outcome = classify(result.reels);

    switch (result.outcome) {
    case SpinOutcome::SkullJackpotLoss:
        Q_ASSERT(threeMatchPayoutMultiplier(result.reels[0])
                 == GameConfig::LoseAllCoinsPayoutMultiplier);
        state_.moneyLost += state_.coins;
        state_.coins = 0;
        break;
    case SpinOutcome::ThreeBar:
    case SpinOutcome::ThreeOfAKind:
        result.payout = calculatePayout(result.wager,
                                        threeMatchPayoutMultiplier(result.reels[0]));
        state_.coins += result.payout;
        state_.moneyWon += result.payout;
        break;
    case SpinOutcome::TwoOfAKind:
        result.payout = calculatePayout(result.wager,
                                        GameConfig::TwoMatchingPayoutMultiplier);
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
    if (allMatch) {
        if (threeMatchPayoutMultiplier(reels[0])
            == GameConfig::LoseAllCoinsPayoutMultiplier) {
            return SpinOutcome::SkullJackpotLoss;
        }
        if (reels[0] == Symbol::Bar) {
            return SpinOutcome::ThreeBar;
        }
        return SpinOutcome::ThreeOfAKind;
    }

    const bool hasPair = reels[0] == reels[1] || reels[0] == reels[2]
                         || reels[1] == reels[2];
    return hasPair ? SpinOutcome::TwoOfAKind : SpinOutcome::NoPayout;
}

float GameLogic::threeMatchPayoutMultiplier(Symbol symbol)
{
    switch (symbol) {
    case Symbol::Skull: return GameConfig::SkullPayoutMultiplier;
    case Symbol::Bar: return GameConfig::BarPayoutMultiplier;
    case Symbol::Seven: return GameConfig::SevenPayoutMultiplier;
    case Symbol::Cherry: return GameConfig::CherryPayoutMultiplier;
    case Symbol::Lemon: return GameConfig::LemonPayoutMultiplier;
    case Symbol::Orange: return GameConfig::OrangePayoutMultiplier;
    case Symbol::Bell: return GameConfig::BellPayoutMultiplier;
    }
    return 0.0F;
}

int GameLogic::calculatePayout(int wager, float multiplier)
{
    Q_ASSERT(wager >= 0);
    Q_ASSERT(multiplier >= 0.0F);
    const float grossPayout = static_cast<float>(wager) * multiplier;
    return static_cast<int>(std::floor(grossPayout));
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
