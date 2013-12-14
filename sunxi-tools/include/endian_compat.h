#ifndef SUNXI_ENDIAN_COMPAT_H_
#define SUNXI_ENDIAN_COMPAT_H_

#ifdef __APPLE__ 
#include <CoreFoundation/CoreFoundation.h>
#define htole32(x) CFSwapInt32HostToLittle(x)
#define le32toh(x) CFSwapInt32LittleToHost(x)
#define htole16(x) CFSwapInt16HostToLittle(x)
#define le16toh(x) CFSwapInt16LittleToHost(x)
#else
#include <endian.h>
#endif

#endif
