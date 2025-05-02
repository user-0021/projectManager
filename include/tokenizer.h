#pragma once

typedef struct _TOKEN TOKEN;

//token type
typedef enum{
	TOKEN_STR,
	TOKEN_RESURVED,
	TOKEN_PUN,
	TOKEN_EOF
} TOKEN_TYPE;

//token
typedef struct _TOKEN
{
	TOKEN_TYPE type;
	TOKEN* next;
	char* str;
} TOKEN;

TOKEN* tokenize(char* str);