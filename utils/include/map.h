#ifndef _MAP_H_
#define _MAP_H_

typedef void (*MValueFree) (void* v);

typedef struct _Node
{
	char* _key;
	void* _value;

	struct _Node* _left;
	struct _Node* _right;

} Node;

typedef struct _Map
{
	Node* _root;
} Map;

Map* Map_init();
void Map_free(Map* m, MValueFree freeFunc);

int Map_insert(Map* m, const char* k, void* v);
void* Map_get(Map* m, const char* k);
void Map_remove(Map* m, const char* k, MValueFree freeFunc);
void Map_remove_all(Map* m, MValueFree freeFunc);
void Map_traversal(Map* m);

#endif /* _MAP_H_ */
