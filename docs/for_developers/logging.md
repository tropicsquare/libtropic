# Logging
We log using our logging macros defined in `include/libtropic_logging.h`. Those macros
are useful mainly in tests and examples. Following macros are available:

- `LT_LOG_INFO`,
- `LT_LOG_WARN`,
- `LT_LOG_ERROR`,
- `LT_LOG_DEBUG`.

Each macro correspond to a verbosity level, which can be set using a CMake switch (see [Index](./index.md)). Macros have the same interface as the `printf` function, as they are essentially a wrapper over `printf`.

!!! important
    Avoid passing function calls as macro arguments (except for simple formatting helpers like `lt_ret_verbose` or `strerror`).
    Logging macros may be completely removed at lower verbosity levels, meaning any function calls inside them will **not** execute.

This is safe (formatting helper only):

```c
LT_LOG_INFO("Error code: %d, error string: %s", ret, lt_ret_verbose(ret));
```

This is unsafe — `lt_init` will never run if logging is disabled:

```c
LT_LOG_INFO("Initializing handle: %d", lt_init(&h));
```

Correct approach — call the function first, then log its result:

```c
int ret = lt_init(&h);
LT_LOG_INFO("Initializing handle: %d", ret);
```

!!! note
    There are also macros used for assertion. These are used in functional tests.