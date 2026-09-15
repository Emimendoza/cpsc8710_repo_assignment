# Qt 6 Slot Machine Game — Design Specification

## 1. Purpose

Create a small, single-window desktop slot machine game using Qt 6. The game is intentionally simple: the player repeatedly selects a bet and pulls a GUI lever until their coins reach zero. There is no winning condition or target score.

The implementation should favor clear, testable game logic separated from the Qt user interface.

## 2. Technology and project requirements

- Language: C++20 or newer.
- Framework: Qt 6 Widgets.
- Build system: CMake.
- Application type: one non-resizable main window.
- No network, account, persistence, or external services.
- Use Qt's standard random facilities (`QRandomGenerator`) for spin results.
- Use Qt resource files (`.qrc`) for bundled icons if image assets are used.

Suggested project files:

```text
CMakeLists.txt
src/main.cpp
src/mainwindow.h
src/mainwindow.cpp
src/gamelogic.h
src/gamelogic.cpp
resources/resources.qrc
resources/icons/...
```

## 3. Core configurable rules

Put all balance and rules values in one clearly named configuration structure or namespace so they can be changed without searching through UI code.

```cpp
namespace GameConfig {
constexpr int StartingCoins = 100;
constexpr int MinimumBet = 1;
constexpr int MaximumBet = StartingCoins; // Also clamp to current coins.
constexpr float TwoMatchingPayoutMultiplier = 1.0F;
constexpr float LoseAllCoinsPayoutMultiplier = -1.0F;
constexpr float SkullPayoutMultiplier = LoseAllCoinsPayoutMultiplier;
constexpr float BarPayoutMultiplier = 5.0F;
constexpr float SevenPayoutMultiplier = 2.0F;
constexpr float CherryPayoutMultiplier = 2.0F;
constexpr float LemonPayoutMultiplier = 2.0F;
constexpr float OrangePayoutMultiplier = 2.0F;
constexpr float BellPayoutMultiplier = 2.0F;
constexpr bool RiggedMode = false;
constexpr int ScoreRollMultiplier = 1; // Score = net money change * roll number * this value.
}
```

Interpretation of payouts:

1. A valid roll requires `bet >= MinimumBet` and `bet <= coins`.
2. Deduct the bet immediately when the roll begins.
3. Calculate a payout as `floor(bet * payoutMultiplier)` so all credited amounts are whole coins and fractional results round down.
4. Add the calculated payout to coins after evaluating the result.
5. Exactly two matching symbols return `1.0F * bet` by default; because the bet was already paid, the player's net coin change is zero.
6. A `2.0F` triple result returns `2 * bet`; because the bet was already paid, the player's net coin change is `+bet`.
7. A `5.0F` BAR result returns `5 * bet`; the player's net coin change is `+4 * bet`.
8. A three-symbol match configured with the special `-1.0F` multiplier sets coins to zero. By default this is the three-Skull result.
9. If coins are zero at any point, the game is over and no further rolls are allowed.

The exact meaning of “money won” and “money lost” must be tracked explicitly:

- `moneyWon`: gross, rounded-down payout coins credited by successful matching outcomes.
- `moneyLost`: wager coins deducted on every valid roll, plus any remaining coins destroyed by the skull outcome.
- `netMoneyChange = moneyWon - moneyLost`.
- `score = netMoneyChange * currentRoll * ScoreRollMultiplier`.

For a skull, record the coins remaining immediately after deducting the wager as additional `moneyLost` before setting coins to zero. This makes the “lose all money” rule reflected in the score. Display score as a signed integer; it may be negative.

The score is recomputed after every valid roll from the tracked totals and the current roll number. Keep this formula in one function so it is easy to modify.

## 4. Symbols and result rules

Use a standard slot-machine icon set containing at least:

- `Skull`
- `BAR`
- `Seven`
- `Cherry`
- `Lemon`
- `Orange`
- `Bell`

The three reels are independent random symbol selections during normal play. When `RiggedMode` is enabled, generate one random symbol per roll and repeat it across all three reels, guaranteeing a three-symbol match. Store symbols as an enum plus display metadata, not as UI strings alone.

Evaluate outcomes in this order:

| Reel result | Effect |
|---|---|
| Three symbols whose multiplier is `-1.0F` | Lose all coins; game over; no payout |
| Three BAR symbols | Credit `floor(BarPayoutMultiplier * bet)` |
| Any other three identical symbols | Credit the rounded-down product of that symbol's configured multiplier and the bet |
| Exactly two matching symbols | Credit `floor(TwoMatchingPayoutMultiplier * bet)`, returning the wager by default |
| Three different symbols | No payout and no extra penalty |

“Two matching” includes any pair, including two BAR or two Skull symbols. A result is only “three BAR” when all three symbols are BAR. A three-Skull result uses the configured `-1.0F` sentinel and takes precedence over generic three-of-a-kind.

For maintainability, use a result enum such as `SkullJackpotLoss`, `ThreeBar`, `ThreeOfAKind`, `TwoOfAKind`, and `NoPayout`.

## 5. Window and layout

Create one `QMainWindow` with a central widget and a vertically arranged layout. Use a dark casino-inspired theme while keeping text and controls high contrast.

The window should be approximately 900×500 pixels at startup and should not be resizable. Use layout stretch/spacers rather than fixed absolute positioning.

### Top row

Horizontal layout:

```text
Coins: {coins}                         Rolls: {rolls}
```

- Left label: `Coins: 100` initially.
- Expanding horizontal spacer.
- Right label: `Rolls: 0` initially.
- Use a large, bold font suitable for a HUD.

### Middle row

Horizontal layout:

```text
[ slot machine window with 3 icons ]       [ GUI lever ]
```

Slot machine area:

- A framed panel or `QGroupBox` titled `SLOTS`.
- Three equal-sized reel cells in a horizontal layout.
- Each cell shows one icon and optionally its symbol name as accessible text.
- Initial symbols may be `Cherry`, `Seven`, and `BAR`; the first pull replaces them.
- Make the reel panel the dominant visual element.

Lever area:

- A clearly visible vertical lever control on the right.
- Implement the clickable control as a custom `QPushButton` or a small custom `QWidget` with pressed/released painting.
- The lever must visually move down while pressed and return to its resting position on release.
- It must expose a keyboard-accessible label, tooltip, and mnemonic if practical: `Pull lever`.
- Clicking or activating it triggers exactly one roll. Ignore additional activations while a roll is animating.

### Bottom row

Horizontal layout:

```text
[down arrow] [up arrow]    Bet: {bet}                    Score: {score}
```

- Down arrow button decreases the bet by one.
- Up arrow button increases the bet by one.
- Use `QToolButton` or `QPushButton` with `Qt::Key_Down`/`Qt::Key_Up`-appropriate arrow glyphs.
- Disable down when `bet == MinimumBet`.
- Disable up when `bet == min(coins, MaximumBet)`.
- Expanding spacer separates the bet controls from the score label.
- Initial display: `Bet: 1` and `Score: 0`.

### Game-over state

When coins reach zero:

- Disable the lever and both bet buttons.
- Keep the final reel symbols, roll count, and score visible.
- Show a prominent `GAME OVER` message in the slot panel or directly below it.
- Do not automatically restart. A restart button is optional only if it fits the single-window design; if included, it must reset all state to the initial values after user activation.

## 6. Interaction and state model

Maintain these game-state fields in a non-UI game logic class:

```cpp
struct GameState {
    int coins = GameConfig::StartingCoins;
    int bet = GameConfig::MinimumBet;
    int rolls = 0;
    qint64 moneyWon = 0;
    qint64 moneyLost = 0;
    qint64 score = 0;
    bool gameOver = false;
    std::array<Symbol, 3> reels;
};
```

Expose methods similar to:

```cpp
bool canRoll() const;
void increaseBet();
void decreaseBet();
SpinResult spin();
qint64 calculateScore() const;
```

The UI must not directly modify coins, payouts, or score. It calls game-logic methods and refreshes labels/buttons from the resulting state.

Bet rules:

- Clamp bet to `[MinimumBet, min(coins, MaximumBet)]`.
- On a coin decrease, automatically clamp the current bet if it exceeds the new coin balance.
- If coins are below `MinimumBet`, mark the game over state.
- A bet change does not increment rolls or change score.

## 7. Roll sequence and optional animation

On lever activation:

1. Check `gameOver`, animation state, and `canRoll()`.
2. Increment `rolls`.
3. Deduct `bet` and add it to `moneyLost`.
4. Generate three independent symbols in normal mode. In rigged mode, generate one symbol and repeat it on all three reels.
5. Classify the result using the precedence in Section 4.
6. Apply the result's payout or skull loss.
7. Recalculate score.
8. Update all labels, icons, button enabled states, and game-over state.
9. Show a short result message such as `Three BARs! +20 coins` or `No payout`.

An animation is optional but recommended. A simple implementation can use a `QTimer` for 500–800 ms, rapidly cycling visual symbols before showing the final generated symbols. Generate the authoritative result once at the start of the roll; animation must not alter it. Disable the lever and bet controls during animation.

If animation is omitted, update the reels immediately and still preserve the same state transition order.

## 8. Icon rendering and accessibility

- Prefer bundled SVG or high-quality raster icons with transparent backgrounds.
- Use consistent dimensions, such as 96×96 pixels per reel cell.
- Do not rely on color alone to distinguish symbols.
- Set meaningful accessible names/descriptions for the lever, bet buttons, reel cells, and status message.
- Ensure all primary actions are usable with keyboard focus and Enter/Space activation.
- Keep symbol names available to screen readers even if the visual representation is only an icon.

## 9. Randomness and testability

Inject or wrap the random-symbol generator so game logic can be tested deterministically. The production implementation may use `QRandomGenerator::global()`, while tests may supply a sequence of symbols.

Do not use randomness to decide payout separately from the reel symbols. Payout must always be derived from the displayed final symbols.

Expose `GameConfig::RiggedMode` as a compile-time switch, defaulting to `false`. The game-logic constructor may also accept a rigged-mode override so automated tests can exercise the mode without editing the configuration and rebuilding. When active, rigged mode must guarantee three identical final symbols on every valid roll. The UI should visibly identify rigged mode when it is enabled through `gameconfig.h`.

Optional future balance control:

```cpp
struct SymbolDefinition {
    Symbol symbol;
    QString name;
    QIcon icon;
    int weight;
};
```

If weights are implemented, default all weights to the same value so the initial game has uniform symbol selection.

## 10. Validation and acceptance criteria

The coding agent should provide unit tests for the game-logic class covering:

- Initial state is 100 coins, bet 1, rolls 0, score 0, and not game over.
- Bet cannot go below the minimum.
- Bet cannot exceed current coins.
- A valid roll increments rolls exactly once.
- Two matching symbols credit `floor(TwoMatchingPayoutMultiplier * bet)`, returning the wager with the default multiplier.
- Every three-symbol match uses its symbol-specific configured multiplier.
- Fractional payout results round down to whole coins.
- Three BAR symbols credit exactly `floor(BarPayoutMultiplier * bet)`.
- Three Skull symbols set coins to zero and game over.
- Rigged mode produces three identical symbols on every valid roll.
- Score follows `netMoneyChange * currentRoll * ScoreRollMultiplier`.
- No roll is possible after game over.
- A coin decrease clamps the bet when necessary.
- A roll with insufficient coins is rejected.

Manual UI acceptance checks:

- The window contains only the specified single-window game UI.
- The top row shows coins and rolls with the spacer between them.
- The middle row shows three reel icons and a lever to their right.
- The bottom row shows down/up bet controls, bet value, and score with spacers.
- The lever visibly responds to press/click and cannot trigger duplicate rolls during animation.
- The game-over state is obvious and all relevant controls are disabled.
- The application builds and runs on a standard Qt 6 desktop environment.

## 11. Suggested implementation notes

- Use signals from the game logic or main window to refresh the presentation after state changes.
- Keep all user-facing strings in one place where practical to make later localization easy.
- Use `qint64` for score and money counters even though the initial balance is small.
- Avoid blocking sleeps in the GUI thread. Use `QTimer` for animation.
- Keep the payout classification as a pure function so it can be tested without constructing widgets.
- Log no sensitive information; there is no user data to persist.
