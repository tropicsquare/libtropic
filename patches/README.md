# Security Audit Patches

This directory contains patches for security issues identified in the comprehensive security audit performed on 2025-11-17.

## Critical Patches (MUST apply before release)

### 0001-LTAUD-CRIT-001-secure-memory-zeroization.patch
**Severity:** Critical  
**CWE:** CWE-14  
**Issue:** Non-volatile memory clearing of sensitive cryptographic material

Implements `lt_secure_zero()` function using volatile pointers or platform-specific secure clearing functions (`explicit_bzero()`, `memset_s()`, `SecureZeroMemory()`).

**Impact:** Prevents compiler from optimizing away clearing of ephemeral keys and IVs.

**Files Modified:**
- Creates: `src/lt_secure_memory.c`, `src/lt_secure_memory.h`
- Modifies: `src/libtropic.c`, `src/lt_l3_process.c`, `include/libtropic_common.h`

**CMakeLists.txt Update Required:** Add `src/lt_secure_memory.c` to SDK_SRCS

## High Priority Patches (Should apply before release)

### 0002-LTAUD-HIGH-001-replace-strcpy-with-snprintf.patch
**Severity:** High  
**CWE:** CWE-676  
**Issue:** Use of unsafe string function strcpy()

Replaces 3 instances of `strcpy()` with bounds-checked `snprintf()`.

**Files Modified:** `src/libtropic.c`

### 0003-LTAUD-HIGH-002-replace-sprintf-with-snprintf.patch
**Severity:** High  
**CWE:** CWE-676  
**Issue:** Use of unsafe function sprintf()

Replaces `sprintf()` with `snprintf()` and adds return value checking.

**Files Modified:** `src/libtropic.c`

### 0004-LTAUD-HIGH-003-fix-integer-overflow-fw-update.patch
**Severity:** High  
**CWE:** CWE-190  
**Issue:** Potential integer overflow in firmware update size calculation

Adds check for `len == 255` and promotes to `size_t` before addition.

**Files Modified:** `src/libtropic.c`

## Applying Patches

### Option 1: Apply all critical/high patches

```bash
cd /path/to/libtropic
git apply patches/0001-LTAUD-CRIT-001-secure-memory-zeroization.patch
git apply patches/0002-LTAUD-HIGH-001-replace-strcpy-with-snprintf.patch
git apply patches/0003-LTAUD-HIGH-002-replace-sprintf-with-snprintf.patch
git apply patches/0004-LTAUD-HIGH-003-fix-integer-overflow-fw-update.patch

# Don't forget to update CMakeLists.txt to add lt_secure_memory.c to SDK_SRCS
```

### Option 2: Apply individually

```bash
git apply patches/0001-LTAUD-CRIT-001-secure-memory-zeroization.patch
```

### Option 3: Create fix branch

```bash
git checkout -b audit/fix-20251117
git am patches/*.patch
```

## Testing After Applying Patches

1. **Build with strict flags:**
   ```bash
   mkdir build && cd build
   cmake .. -DLT_STRICT_COMPILATION=ON
   cmake --build .
   ```

2. **Verify secure zeroing:**
   - Build with `-O2` or `-O3`
   - Use `objdump -d` to verify `lt_secure_zero()` calls are not eliminated
   - Test with memory analysis tools

3. **Run functional tests:**
   - Follow instructions in `docs/for_contributors/functional_tests.md`

4. **Verify firmware update:**
   - Test firmware update with various chunk sizes
   - Specifically test edge case with chunk length approaching 255

## Additional Recommendations (not in patches)

See `audit-report.md` for:
- LTAUD-MED-001: Add thread-safety documentation to API
- LTAUD-MED-002: Resolve TODO comment in libtropic_l3.c
- General recommendations for CI/CD improvements

## Questions or Issues

Refer to the full audit report in `audit-report.md` or `audit-report.json`.
