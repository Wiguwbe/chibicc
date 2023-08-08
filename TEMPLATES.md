# Template directive for preprocessor

A simple template engine allows to generate parameterized templates into the
source file.

## Template

```
<keys and defaults>
@@
<the template>
```

### keys and defaults

The template must provide the keys/names it will use, optionally providing a default.

A call to the template that does not provide a required value will fail.

To provide a default value:

```
key=default
another_key=another_default_value
```

And to make it required (leave the value empty):

```
required=
```

### the template

The template has text (directly output to the source) and _code_.

Code is surrounded by `{{` and `}}` (_very original_).

It is used to output the value of variables/keys or `if..else` logic.

An example:
```
key=value
do_the_thing=yes
revert=no
@@

the key provided is "{{ value }}"

{{ if do_the_thing }}
the thing is done
{{ else }}
nope
{{ end }}

{{if ! revert }}
not reverted
{{ end }}
```

## Preprocessor directive

To invoke a template, an interface similar to the `#include` was provided:

```
// ...

#template "template_name" param1=value1 param2=value2 ...

// or

#template <global_template> param1=value1 ...

// ...
```

Similarily to the `#include` directive, if double quotes are provided, it will search first
in the current directory.
