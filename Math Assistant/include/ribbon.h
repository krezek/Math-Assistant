#ifndef _RIBBON_H_
#define _RIBBON_H_

#define WM_RIBBON_HEIGHT_CHANGED WM_USER + 10
#define WM_RIBBON_COMMAND WM_USER + 11

int CreateRibbon(HWND hWnd);
void DestroyRibbon();

#endif /* _RIBBON_H_ */
