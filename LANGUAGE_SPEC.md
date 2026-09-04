weeC Language Specification

Version: 0.1
Date: 2026-09-04

1. Overview

weeC is a small, statically-typed, C-like language designed for education and experimentation. It focuses on a simple, deterministic syntax, a small set of builtin types, lexically scoped identifiers, first-class functions, and a compact runtime implemented with an interpreter. The language trades advanced features for clarity and predictable semantics.

Goals:
- Small, regular grammar suitable for recursive-descent parsing.
- Clear runtime semantics and straightforward mapping to AST/evaluator.
- Predictable type coercion rules and well-defined error cases.

2. Source and encoding
- File encoding: UTF-8 without BOM recommended.
- Line endings: Windows CRLF (\r\n) in this repository; tools should tolerate LF.

3. Lexical structure
- Whitespace separates tokens; newline significant only for diagnostics.
- Tokens: identifiers, literals, operators, punctuation, keywords.
- Identifiers: [A-Za-z_][A-Za-z0-9_]*, may be qualified with scope separators when represented internally.
- Literals:
  - Integer (decimal): e.g. 123
  - Float/double: 1.23 or 1.0 (float and double are distinct builtin types)
  - Char: 'c'
  - String: "text"
  - Boolean keywords: true, false

4. Keywords
Reserved: if, else, while, return, print, struct, namespace, function, true, false, int, uint, float, double, bool, char, string, void, var, const

5. Types
Builtin: int (signed 32-bit), uint (unsigned 32-bit), float (32-bit), double (64-bit), bool, char, string, void
- Types are first-class and must be declared for variables and function return types.
- Type coercion rules (implicit conversions):
  - Integer widening: int -> double, int -> float, uint -> double/float when used with floating operations.
  - Float to double allowed implicitly when required by op.
  - No implicit narrowing (e.g., double -> int) without explicit cast (casts currently unsupported).
  - Boolean conversions: non-zero numeric considered true in boolean contexts (implementation-defined in interpreter; prefer explicit comparisons).

6. Operators and precedence
- Precedence (highest to lowest):
  1. Primary: literals, identifiers, function call
  2. Unary: + - !
  3. Multiplicative: * / %
  4. Additive: + -
  5. Comparison: > >= < <=
  6. Equality: == !=
  7. Logical AND: &&
  8. Logical OR: ||
  9. Ternary: ? :
  10. Assignment: =, +=, -=, *=, /=

- Operators supported: +, -, *, /, %, ==, !=, >, >=, <, <=, &&, ||, !, ++, -- (prefix only), assignment operators (= and compound variants), ternary ?:. 
- Compound-assignment operators: +=, -=, *=, /= implemented with the semantics described below.

7. Expressions
- Primary: literal, identifier, parenthesized expression, function call.
- Call: callee followed by argument list in parentheses. Arguments evaluate left-to-right.
- Unary: prefix operators applied to single operand.
- Binary: left and right subexpressions; evaluation order determined by parse tree and associativity.
- Ternary: condition ? true_expr : false_expr. Condition must evaluate to bool.
- Assignment expressions produce the assigned value (value of RHS after assignment).

Compound-assignment semantics (+=, -=, *=, /=):
- LHS must be an lvalue (a variable identifier). If not, a compile/parse-time error or runtime Expression_NotAnLValue error is produced.
- Evaluation order: evaluate LHS as lvalue (resolve identifier), evaluate RHS expression, read current LHS value, compute binary op (using the existing binary operator semantics, e.g., DoOp for plus), check for runtime errors (e.g., div by zero), then assign the result back into the symbol table under the LHS identifier.
- Example: a += b  => temp = a; result = temp + b; a = result; expression value = result.
- Type compatibility: the binary operation rules apply. If binary op resolves to a compatible builtin type, the assignment stores that value; otherwise, a runtime BadOperation error is returned.

8. Statements
- Declaration: <type> <ident> [= expression];
- Expression statement: expression ;
- If statement: if (expr) stmt [ else stmt ]
- While loop: while (expr) stmt
- Return: return [expr] ;
- Print: print(expr) ; — writes a textual representation to interpreter print handler.
- Block: { statements }

9. Functions
- Declaration: <type> identifier ( type ident, ... ) { body }
- Functions are first-class in symbol resolution via fully qualified names (name + parameter types) to support overloading.
- Calling conventions:
  - Arguments evaluated left-to-right.
  - Arity must match function signature — interpreter enforces argument count and returns a clear error if mismatched.
  - A stack frame is pushed per call; parameters are bound to names in that frame.
  - Return: returns a typed value; void indicates no value.

10. Structs and Namespaces
- struct { ... } declarations create user types with named fields.
- Fields are accessed via qualified identifiers (future work) — presently flat symbol table and simple scoping are supported.
- Namespaces group declarations and avoid name collisions; resolution must yield fully-qualified identifiers.

11. Scoping and symbol resolution
- Lexical scoping: block scopes nest; declarations in outer scopes are visible unless shadowed.
- Resolution algorithm produces a fully-qualified identifier; unresolved or ambiguous identifiers cause parse-time errors.
- Symbol table stores variables, functions and types; the interpreter uses this table for lookup and Set/Get semantics.

12. Runtime semantics and errors
- Known runtime errors:
  - DivByZero: division or modulus by zero.
  - BadOperation: invalid operator application or incompatible types.
  - InvalidNode/BadInput: interpreter internal errors.
  - Expression_NotAnLValue: attempting assignment to non-lvalue.
- Error propagation: operations return a wcInterpreterError object wrapped in std::any; interpreter functions check for this sentinel type and propagate early on error.

13. Standard library
- print(string|any): write textual form to interpreter output handler. Other stdlib functions are minimal; the function table can be extended.

14. Parsing and AST
- Parser: recursive-descent precedence ladder.
- Parse-node types map to expression categories (Expression_Assignment, Expression_Ternary, Expression_LogicOr, Expression_LogicAnd, Expression_Comparison, Expression_Term, Expression_Factor, Expression_Unary, Expression_Call, Expression_Primary, Expression_Operator, etc.).
- Tokens for assignment include specialized token types for compound operators (PlusAssignOperator, MinusAssignOperator, MultAssignOperator, DivAssignOperator) to simplify parsing.

15. Interpreter internals (implementation notes)
- Evaluation flow: an expression interpreter traverses AST nodes and invokes Exec* handlers per node type.
- DoOp dispatch: type-indexed AnyOperator<T1,T2> templates implement binary ops between concrete builtin combinations. Unary ops use AnyOperatorUnary<T>.
- Assignment handling: AssignOperator uses AnyOperator specialization to perform final store using SymTab.Set(). Compound-assignment uses existing binary operator kinds (PlusOperator, MinusOperator, MultiplyOperator, DivideOperator) to compute result and then reuses AssignOperator path to store.
- Function calls: wcInterpreterFunctionTable holds signatures; interpreter pushes wcInterpreterStackFrame with argument values, binds parameters, invokes function body, and restores stack after return.

16. Examples

int a;
a = 3;
a += 2; // a becomes 5, expression value 5

int add(int x, int y) { return x + y; }
int z = add(a, 4);

if (z > 5) print("big"); else print("small");

17. Tooling and build notes
- The repository uses CRLF and historical Visual Studio project metadata. Contributors may retarget .vcxproj PlatformToolset to match available Visual Studio installations.
- Tests reside under weec_test/ and use unit harness files; parser fixtures are in test_listings.h.

18. Undefined/Planned features
- Explicit casts and generics.
- Member access syntax and struct field assignment semantics.
- Full operator overload resolution beyond builtin AnyOperator specializations.
- Enhanced standard library and I/O streams.

19. Conformance and change log
- This document reflects the current interpreter implementation semantics (assignment arity checks, compound assignment semantics added 2026-09-04).
- Future edits should record date and change summary.

Appendix: Token categories
- Operators: +, -, *, /, %, +=, -=, *=, /=, ==, !=, >, <, >=, <=, &&, ||, !, =, ? :
- Punctuation: (, ), {, }, ;, ,, .
- Literals: IntLiteral, FloatLiteral, StringLiteral, CharLiteral, TrueKeyword, FalseKeyword
- Identifiers: Identifier

End of spec.
