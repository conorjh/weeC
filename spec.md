# weeC language reference

This document describes the language currently implemented by the lexer and parser in weeC. It is intentionally descriptive rather than aspirational: when behavior is not implemented, this spec says so.

## Overview

weeC is a small, statically scoped language with declarations, expressions, blocks, control flow, functions, and structs. Parsing is syntax- and scope-aware: the parser resolves names while it parses, and it rejects declarations or references that cannot be resolved in the current scope model.

The current parser supports:

- top-level declarations and statements
- local declarations inside blocks
- function declarations and definitions
- struct declarations
- `if`, `while`, `return`, `print`, and empty statements
- expression statements terminated by `;`
- assignment, ternary, logical, comparison, arithmetic, unary, call, and grouping expressions

The current parser does not implement a full type checker, member-access semantics, or full namespace declaration syntax. Some keywords are tokenized but not yet accepted by the parser.

## Lexical structure

The lexer classifies source text into tokens for literals, identifiers, operators, punctuation, whitespace, newlines, comments, and reserved words.

Whitespace and newlines are tokenized and consumed by the parser as needed. Comments are tokenized by the lexer and are not part of the parsed AST.

### Token categories

- literals: integer, floating-point, string, and character literals
- identifiers
- keywords and built-in type keywords
- operators
- punctuation
- end-of-stream / end-of-file markers

### Punctuation and delimiters

The parser currently relies on these delimiters:

- scope delimiter: `::`
- member delimiter: `.`
- global identifier prefix: `$g::`

The scope delimiter is part of identifier qualification. The member delimiter is tokenized, but member access is not implemented as a parser feature in the current implementation.

### Whitespace and comments

Whitespace and newline tokens are recognized by the lexer. The parser generally ignores them between syntactic constructs.

Single-line and multi-line comments are tokenized by the lexer. Comments do not form syntax nodes.

### Operators

The lexer recognizes the following operators:

- arithmetic: `+`, `-`, `*`, `/`, `%`, `**`
- increment/decrement: `++`, `--`
- assignment: `=`, `+=`, `-=`, `*=`, `/=`
- comparison: `>`, `<`, `>=`, `<=`, `==`, `!=`
- logical: `!`, `||`, `&&`
- ternary: `? :`

Of these, the parser currently uses:

- unary `+`, `-`, `!`
- binary `+`, `-`, `*`, `/`
- comparison `>`, `<`, `>=`, `<=`
- equality `==`, `!=`
- logical `&&`, `||`
- assignment `=`
- ternary `?:`

The lexer tokenizes additional operator forms, but the parser does not currently assign them expression semantics.

## Keywords

The lexer reserves the following keywords:

- value keywords: `true`, `false`
- built-in type keywords: `string`, `double`, `int`, `uint`, `float`, `bool`, `char`, `void`
- other reserved words: `object`, `var`, `namespace`, `func`, `if`, `else`, `while`, `break`, `continue`, `return`, `inline`, `const`, `print`, `struct`

### Parser support status

Supported today:

- `if`
- `else`
- `while`
- `return`
- `print`
- `struct`

Tokenized but not currently used as standalone parser constructs:

- `namespace`
- `func`
- `break`
- `continue`
- `inline`
- `const`
- `object`
- `var`

## Literals

The parser currently accepts these literal forms in expressions:

- integer literals
- floating-point literals
- string literals
- character literals
- `true`
- `false`

Literal expressions are parsed as primary expressions. The parser does not currently perform constant folding or literal type conversion beyond attaching the literal’s built-in type where applicable.

### Examples

```weeC
1
3.14
"hello"
'a'
true
false
```

## Identifiers

Identifiers are parsed from lexer `Identifier` tokens.

### Valid forms

The identifier helper accepts:

- simple identifiers such as `x` or `MyType`
- qualified identifiers using `::`
- globally qualified identifiers using `$g::`
- function identifiers with a parenthesized parameter-type list

### Examples

```weeC
x
math::Vector
$g::int
print(string)
```

### Resolution rules

The parser resolves identifiers during parsing. Resolution is scope-based and can report:

- unresolved identifier
- ambiguous identifier
- redeclaration
- unresolved type
- ambiguous type

For declarations, the parser expects the name not to already be present in the current resolution context. For expressions, the parser expects identifiers to already resolve to a declared symbol.

### Current limitations

- member-style identifiers using `.` are tokenized, but member resolution is not implemented in the current parser
- full namespace declaration syntax is tokenized but not parsed as a language construct
- function names are internally normalized with their parameter-type list, but overload resolution remains parser-side bookkeeping rather than semantic dispatch

## Namespacing

weeC uses a scope-qualified naming model.

### Global namespace prefix

The special global identifier prefix is `$g::`.

Examples:

```weeC
$g::int
$g::math::Vector
```

The parser predefines `$g` as the root scope and registers built-in types under that scope.

### Scope delimiter

The scope delimiter is `::`.

It is used to qualify names into nested scopes, such as:

```weeC
math::Vector
outer::inner::Thing
```

### Member delimiter

The member delimiter is `.`.

It is recognized by the lexer, but current parser behavior does not implement member access or member lookup semantics.

### Scope model

The parser maintains a stack of scopes while parsing nested declarations and blocks. A name is resolved by searching available scopes and symbol tables according to the current parser state.

## Type system

The current parser is aware of a small set of built-in types and user-defined struct types.

### Built-in types

Built-in types currently registered by the parser are:

- `void`
- `int`
- `uint`
- `double`
- `float`
- `bool`
- `string`

These are available under the global scope as `$g::void`, `$g::int`, and so on.

### User-defined types

`struct` declarations register a type symbol that can be used as a declaration type after the struct is parsed.

### Type usage

Types are currently used in:

- variable declarations
- function return types
- function parameter types
- struct declarations

The parser does not currently implement conversion rules, numeric promotion, or a deeper semantic type checker.

## Declarations

The parser currently accepts declarations in these forms:

```weeC
int x;
int x = 1;
int add(int a, int b);
int add(int a, int b) { return a + b; }
struct Point { int x; int y; };
```

### Variable declarations

A variable declaration has a type and an identifier.

An initializer is optional:

```weeC
int answer;
int answer = 42;
```

If an initializer is present, it must be followed by `;`.

### Function declarations and definitions

A function declaration is parsed when a typed identifier is followed by `(`.

```weeC
int add(int a, int b);
int add(int a, int b) { return a + b; }
```

A function may end with a semicolon or continue into a block body.

Parameters are typed and named. Parameter identifiers are scoped as part of the function’s qualified identifier bookkeeping.

#### Current implementation quirk

The parser rewrites the internal function identifier to include its parameter-type list. This is how function signatures are stored and resolved today.

### Struct declarations

Struct declarations are parsed as a distinct declaration kind:

```weeC
struct Point {
    int x;
    int y;
};
```

A struct body currently accepts declarations. The parser closes the struct with `}` followed by `;`.

#### Current implementation quirk

Struct member semantics are minimal. The parser records the declaration structure, but it does not implement rich member binding or member lookup behavior.

## Statements

The parser accepts the following statement forms today:

- declaration statements
- expression statements terminated by `;`
- empty statements (`;`)
- `if` statements
- `while` statements
- `return` statements
- `print` statements
- `struct` declarations where declarations are permitted

### Examples

```weeC
x = 1;
;
if (x > 0) x = 1; else x = 2;
while (x < 10) { x = x + 1; }
return;
print(x);
```

### Declarations in statements

Declarations are allowed at top level and inside blocks when the parser is configured to allow them. Some nested constructs, such as the bodies of `if` and `while`, explicitly disable declarations in the current implementation.

## Blocks

Blocks are delimited by braces:

```weeC
{
    int x = 1;
    x = x + 1;
}
```

A block contains zero or more statements until a closing `}` is found.

### Current implementation quirk

If the closing brace is missing, the parser reports `MissingClosingBrace`.

## Control flow

### If statements

Syntax:

```weeC
if (condition) statement
if (condition) { statements } else statement
```

The condition is a parenthesized expression.

The `then` branch may be either a single statement or a block. The `else` branch, if present, uses the same forms.

#### Current implementation quirk

The parser does not currently enforce declaration allowance inside `if` branches; it parses branch bodies with declarations disabled.

If the closing `)` after the condition is missing, the parser reports `If_MissingClosingParenthesis`.

### While statements

Syntax:

```weeC
while (condition) statement
while (condition) { statements }
```

The condition is a parenthesized expression.

If the closing `)` after the condition is missing, the parser reports `While_MissingClosingParenthesis`.

#### Current implementation quirk

Like `if`, `while` currently parses its body with declarations disabled.

### Return statements

Syntax:

```weeC
return;
return expression;
```

A `return` may be empty or carry a single expression.

### Print statements

Syntax:

```weeC
print(expression);
```

`print` takes one expression followed by `;`.

### Break and continue

`break` and `continue` are reserved keywords, but the parser does not currently implement them as statements.

## Expressions

Expressions are parsed with a recursive descent parser using precedence levels.

### Expression forms

Supported expression forms today are:

- literals
- variables
- unary expressions
- function calls
- binary expressions
- logical expressions
- grouped expressions
- ternary expressions
- assignment expressions

### Examples

```weeC
1 + 2 * 3
(a + b) * c
!ready
foo(bar, baz)
x = y = 10
cond ? left : right
```

### Primary expressions

Primary expressions currently include:

- literals
- identifiers that resolve to variables or functions
- grouped expressions in parentheses

An identifier used in an expression must resolve to an existing symbol. Otherwise the parser reports `UndeclaredIdent` or a related resolution error.

### Function calls

A call expression is formed by a callee followed immediately by `(` and a comma-separated argument list.

```weeC
foo()
foo(1, 2, 3)
math::max(a, b)
```

The parser currently enforces a maximum of 255 arguments.

#### Current implementation quirks

- if a callee is not followed by `(`, the parser treats it as a non-call primary expression
- missing closing `)` in a call reports `Expression_FunctionCallMissingOpenParenthesis`
- arity checking is not implemented yet; the parser currently only counts arguments and caps the maximum

### Unary expressions

Supported unary operators are:

- `+`
- `-`
- `!`

Example:

```weeC
!-x
```

### Assignment expressions

Assignment currently supports `=`.

The left-hand side must parse as a variable expression. Otherwise the parser reports `Expression_NotAnLValue`.

Assignment is right-associative:

```weeC
a = b = c
```

### Ternary expressions

The parser supports the conditional operator:

```weeC
condition ? a : b
```

The middle and right operands are parsed as full expressions.

### Binary and logical expressions

Supported binary and logical operators:

- logical OR: `||`
- logical AND: `&&`
- equality: `==`, `!=`
- comparison: `>`, `<`, `>=`, `<=`
- additive: `+`, `-`
- multiplicative: `*`, `/`

Operator precedence, from lowest to highest:

1. assignment
2. ternary
3. logical OR
4. logical AND
5. equality
6. comparison
7. additive
8. multiplicative
9. unary
10. call / primary

Binary operators are parsed left-to-right within each precedence level.

#### Current implementation quirk

The lexer recognizes exponentiation and compound assignment operators, but the current expression parser does not implement them.

## Function calls

Function calls are parsed as a postfix form on a primary expression.

```weeC
log(message)
sum(a, b, c)
```

The callee must already resolve to a callable symbol in the current parser model.

Function signatures are stored using their parameter-type list as part of the identifier model.

## Structs

Struct declarations introduce a named type in the current scope.

```weeC
struct Vec2 {
    float x;
    float y;
};
```

A struct body currently accepts declarations only. The parser does not yet implement nested methods, access modifiers, inheritance, or layout rules.

## Symbol resolution and scope

The parser uses a symbol table plus a stack of lexical scopes.

### Scope behavior

- the global scope exists as `$g`
- each parsed block uses a nested scope context
- function declarations register a function symbol and a function-scope context
- struct declarations register a type symbol and a struct scope context

### Resolution behavior

Resolution is performed during parsing rather than deferred.

The parser can report:

- `IdentRedeclaration`
- `UndeclaredIdent`
- `DeclarationsProhibited`
- `CouldntResolveType`
- `AmbiguousType`
- `CouldntResolveIdentifier`
- `AmbiguousIdentifier`

### Identifier normalization

The parser may normalize identifiers by stripping the global prefix or argument list when comparing or displaying them.

This affects how the parser stores and compares function identifiers and fully-qualified names, but source syntax still uses the explicit forms shown above.

## Error handling

The parser returns structured parse errors instead of throwing exceptions.

### Parser errors currently produced

- `UnexpectedToken`
- `InvalidType`
- `If_MissingClosingParenthesis`
- `MissingClosingBrace`
- `While_MissingClosingParenthesis`
- `IdentRedeclaration`
- `UndeclaredIdent`
- `DeclarationsProhibited`
- `CouldntResolveType`
- `AmbiguousType`
- `CouldntResolveIdentifier`
- `AmbiguousIdentifier`
- `UnexpectedEOF`
- `Expression_Empty`
- `Expression_UnexpectedToken`
- `Expression_MissingClosingParenthesis`
- `Expression_UnexpectedEOF`
- `Expression_NotAnLValue`
- `Expression_FunctionCallMissingOpenParenthesis`
- `FunctionCall_MaxArgumentsExceeded`

### Error propagation

Parsing stops at the first unrecoverable error in the current parse path and returns the partial AST alongside the error.

### Syntax examples that fail today

```weeC
x = ;
if (x > 0 { x = 1; }
return value
foo(1, 2, 3, ... 256 args ...)
```

These examples illustrate the kinds of errors the parser currently reports, not a complete diagnostic catalog.