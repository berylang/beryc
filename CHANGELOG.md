# Changelog

## [1.1.0] - 2026-11-11
### Added
- Full UTF-8 support: UTF-8 source files (BOM accepted, invalid bytes rejected), Unicode identifiers, Unicode strings, and `\u{XXXX}` escapes.
- `char` is now a Unicode code point (32-bit). `len()`, indexing, `substr()` and `for-in` over strings work on code points.
- UTF-8 aware `print()`/`inputChar()` and UTF-8 console mode on Windows.
- Diagnostics columns and carets are measured in characters.
- New diagnostics ERROR118-ERROR121.

### Changed
- `char` ABI changed from `i8` to `i32`.


## [1.0.0] - 2026-10-04
### Added
- First stable release of the Bery language.
- Features - Compiler, BRE (runtime environment), standard libraries, Garbage Collectors.

### Known issues
- Some bugs related to the IR generation are their.

[1.0.0]: https://github.com/berylang/berc/releases/tag/v1.0.0
