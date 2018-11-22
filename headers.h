#if ! defined(_HEADERS_KUDUSHTEEV_H_)
#define _HEADERS_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __GNUC__
#define POWERED_BY_DIRECT_SOUND  1
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS  1
#endif

#ifndef STRICT
#define STRICT
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#ifdef __BORLANDC__
#pragma warn -8004
#pragma warn -8027
#endif

#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "resource.h"

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")

#ifdef POWERED_BY_DIRECT_SOUND
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#endif

#pragma warning(disable:4244)
#endif

#ifndef __BORLANDC__
#define  _tcos  cosf
#define  _tsin  sinf
#define  _tsqrt sqrtf
#define  _ttan  tanf
#define  _tatan atanf
#define  _tabs  fabsf
#else
#define  _tcos(x)    static_cast<float>(cos(static_cast<float>((x))))
#define  _tsin(x)    static_cast<float>(sin(static_cast<float>((x))))
#define  _tsqrt(x)   static_cast<float>(sqrt(static_cast<float>((x))))
#define  _ttan(x)    static_cast<float>(tan(static_cast<float>((x))))
#define  _tatan(x,y) static_cast<float>(atan(static_cast<float>((x)), static_cast<float>((y))))
#define  _tabs(x)    static_cast<float>(fabs(static_cast<float>((x))))
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

void* operator new (size_t size);
void* operator new [] (size_t size);
void  operator delete (void* ptr);
void  operator delete [] (void* ptr);

#include "form.h"
#include "gdiobj.h"
#include "sdata.h"
#include "in_out.h"
#include "util.h"
#include "menu.h"
#include "sound.h"
#include "game.h"
#endif
