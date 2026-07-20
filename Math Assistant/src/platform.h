#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#define _CRT_SECURE_NO_WARNINGS
#define COBJMACROS

#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <objbase.h>
#include <UIRibbon.h>
#include <initguid.h>
#include <propvarutil.h>
#include "strsafe.h"

#include <initguid.h>
#include <strsafe.h>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <D3Dcompiler.h>

#include <d3d11on12.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


#ifdef _DEBUG
#include <crtdbg.h>
#endif

#endif /* _PLATFORM_H_ */

