# Methods feature

_a fun POC_

Struct functions, similar to Go's methods.

This feature allows to associate a function with a struct, giving a better
understanding of the relationship between functions and datatypes.

Only structs can be used with methods.

### Syntax

```
DECLSPEC (STRUCT? TAG POINTERS IDENT) FUNC_NAME(...) ...
```

Example:
```
int (struct Point *p) dot() ...
int (struct foo f) bar(int baz) { ...
```

The "argument" inside the parenthesis will also be the first argument
to the function.

When calling a method, the value will be used as the first argument
of the function.

### Internals

Methods will be renamed to be compiled,

given TAG (the struct name) and FUNC_NAME, the final name will be:
`__<TAG>_<FUNC_NAME>`.

When calling a method, if the first argument has pointers, pointers
(get address, `&`) will be automatically added before calling.

See under tests for a small complete example.
