#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <wchar.h>
#include <math.h>

#include <mpfr.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#endif /* _PLATFORM_H_ */

