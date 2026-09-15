#pragma once

namespace GameConfig {

inline constexpr int StartingCoins = 100;
inline constexpr int MinimumBet = 1;
inline constexpr int MaximumBet = StartingCoins;

// Gross payouts are rounded down to whole coins. -1.0F means lose all coins.
inline constexpr float TwoMatchingPayoutMultiplier = 1.0F;
inline constexpr float LoseAllCoinsPayoutMultiplier = -1.0F;
inline constexpr float SkullPayoutMultiplier = LoseAllCoinsPayoutMultiplier;
inline constexpr float BarPayoutMultiplier = 20.0F;
inline constexpr float SevenPayoutMultiplier = 10.0F;
inline constexpr float CherryPayoutMultiplier = 1.0F;
inline constexpr float LemonPayoutMultiplier = 0.5F;
inline constexpr float OrangePayoutMultiplier = 1.5F;
inline constexpr float BellPayoutMultiplier = 5.0F;

// When enabled, each roll chooses one random symbol and repeats it on all reels.
inline constexpr bool RiggedMode = false;

inline constexpr int ScoreRollMultiplier = 1;

} // namespace GameConfig
