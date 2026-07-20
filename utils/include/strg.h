#ifndef _STRG_H_
#define _STRG_H_

typedef struct
{
	char* _str;
	size_t _len;
	size_t _expand;
	size_t _capacity;
	size_t _index;
} String;

String* String_init();
String* String_init_v2(size_t expand, size_t count);
void String_free(String* s);
void String_cpy(String* s, const char* cstr);
void String_cat(String* s, const char* cstr);

#endif /* _STRG_H_ */
