- [1. Intro](#1-intro)
- [2. Variable Names](#2-variable-names)
  - [Examples](#examples)
- [3. Function Names](#3-function-names)
  - [Examples](#examples-1)
- [4. File Names](#4-file-names)
  - [Examples](#examples-2)
- [5. Type Names](#5-type-names)
  - [Examples](#examples-3)
- [6. Constants and Macros](#6-constants-and-macros)
  - [Examples](#examples-4)
- [7. Summary Table](#7-summary-table)


# 1. Intro
This document outlines the naming conventions for C++ projects to ensure consistency and readability across the codebase. Each rule is accompanied by examples for clarity.

`src/util/test` does not strictly follow these rules temporarily.

# 2. Variable Names
- **General Variables**: Use camelCase.
- **Member Variables**: Prefix with `m_`.
- **Static Member Variables**: Prefix with `ms_`.
- **Static Variables**: Prefix with `s_`.
- **Global Variables**: Prefix with `g_`.
- **Static Global Variables**: Prefix with `sg_`.
- **Process-Shared Global Variables**: Prefix with `gg_`.

## Examples
```cpp
int localVariable = 0; // General variable
int m_memberVariable;  // Member variable
static int s_staticVariable; // Static variable
int g_globalVariable; // Global variable
static int sg_staticGlobalVariable; // Static global variable
int gg_sharedGlobalVariable; // Process-shared global variable
```

# 3. Function Names
Use lowercase words separated by underscores `(_)`.

## Examples
```cpp
void initialize_app();
int calculate_sum(int a, int b);
```

# 4. File Names
Use lowercase words separated by underscores `(_)`.

## Examples
```arduino
// File names
user_manager.cpp
data_parser.h
```

# 5. Type Names
- Use PascalCase (UpperCamelCase).
- Classes: Prefix with `C`.
- Structures: Prefix with `S`.
- Interfaces (Abstract Classes): Prefix with `I`.
- Enumerations: Prefix with `E`.

## Examples
```cpp
class CUserManager; // Class
struct SDataInfo;   // Structure
class IFileHandler; // Interface
enum class EStatus { Success, Failure }; // Enumeration
```

# 6. Constants and Macros
Use all uppercase letters separated by underscores `(_)`.

## Examples
```cpp
const int MAX_BUFFER_SIZE = 1024; // Constant
#define ERROR_CODE 1              // Macro
```

# 7. Summary Table
| **Element**                 | **Naming Style**             | **Example**               |
| --------------------------- | ---------------------------- | ------------------------- |
| **Local Variables**         | camelCase                    | `localVariable`           |
| **Member Variables**        | camelCase with `m_` prefix   | `m_memberVariable`        |
| **Static Variables**        | camelCase with `s_` prefix   | `s_staticVariable`        |
| **Static Member Variables** | camelCase with `ms_` prefix  | `ms_staticMemberVariable` |
| **Global Variables**        | camelCase with `g_` prefix   | `g_globalVariable`        |
| **Static Global Variables** | camelCase with `sg_` prefix  | `sg_staticGlobalVariable` |
| **Process-Shared Globals**  | camelCase with `gg_` prefix  | `gg_sharedGlobalVariable` |
| **Functions**               | lowercase with `_` separator | `initialize_app()`        |
| **File Names**              | lowercase with `_` separator | `user_manager.cpp`        |
| **Classes**                 | PascalCase with `C` prefix   | `CUserManager`            |
| **Structures**              | PascalCase with `S` prefix   | `SDataInfo`               |
| **Interfaces**              | PascalCase with `I` prefix   | `IFileHandler`            |
| **Enumerations**            | PascalCase with `E` prefix   | `EStatus`                 |
| **Constants/Macros**        | Uppercase with `_` separator | `MAX_BUFFER_SIZE`         |
