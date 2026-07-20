#include "pch.h"
#include "platform.h"

#include <strg.h>

#define DEFAULT_EXPAND 100

String* String_init()
{
	String* s = (String*)malloc(sizeof(String));
	assert(s != NULL);

	s->_len = strlen("");
	s->_expand = DEFAULT_EXPAND;
	s->_capacity = s->_expand + 1;
	s->_index = 0;

	s->_str = (char*)malloc(sizeof(char) * (s->_capacity));
	assert(s->_str != NULL);

	strncpy(s->_str, "", s->_capacity);

	return s;
}

String* String_init_v2(size_t expand, size_t count)
{
	String* s = (String*)malloc(sizeof(String));
	assert(s != NULL);

	s->_len = strlen("");
	s->_expand = expand;
	s->_capacity = s->_expand * count + 1;
	s->_index = 0;

	s->_str = (char*)malloc(sizeof(char) * (s->_capacity));
	assert(s->_str != NULL);

	strncpy(s->_str, "", s->_capacity);

	return s;
}

void String_free(String* s)
{
	if (s->_str != NULL)
		free(s->_str);
	free(s);
}

void String_cpy(String* s, const char* cstr)
{
	size_t len = strlen(cstr);
	size_t cap = (len / s->_expand + ((len % s->_expand > 0) ? 1 : 0)) * s->_expand + 1;

	s->_len = len;


	if (cap > s->_capacity)
	{
		s->_capacity = cap;

		char* str = (char*)realloc(s->_str, sizeof(char) * s->_capacity);
		assert(str != NULL);

		s->_str = str;
	}

	strncpy(s->_str, cstr, s->_capacity);
}

void String_cat(String* s, const char* cstr)
{
	size_t len = strlen(cstr) + s->_len;
	size_t cap = (len / s->_expand + ((len % s->_expand > 0) ? 1 : 0)) * s->_expand + 1;

	s->_len = len;


	if (cap > s->_capacity)
	{
		s->_capacity = cap;

		char* str = (char*)realloc(s->_str, sizeof(char) * s->_capacity);
		assert(str != NULL);

		s->_str = str;
	}
	
	strncat(s->_str, cstr, s->_capacity);
}

