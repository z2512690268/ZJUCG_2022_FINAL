#ifndef _UTILS_H
#define _UTILS_H

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) { if (p) { delete p; p = NULL; } }

#endif