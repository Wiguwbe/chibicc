# Methods feature

_a fun POC_

Struct functions, similar to C2 (or C3?).

This feature allows to associate a function with a struct, giving a better
understanding of the relationship between functions and datatypes.

Only structs can be used with methods.

### Syntax

```
DECLSPEC TAG "." FUNC_NAME "(" POINTERS IDENT (, ...)? ")" ...
```

Example:
```
int Point.dot(*p) { ...
int foo.bar(f, baz) { ...
```

The first argument will be of the struct type, so the first argument doesn't
need the type, only the pointers and name.

### Internals

Methods will be renamed to be compiled,

given TAG (the struct name) and FUNC_NAME, the final name will be:
`__<TAG>_<FUNC_NAME>`.

When calling a method, if the first argument has pointers, pointers
(get address, `&`) will be automatically added before calling.

See under tests for a small complete example.
