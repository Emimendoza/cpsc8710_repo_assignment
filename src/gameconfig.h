#pragma once

namespace GameConfig {

inline constexpr int StartingCoins = 100;
inline constexpr int MinimumBet = 1;
inline constexpr int MaximumBet = StartingCoins;

// Gross payouts are rounded down to whole coins. -1.0F means lose all coins.
inline constexpr float TwoMatchingPayoutMultiplier = 1.0F;
inline constexpr float LoseAllCoinsPayoutMultiplier = -1.0F;
inline constexpr float SkullPayoutMultiplier = LoseAllCoinsPayoutMultiplier;
inline constexpr float BarPayoutMultiplier = 5.0F;
inline constexpr float SevenPayoutMultiplier = 2.0F;
inline constexpr float CherryPayoutMultiplier = 2.0F;
inline constexpr float LemonPayoutMultiplier = 2.0F;
inline constexpr float OrangePayoutMultiplier = 2.0F;
inline constexpr float BellPayoutMultiplier = 2.0F;

// When enabled, each roll chooses one random symbol and repeats it on all reels.
inline constexpr bool RiggedMode = false;

inline constexpr int ScoreRollMultiplier = 1;

} // namespace GameConfig
