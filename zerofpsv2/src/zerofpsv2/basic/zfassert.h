#ifndef _BASIC_ZFASSERT_H_
#define _BASIC_ZFASSERT_H_

#include <cassert>

#define ZFAssert(test, strdesc)	assert(test && strdesc)

#endif


