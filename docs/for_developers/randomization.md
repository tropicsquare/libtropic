# Randomization
!!! bug
    Redo this? This does not talk about the randomization for functional testing.

Some tests use a rudimentary randomization mechanism using standard C functions. The PRNG is normally
seeded with current time. Used seed is always printed to stdout. You can find the seed in logs
(`build/gcov/results/...`).

To run tests with fixed seed, set `RNG_SEED` parameter to your
desired seed (either directly in the file, or in the project.yml section `:defines:`). This is
useful mainly to replicate a failed test run -- just find out what seed was used and then
set `RNG_SEED` to this.