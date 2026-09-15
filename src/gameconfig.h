#pragma once

namespace GameConfig {

inline constexpr int StartingCoins = 100;
inline constexpr int MinimumBet = 1;
inline constexpr int MaximumBet = StartingCoins;

// Gross payouts. A pair returns the wager; -1 means the player loses all coins.
inline constexpr int TwoMatchingPayoutMultiplier = 1;
inline constexpr int LoseAllCoinsPayoutMultiplier = -1;
inline constexpr int SkullPayoutMultiplier = LoseAllCoinsPayoutMultiplier;
inline constexpr int BarPayoutMultiplier = 5;
inline constexpr int SevenPayoutMultiplier = 2;
inline constexpr int CherryPayoutMultiplier = 2;
inline constexpr int LemonPayoutMultiplier = 2;
inline constexpr int OrangePayoutMultiplier = 2;
inline constexpr int BellPayoutMultiplier = 2;

inline constexpr int ScoreRollMultiplier = 1;

} // namespace GameConfig
