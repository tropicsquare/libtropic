# Logging
Libtropic contains a logging functionality, which is disabled by default (unless you compile *tests* or *examples*).

You may find it useful to enable logging during libtropic evaluation or integration. There are five logging levels:

- none (default unless compiling *tests* or *examples*),
- error,
- warning,
- info,
- debug.

One of these logging levels can be switched on using the CMake switch `-DLT_LOG_LVL`, which is passed to `cmake` when building the project. The following table summarizes all logging levels and a switch corresponding to each level:

| Level   | CMake switch           |
|---------|------------------------|
| none    | `-DLT_LOG_LVL=None`    |
| error   | `-DLT_LOG_LVL=Error`   |
| warning | `-DLT_LOG_LVL=Warning` |
| info    | `-DLT_LOG_LVL=Info`    |
| debug   | `-DLT_LOG_LVL=Debug`   |

## How to Log
Logging can be done using *logging macros*, which are defined in `include/libtropic_logging.h`. Following logging macros are available:

- `LT_LOG_INFO`,
- `LT_LOG_WARN`,
- `LT_LOG_ERROR`,
- `LT_LOG_DEBUG`.

Each macro corresponds to a verbosity level, which is activated with the aforementioned CMake switch. Macros have the same interface as the `printf` function, as they are essentially a wrapper over `printf`.

!!! important
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

!!! note
    There are also macros used for assertion. These are used in functional tests.