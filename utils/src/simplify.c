#include "pch.h"
#include "platform.h"

#include <gmp.h>
#include <mpfr.h>

#include <items.h>
#include <simplify.h>

void parse_items(Item** pItems, const char* s);
bool traverse_rules(Item** pItems);

char* do_simplify(const char* expr)
{
	Item* item = NULL;
	parse_items(&item, expr);

	if (item)
	{
#ifdef _DEBUG
		printf("simplify origin: %s\n", expr);

		String* sx = String_init();
		item->_toStringFunc(0, item, sx);
		printf("simplify after: %s\n", sx->_str);
		String_free(sx);
#endif

		int rc = 0;
		String* s = String_init();

		while(traverse_rules(&item));
		item->_toStringFunc(0, item, s);

		ItemTree_free(&item);

		char* rt = (char*)malloc((s->_len + 1) * sizeof(char));
		assert(rt != NULL);
		sprintf(rt, "%s", s->_str);

		String_free(s);

		return rt;
	}


	return NULL;
}
