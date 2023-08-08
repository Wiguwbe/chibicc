key=default
integer=
default=y
value=50
default_value=50

@@

char *key = "{{key}}";
int integer = {{integer}};

{{ if default }}
int default_value = {{default_value}};
{{ else }}
int default_value = {{value}};
{{ end }}
