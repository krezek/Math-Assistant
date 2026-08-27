/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include "map.h"


Node* Node_init(const char* k, void* v, Node* l, Node* r)
{
	Node* n = (Node*)malloc(sizeof(Node));
	assert(n != NULL);

	n->_key = (char*)malloc((strlen(k) + 1) * sizeof(char));
	assert(n->_key != NULL);
	strncpy(n->_key, k, strlen(k) + 1);

	n->_value = v;
	n->_left = l;
	n->_right = r;

	return n;
}

void Node_free(Node* n, MValueFree freeFunc)
{
	if (n->_key)
		free(n->_key);

	if (n->_value)
		freeFunc(n->_value);

	free(n);
}

Map* Map_init()
{
	Map* m = (Map*)malloc(sizeof(Map));
	assert(m != NULL);

	m->_root = NULL;

	return m;
}

void Map_free(Map* m, MValueFree freeFunc)
{
	Map_remove_all(m, freeFunc);
	free(m);
}

int Map_insert_node(Node** pn, const char* k, void* v)
{
	if (*pn == NULL)
	{
		*pn = Node_init(k, v, NULL, NULL);
		return 0;
	}
	else
	{
		if (strcmp((*pn)->_key, k) > 0)
		{
			return Map_insert_node(&((*pn)->_left), k, v);
		}
		else if (strcmp((*pn)->_key, k) < 0)
		{
			return Map_insert_node(&((*pn)->_right), k, v);
		}
		else
		{
			printf("Map_insert_n: error\n");
			return -1;
		}
	}
}

int Map_insert(Map* m, const char* k, void* v)
{
	return Map_insert_node(&m->_root, k, v);
}

void* Map_get_node(Node** pn, const char* k)
{
	if (*pn == NULL)
		return NULL;

	if (strcmp((*pn)->_key, k) == 0)
		return (*pn)->_value;
	else if (strcmp((*pn)->_key, k) > 0)
		return Map_get_node(&((*pn)->_left), k);
	else
		return Map_get_node(&((*pn)->_right), k);
}

void* Map_get(Map* m, const char* k)
{
	return Map_get_node(&m->_root, k);
}

void Map_remove_node(Node** pn, const char* k, MValueFree freeFunc)
{
	if (*pn == NULL)
		return;

	if (strcmp((*pn)->_key, k) == 0)
	{
		if ((*pn)->_left == NULL &&
			(*pn)->_right == NULL)
		{
			Node_free(*pn, freeFunc);
			*pn = NULL;
		}
		else if ((*pn)->_right == NULL)
		{
			Node* temp = *pn;
			*pn = (*pn)->_left;
			temp->_left = NULL;
			Node_free((temp), freeFunc);
		}
		else if ((*pn)->_left == NULL)
		{
			Node* temp = *pn;
			*pn = (*pn)->_right;
			temp->_right = NULL;
			Node_free((temp), freeFunc);
		}
		else
		{
			Node** cur = &(*pn)->_right;

			while ((*cur)->_left)
				cur = &(*cur)->_left;

			free((*pn)->_key);
			if((*pn)->_value)
				freeFunc((*pn)->_value);

			(*pn)->_key = (char*)malloc((strlen((*cur)->_key) + 1) * sizeof(char));
			assert((*pn)->_key != NULL);
			strcpy((*pn)->_key, (*cur)->_key);
			
			(*pn)->_value = (*cur)->_value;
			(*cur)->_value = NULL;
			
			Map_remove_node(cur, (*cur)->_key, freeFunc);
		}
	}
	else if (strcmp((*pn)->_key, k) > 0)
		Map_remove_node(&((*pn)->_left), k, freeFunc);
	else
		Map_remove_node(&((*pn)->_right), k, freeFunc);
}

void Map_remove(Map* m, const char* k, MValueFree freeFunc)
{
	Map_remove_node(&m->_root, k, freeFunc);
}

void Map_traversal_n(Node* node) {
	if (node != NULL) 
	{
		Map_traversal_n(node->_left);
		printf("(%s, %llx) ", node->_key, (long long)node->_value);
		Map_traversal_n(node->_right);
	}
}

void Map_traversal(Map* m)
{
	Map_traversal_n(m->_root);
	printf("\n");
}

void Map_remove_all_node(Node* node, MValueFree freeFunc)
{
	if (node == NULL)
		return;

	Map_remove_all_node(node->_left, freeFunc);
	Map_remove_all_node(node->_right, freeFunc);

	Node_free(node, freeFunc);
}

void Map_remove_all(Map* m, MValueFree freeFunc)
{
	Map_remove_all_node(m->_root, freeFunc);
}
