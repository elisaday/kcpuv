#pragma once

#include <errno.h>
#include "log.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(n) \
	do { \
		if (n) { \
			(n)->Release(); \
			(n) = NULL; \
		} \
	} while (0)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(n) \
	do { \
		if (n) { \
			delete (n); \
			(n) = NULL; \
		} \
	} while (0)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(n) \
	do { \
		if (n) { \
			delete [](n); \
			(n) = NULL; \
		} \
	} while (0)
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(n) \
	do { \
		if (n) { \
			free(n); \
			(n) = NULL; \
		} \
	} while (0)
#endif

#ifndef PROC_ERR
#define PROC_ERR(n) \
	do { \
		if ((n) < 0) { \
			log_err("file: [%s] line: [%d] condition: [%d] ecode: [%d] desc: [%s]", __FILE__, __LINE__, (n), errno, strerror(errno)); \
			goto Exit0; \
		} \
	} while (0)
#endif

#ifndef PROC_ERR_NOLOG
#define PROC_ERR_NOLOG(n) \
	do { \
		if ((n) < 0) { \
			goto Exit0; \
		} \
	} while (0)
#endif

#ifndef PROC_ERR_NOLOG_EXIT
#define PROC_ERR_NOLOG_EXIT(n, e) \
	do { \
		if ((n) < 0) { \
			goto e; \
		} \
	} while (0)
#endif

#ifndef CHK_COND
#define CHK_COND(n) \
	do { \
		if (!(n)) { \
			log_err("file: [%s] line: [%d]", __FILE__, __LINE__); \
			goto Exit0; \
		} \
	} while (0)
#endif

#ifndef CHK_COND_NOLOG
#define CHK_COND_NOLOG(n) \
	do { \
		if (!(n)) { \
			goto Exit0; \
		} \
	} while (0)
#endif

#ifndef CHK_COND_NOLOG_EXIT
#define CHK_COND_NOLOG_EXIT(n, e) \
	do { \
		if (!(n)) { \
			goto e; \
		} \
	} while (0)
#endif

