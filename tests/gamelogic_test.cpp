#include "gamelogic.h"

#include <QTest>

#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace {

GameLogic withSequence(std::initializer_list<Symbol> symbols, bool riggedMode = false)
{
    auto sequence = std::make_shared<std::vector<Symbol>>(symbols);
    auto index = std::make_shared<std::size_t>(0);
    return GameLogic([sequence, index] {
        const Symbol value = sequence->at(*index % sequence->size());
        ++*index;
        return value;
    }, riggedMode);
}

void setBet(GameLogic &game, int amount)
{
    while (game.state().bet < amount) {
        game.increaseBet();
    }
}

} // namespace

class GameLogicTest final : public QObject {
    Q_OBJECT

private slots:
    void initialState();
    void betCannotGoBelowMinimum();
    void betCannotExceedCoins();
    void validRollIncrementsOnce();
    void twoMatchingSymbolsReturnBet();
    void threeMatchUsesConfiguredMultiplier();
    void fractionalPayoutRoundsDown();
    void riggedModeAlwaysProducesTriples();
    void threeSkullsEndGameAndLoseEverything();
    void scoreUsesNetChangeAndRollNumber();
    void cannotRollAfterGameOver();
    void coinDecreaseClampsBet();
};

void GameLogicTest::initialState()
{
    GameLogic game;
    QCOMPARE(game.state().coins, 100);
    QCOMPARE(game.state().bet, 1);
    QCOMPARE(game.state().rolls, 0);
    QCOMPARE(game.state().score, 0);
    QVERIFY(!game.state().gameOver);
}

void GameLogicTest::betCannotGoBelowMinimum()
{
    GameLogic game;
    game.decreaseBet();
    QCOMPARE(game.state().bet, GameConfig::MinimumBet);
    QVERIFY(!game.canDecreaseBet());
}

void GameLogicTest::betCannotExceedCoins()
{
    GameLogic game;
    for (int i = 0; i < 150; ++i) {
        game.increaseBet();
    }
    QCOMPARE(game.state().bet, game.state().coins);
    QVERIFY(!game.canIncreaseBet());
}

void GameLogicTest::validRollIncrementsOnce()
{
    auto game = withSequence({Symbol::Cherry, Symbol::Lemon, Symbol::Bell});
    QVERIFY(game.spin().has_value());
    QCOMPARE(game.state().rolls, 1);
}

void GameLogicTest::twoMatchingSymbolsReturnBet()
{
    auto game = withSequence({Symbol::Bar, Symbol::Bar, Symbol::Seven});
    setBet(game, 5);
    const auto result = game.spin();
    QVERIFY(result.has_value());
    QCOMPARE(result->outcome, SpinOutcome::TwoOfAKind);
    QCOMPARE(result->payout, 5);
    QCOMPARE(game.state().coins, 100);
    QCOMPARE(game.state().moneyWon, 5);
    QCOMPARE(game.state().moneyLost, 5);
    QCOMPARE(game.state().score, 0);
}

void GameLogicTest::threeMatchUsesConfiguredMultiplier()
{
    const std::array configuredPayouts {
        std::pair {Symbol::Bar, GameConfig::BarPayoutMultiplier},
        std::pair {Symbol::Seven, GameConfig::SevenPayoutMultiplier},
        std::pair {Symbol::Cherry, GameConfig::CherryPayoutMultiplier},
        std::pair {Symbol::Lemon, GameConfig::LemonPayoutMultiplier},
        std::pair {Symbol::Orange, GameConfig::OrangePayoutMultiplier},
        std::pair {Symbol::Bell, GameConfig::BellPayoutMultiplier},
    };

    for (const auto &[symbol, multiplier] : configuredPayouts) {
        auto game = withSequence({symbol, symbol, symbol});
        setBet(game, 4);
        const auto result = game.spin();
        QVERIFY(result.has_value());
        QCOMPARE(result->outcome, symbol == Symbol::Bar
                                      ? SpinOutcome::ThreeBar
                                      : SpinOutcome::ThreeOfAKind);
        QCOMPARE(GameLogic::threeMatchPayoutMultiplier(symbol), multiplier);
        const int expectedPayout = GameLogic::calculatePayout(4, multiplier);
        QCOMPARE(result->payout, expectedPayout);
        QCOMPARE(game.state().coins, 100 - 4 + expectedPayout);
        QCOMPARE(game.state().moneyWon, expectedPayout);
    }
}

void GameLogicTest::fractionalPayoutRoundsDown()
{
    QCOMPARE(GameLogic::calculatePayout(5, 1.5F), 7);
    QCOMPARE(GameLogic::calculatePayout(5, 1.25F), 6);
    QCOMPARE(GameLogic::calculatePayout(3, 0.5F), 1);
}

void GameLogicTest::riggedModeAlwaysProducesTriples()
{
    auto game = withSequence({Symbol::Cherry, Symbol::Lemon, Symbol::Orange}, true);

    for (const Symbol expected : {Symbol::Cherry, Symbol::Lemon, Symbol::Orange}) {
        const auto result = game.spin();
        QVERIFY(result.has_value());
        QCOMPARE(result->reels[0], expected);
        QCOMPARE(result->reels[1], expected);
        QCOMPARE(result->reels[2], expected);
        QCOMPARE(result->outcome, SpinOutcome::ThreeOfAKind);
    }
}

void GameLogicTest::threeSkullsEndGameAndLoseEverything()
{
    auto game = withSequence({Symbol::Skull, Symbol::Skull, Symbol::Skull});
    setBet(game, 7);
    const auto result = game.spin();
    QCOMPARE(GameConfig::SkullPayoutMultiplier,
             GameConfig::LoseAllCoinsPayoutMultiplier);
    QCOMPARE(GameConfig::SkullPayoutMultiplier, -1.0F);
    QCOMPARE(result->outcome, SpinOutcome::SkullJackpotLoss);
    QCOMPARE(game.state().coins, 0);
    QCOMPARE(game.state().moneyLost, 100);
    QCOMPARE(game.state().score, -100);
    QVERIFY(game.state().gameOver);
}

void GameLogicTest::scoreUsesNetChangeAndRollNumber()
{
    auto game = withSequence({Symbol::Bell, Symbol::Bell, Symbol::Bell,
                              Symbol::Cherry, Symbol::Lemon, Symbol::Orange});
    setBet(game, 5);
    QVERIFY(game.spin().has_value());
    QCOMPARE(game.state().score, 5);
    QVERIFY(game.spin().has_value());
    QCOMPARE(game.state().moneyWon, 10);
    QCOMPARE(game.state().moneyLost, 10);
    QCOMPARE(game.state().score, 0);
}

void GameLogicTest::cannotRollAfterGameOver()
{
    auto game = withSequence({Symbol::Skull, Symbol::Skull, Symbol::Skull});
    QVERIFY(game.spin().has_value());
    QVERIFY(!game.canRoll());
    QVERIFY(!game.spin().has_value());
    QCOMPARE(game.state().rolls, 1);
}

void GameLogicTest::coinDecreaseClampsBet()
{
    auto game = withSequence({Symbol::Cherry, Symbol::Lemon, Symbol::Orange});
    setBet(game, 70);
    QVERIFY(game.spin().has_value());
    QCOMPARE(game.state().coins, 30);
    QCOMPARE(game.state().bet, 30);
}

QTEST_APPLESS_MAIN(GameLogicTest)

#include "gamelogic_test.moc"
