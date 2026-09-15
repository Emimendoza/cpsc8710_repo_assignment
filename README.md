# Lucky Seven Slots

A small Qt 6 Widgets slot machine game with configurable rules, animated reels,
keyboard controls, and deterministic game-logic tests.

## Build and run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/qt_slots
```

Qt 6 with the Widgets and Test components is required.

## Controls

- Pull the lever with a click, Space/Enter while focused, or `Alt+P`.
- Change the bet with the on-screen arrows or the keyboard Up/Down keys.

## Tests

```sh
ctest --test-dir build --output-on-failure
```
