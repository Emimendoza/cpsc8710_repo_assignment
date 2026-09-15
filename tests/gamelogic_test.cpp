#include "gamelogic.h"

#include <QTest>

#include <cstddef>
#include <memory>
#include <vector>

namespace {

GameLogic withSequence(std::initializer_list<Symbol> symbols)
{
    auto sequence = std::make_shared<std::vector<Symbol>>(symbols);
    auto index = std::make_shared<std::size_t>(0);
    return GameLogic([sequence, index] {
        const Symbol value = sequence->at(*index % sequence->size());
        ++*index;
        return value;
    });
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
    void twoMatchingSymbolsDoNotPay();
    void threeMatchingSymbolsPayDouble();
    void threeBarsPayFiveTimes();
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

void GameLogicTest::twoMatchingSymbolsDoNotPay()
{
    auto game = withSequence({Symbol::Bar, Symbol::Bar, Symbol::Seven});
    setBet(game, 4);
    const auto result = game.spin();
    QVERIFY(result.has_value());
    QCOMPARE(result->outcome, SpinOutcome::NoPayout);
    QCOMPARE(result->payout, 0);
    QCOMPARE(game.state().coins, 96);
    QCOMPARE(game.state().moneyWon, 0);
}

void GameLogicTest::threeMatchingSymbolsPayDouble()
{
    auto game = withSequence({Symbol::Cherry, Symbol::Cherry, Symbol::Cherry});
    setBet(game, 4);
    const auto result = game.spin();
    QCOMPARE(result->outcome, SpinOutcome::ThreeOfAKind);
    QCOMPARE(result->payout, 8);
    QCOMPARE(game.state().coins, 104);
    QCOMPARE(game.state().moneyWon, 8);
}

void GameLogicTest::threeBarsPayFiveTimes()
{
    auto game = withSequence({Symbol::Bar, Symbol::Bar, Symbol::Bar});
    setBet(game, 4);
    const auto result = game.spin();
    QCOMPARE(result->outcome, SpinOutcome::ThreeBar);
    QCOMPARE(result->payout, 20);
    QCOMPARE(game.state().coins, 116);
}

void GameLogicTest::threeSkullsEndGameAndLoseEverything()
{
    auto game = withSequence({Symbol::Skull, Symbol::Skull, Symbol::Skull});
    setBet(game, 7);
    const auto result = game.spin();
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
