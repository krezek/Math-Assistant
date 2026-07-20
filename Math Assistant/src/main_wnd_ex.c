#include "pch.h"
#include "platform.h"

#include <..\..\CFG.h>

#include <sqlite3.h>
#include <winutil.h>

static char* g_cookie_ptr = NULL; // used with reading rtf callback function

void LoadText(HWND hWndText, const wchar_t* text, int index);

int Populate_Books(HWND hWndBookCombobox)
{
	sqlite3* bible_db;
	char* err_msg = 0;
	sqlite3_stmt* res;
	int rc;
	const wchar_t* sql = L"SELECT id, name, table_name FROM books ORDER BY id";

	rc = sqlite3_open_v2(DB_URL, &bible_db, SQLITE_OPEN_READONLY, NULL);

	if (rc != SQLITE_OK) {
		ShowError(L"Can't open database file!");
		sqlite3_close(bible_db);
		return -1;
	}

	rc = sqlite3_prepare16_v2(bible_db, sql, -1, &res, 0);

	if (rc != SQLITE_OK)
	{
		ShowError(L"Unable to select books!");
		sqlite3_finalize(res);
		sqlite3_close(bible_db);
		return -1;
	}

	while (sqlite3_step(res) == SQLITE_ROW)
	{
		const int index = sqlite3_column_int(res, 0);
		const wchar_t* name = sqlite3_column_text16(res, 1);

		SendMessage(hWndBookCombobox, CB_INSERTSTRING, index - 1, (LPARAM)name);
	}

	sqlite3_finalize(res);
	sqlite3_close(bible_db);

	return 0;
}

int Populate_Tree(HWND hWndTree, int index)
{
	sqlite3* bible_db;
	char* err_msg = 0;
	sqlite3_stmt* res;
	int rc;
	wchar_t sql[255];
	HTREEITEM hItem;
	TVINSERTSTRUCT insertStruct = { 0 };
	TVITEM* pItem = &insertStruct.item;
	insertStruct.hParent = NULL;
	insertStruct.hInsertAfter = TVI_ROOT;

	TreeView_DeleteAllItems(hWndTree);

	wsprintf(sql, L"SELECT id, chapter_title, body_text FROM book_%d ORDER BY id", index + 1);

	rc = sqlite3_open_v2(DB_URL, &bible_db, SQLITE_OPEN_READONLY, NULL);

	if (rc != SQLITE_OK) {
		ShowError(L"Can't open database file!");
		sqlite3_close(bible_db);
		return -1;
	}

	rc = sqlite3_prepare16_v2(bible_db, sql, -1, &res, 0);

	if (rc != SQLITE_OK)
	{
		ShowError(sql);
		sqlite3_finalize(res);
		sqlite3_close(bible_db);
		return -1;
	}

	while (sqlite3_step(res) == SQLITE_ROW)
	{
		const wchar_t* name = sqlite3_column_text16(res, 1);

		pItem->mask = TVIF_TEXT;
		pItem->pszText = (wchar_t*)name;
		hItem = (HTREEITEM)SendMessage(hWndTree, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
	}

	sqlite3_finalize(res);
	sqlite3_close(bible_db);

	return 0;
}

void OnDBClick_treeView(HWND hWndTree, HWND hWndText, WPARAM wParam, LPARAM lParam, int index)
{
	TVITEM item;
	HTREEITEM hSelectedItem = TreeView_GetSelection(hWndTree);

	item.hItem = hSelectedItem;
	item.mask = TVIF_TEXT;
	item.cchTextMax = 128;
	item.pszText = (wchar_t*)malloc(item.cchTextMax * sizeof(wchar_t));
	assert(item.pszText);
	memset(item.pszText, 0, item.cchTextMax * sizeof(wchar_t));

	if (TreeView_GetItem(hWndTree, &item))
	{
		LoadText(hWndText, item.pszText, index);

		goto cleanup;
	}

cleanup:
	free(item.pszText);
}

DWORD CALLBACK EditStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb) 
{
	strncpy(pbBuff, g_cookie_ptr, cb);
	*pcb = cb;
	g_cookie_ptr += cb;
	
	return 0;
}

void LoadText(HWND hWndText, const wchar_t* text, int index)
{
	sqlite3* bible_db;
	char* err_msg = 0;
	sqlite3_stmt* res;
	int rc;
	wchar_t sql[255];
	CHARRANGE cr;

	cr.cpMin = 0;
	cr.cpMax = -1;
	SendMessage(hWndText, EM_EXSETSEL, 0, (LPARAM)&cr);
	SendMessage(hWndText, EM_REPLACESEL, 0, (LPARAM)L"");

	wsprintf(sql, L"SELECT chapter_title, body_text FROM book_%d WHERE chapter_title = '%s'", index + 1, text);

	rc = sqlite3_open_v2(DB_URL, &bible_db, SQLITE_OPEN_READONLY, NULL);

	if (rc != SQLITE_OK) {
		ShowError(L"Can't open database file!");
		sqlite3_close(bible_db);
		return;
	}

	rc = sqlite3_prepare16_v2(bible_db, sql, -1, &res, 0);

	if (rc != SQLITE_OK)
	{
		ShowError(sql);
		sqlite3_finalize(res);
		sqlite3_close(bible_db);
		return;
	}

	if (sqlite3_step(res) == SQLITE_ROW)
	{
		EDITSTREAM es;
		const char* name = sqlite3_column_text(res, 1);
		
		g_cookie_ptr = (LPSTR)name;
		es.dwCookie = (DWORD_PTR)name;
		es.pfnCallback = EditStreamCallback;

		SendMessage(hWndText, EM_STREAMIN, SF_RTF, (LPARAM)&es);
	}

	sqlite3_finalize(res);
	sqlite3_close(bible_db);
}
