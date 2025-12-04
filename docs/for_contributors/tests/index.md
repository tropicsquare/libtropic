# Tests
We implement two groups of tests:

- [Functional](./functional_tests.md) (which use the standard HAL),
- [Functional Mock](./functional_mock_tests.md) (which use the mock HAL).

Functional tests are compatible with all standard HALs and are used to test Libtropic implementation on both model and real hardware host platforms.

Functional mock tests are used with mock HAL which allows us to mock communication at the lowest level. They are useful to test functionality not easily triggered on neither model nor the real TROPIC01 chips, such as rare error codes and other special conditions.

We also support measuring combined code coverage of both groups: see [Code Coverage](./code_coverage.md).