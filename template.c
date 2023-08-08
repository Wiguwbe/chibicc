// This file implements a templating "engine"
//
// The engine takes a filename, a map of parameters
// and an output FILE stream
//
// and renders it to the output stream
//
// check TEMPLATE.md for more info

#include "chibicc.h"

/*
  Process:

  1. read whole file (open_memstream)
  2. read/set defaults (check for required, until '%%'
  3. template the rest of the file
*/

// variable holds the template name, needless to say, this is not thread safe
static char *tpl_file;

// reads into NULL terminated string
static char *read_file(char *template_file);
// returns a pointer to the beggining of template, or NULL on error
static char *check_defaults(char *file_contents, HashMap *params);
static char *gen_tpl(char *tpl_ptr, HashMap *params, FILE *output);
static char *do_code(char *tpl_ptr, HashMap *params, FILE *output, long *cond);

int generate_template(char *template_file, HashMap *params, FILE *output)
{
  tpl_file = template_file;
  char *file_contents = read_file(template_file);
  char *tpl_ptr = check_defaults(file_contents, params);
  if(!tpl_ptr)
  {
    free(file_contents);
    return 0;
  }

  tpl_ptr = gen_tpl(tpl_ptr, params, output);

  // return 0 on success, !0 on error
  return *tpl_ptr;
}

static char *read_file(char *template_file)
{
  char *buf;
  size_t buf_size;

  FILE *input = fopen(template_file, "r");
  if(!input)
    error_at(template_file, "failed to open file: %s", strerror(errno));

  FILE *stream = open_memstream(&buf, &buf_size);
  if(!stream)
    error_at(template_file, "failed to create memory stream: %s", strerror(errno));

  for (;;) {
    char _buf[512];
    int n = fread(_buf, 1, sizeof(_buf), input);
    if(!n)
      break;
    fwrite(_buf, 1, n, stream);
  }

  fclose(input);
  fclose(stream);

  // `open_memstream` already places a NULL byte at the end, size can be ignored
  return buf;
}

static char *check_defaults(char *ptr, HashMap *params)
{
  while(*ptr!='@')
  {
    char *key;
    char *val;
    void *cur;
    // skip whitespace
    while(*ptr==' '||*ptr=='\t')
      ptr++;
    if(*ptr=='\n')
      goto _skip_nl;

    if(*ptr=='@')
      break;

    // key
    key = ptr;
    ptr = strchr(ptr, '=');
    if(!ptr)
      error_at(tpl_file, "expected a '=' after key");
    *ptr = 0; // null here
    ptr++;

    cur = hashmap_get(params, key);

    // val
    if(*ptr == '\n')
    {
      // it's required
      if(cur == NULL)
        error_at(tpl_file, "key '%s' is required", key);
      goto _skip_nl;
    }

    val = ptr;
    ptr = strchr(ptr, '\n');
    if(!ptr)
      error_at(tpl_file, "expected a newline after value");
    *ptr = 0;
    if(cur == NULL)
    {
      // values from preprocessor will come as tokens, so we also pass them as tokens
      Token *tk = (Token*)calloc(1, sizeof(Token));
      if(!tk)
        error_at(tpl_file, "failed to allocate memory: %s", strerror(errno));
      tk->kind = TK_IDENT;
      tk->loc = val;
      tk->len = ptr-val;
      hashmap_put(params, key, tk);
    }
    ptr++;

  _skip_nl:
    while(*ptr=='\n') ptr++;
  }

  // ptr at '@'
  return ptr+2;
}

static char *gen_tpl(char *ptr, HashMap *params, FILE *output)
{
  long cond = 3;

  // write until '{{'
  while(*ptr)
  {
    char *c = strstr(ptr, "{{");
    int to_write = c ? c-ptr : strlen(ptr);
    if(cond&1)
      fwrite(ptr, 1, to_write, output);
    if(c)
    	ptr = c;
    else
    {
	    ptr += to_write;
	    break;
	}
    // else, code
    ptr = do_code(c, params, output, &cond);
  }
  return ptr;
}

static int evaluate_true(char *key, int key_len, HashMap *params)
{
  Token *tk = hashmap_get2(params, key, key_len);
  // it should be there
  if(!tk)
    error_at(tpl_file, "unknown variable '%.*s'", key_len, key);

  char *val;
  int val_len;

  switch(tk->kind)
  {
  case TK_IDENT:
  case TK_KEYWORD:
  case TK_PP_NUM:
    val = tk->loc;
    val_len = tk->len;
    break;
  case TK_STR:
    val = tk->str;
    val_len = strlen(val);
    break;
  case TK_NUM:
    // assume integer
    return tk->val;
  default:
    return 0;
  }

  /*
    accepted (str) as true:
    - true
    - yes
    - y
    - 1
  */
  switch(val_len)
  {
  case 4:
    if(!memcmp(val, "true", 4)) return 1;
    break;
  case 3:
    if(!memcmp(val, "yes", 3)) return 1;
    break;
  case 1:
    if(*val=='y' || *val == '1') return 1;
    break;
  }

  // default
  return 0;
}

static char *do_code(char *ptr, HashMap *params, FILE *output, long *cond)
{
  ptr += 2;

  // whitespace
  while(isspace(*ptr)) ptr++;

  char *c = ptr;
  int clen;

  while(!isspace(*ptr)&&*ptr!='}') ptr++;

  clen = ptr-c;

  // keyword?
  if(!memcmp(c, "if", 2) && clen == 2)
  {
    int new_cond = 1; // empty 'if' evaluates to true
    while(new_cond)
    {
      // look for variable name
      char *n;
      int neg = 0;
    _ws:
      while(isspace(*ptr)) ptr++;
      if(*ptr == '}') break;
      if(*ptr == '!')
      {
        neg = 1;
        ptr++;
        goto _ws;
      }
      n = ptr;
      while(!isspace(*ptr)&&*ptr!='}') ptr++;
      new_cond &= neg ^ evaluate_true(n, ptr-n, params);
    }
    // ignore until '}}'
    // execute
    ptr = strstr(ptr, "}}");
    if(!ptr)
      error_at(tpl_file, "expected '}}'");
    ptr+=2;
    long prev_cond = *cond;
    *cond <<= 2;
    *cond |= (new_cond << 1) | (new_cond&prev_cond&1);
    return ptr;
  }
  if(!memcmp(c, "else", 4) && clen == 4)
  {
    // ignore until '}}'
    ptr = strstr(ptr, "}}");
    if(!ptr)
      error_at(tpl_file, "expected '}}'");
    ptr += 2;
    long new_cond = ((*cond & 2)>>1)^1;
    long prev_cond = (*cond)>>2;
    *cond &= (long)(-4);  // clear this cond
    *cond |= (new_cond << 1) | (new_cond&prev_cond&1);
    return ptr;
  }
  if(!memcmp(c, "end", 3) && clen == 3)
  {
    *cond >>= 2;
    ptr = strstr(ptr, "}}");
    if(!ptr)
      error_at(tpl_file, "expected '}}'");
    return ptr+2;
  }

  // else, simple variable
  if((*cond)&1)
  {
    // print it
    Token *tk = hashmap_get2(params, c, clen);
    if(!tk)
      error_at(tpl_file, "unknown variable '%.*s'", clen, c);
    switch(tk->kind)
    {
    case TK_IDENT:
    case TK_KEYWORD:
    case TK_PP_NUM:
      fprintf(output, "%.*s", tk->len, tk->loc);
      break;
    case TK_STR:
      fprintf(output, "%s", tk->str);
      break;
    case TK_NUM:
      fprintf(output, "%ld", tk->val);
      break;
    default:
      error_at(tpl_file, "unknown variable type");
    }
  }
  ptr = strstr(ptr, "}}");
  return ptr+2;
}
