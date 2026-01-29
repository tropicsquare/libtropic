# Logging
Libtropic contains a logging functionality, which is disabled by default.

You may find it useful to enable logging during libtropic evaluation or integration. There are five logging levels:

- None (default unless compiling *tests* or *examples*),
- Error,
- Warning,
- Info,
- Debug.

One of these logging levels can be switched on using the [LT_LOG_LVL](integrating_libtropic/how_to_configure/index.md#lt_log_lvl) CMake option (refer to [How to Configure](integrating_libtropic/how_to_configure/index.md) section for ways to set it).

## How to Log
Logging can be done using *logging macros*, which are defined in `include/libtropic_logging.h`. Following logging macros are available:

- `LT_LOG_INFO`,
- `LT_LOG_WARN`,
- `LT_LOG_ERROR`,
- `LT_LOG_DEBUG`.

Each macro corresponds to a verbosity level, which is activated with the aforementioned CMake switch. Macros have the same interface as the `printf` function, as they are essentially a wrapper over `printf`.

!!! warning "Function Calls as LT_LOG_* Arguments"
    Avoid passing function calls as macro arguments (except for simple formatting helpers like `lt_ret_verbose` or `strerror`).
    Logging macros may be completely removed at lower verbosity levels, meaning any function calls inside them will **not** execute.

This is safe (using `lt_ret_verbose()` helper function only):

```c
LT_LOG_INFO("Error code: %d, error string: %s", ret, lt_ret_verbose(ret));
```

This is unsafe — `lt_init()` will never run if logging is disabled:

```c
LT_LOG_INFO("Initializing handle: %d", lt_init(&h));
```

Correct approach — call the function first, then log its result:

```c
int ret = lt_init(&h);
LT_LOG_INFO("Initializing handle: %d", ret);
```

!!! info "Other Macros"
    There are also macros used for assertion. These are used in [Functional Tests](../for_contributors/tests/functional_tests.md).