/* ncftp.h
 *
 * Copyright (c) 1996-2016 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#ifndef _ncftp_h_
#define _ncftp_h_ 1

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

#define kLibraryVersion "@(#) LibNcFTP 3.2.6 (November 12, 2016)"

/* This is used to verify validty of the data passed in. */
#define kLibraryMagic "LibNcFTP 3.2.6"
#define kLibraryMagicLen 14

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	/* Includes for Windows */
#	pragma once
#	ifndef __MINGW32__
#		pragma warning(disable : 4127)	// warning C4127: conditional expression is constant
#		pragma warning(disable : 4100)	// warning C4100: 'lpReserved' : unreferenced formal parameter
#		pragma warning(disable : 4514)	// warning C4514: unreferenced inline function has been removed
#		pragma warning(disable : 4115)	// warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
#		pragma warning(disable : 4201)	// warning C4201: nonstandard extension used : nameless struct/union
#		pragma warning(disable : 4214)	// warning C4214: nonstandard extension used : bit field types other than int
#		pragma warning(disable : 4115)	// warning C4115: 'IRpcStubBuffer' : named type definition in parentheses
#		pragma warning(disable : 4711)	// warning C4711: function selected for automatic inline expansion
#	endif
	/* We now try for at least Windows 2000 compatibility (0x0500).
	 * The code will still work on older systems, though.
	 * Prior versions used 0x0400 instead.
	 */
#	ifndef WINVER
#		define WINVER 0x0500
#	endif
#	ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0500
#	endif
#	ifndef _INC_WINDOWS
#		include <windows.h>	/* includes <winsock2.h> if _WIN32_WINNT >= 0x400 */
#	endif
#	ifndef _INC_TYPES
#		include <sys/types.h>
#	endif
#	ifndef _INC_STAT
#		include <sys/stat.h>
#	endif
#	ifndef _INC_IO
#		include <io.h>
#	endif
#	ifndef _INC_CONIO
#		include <conio.h>
#	endif
#	ifndef _INC_DIRECT
#		include <direct.h>
#	endif
#	ifndef _INC_STDIO
#		include <stdio.h>
#	endif
#	ifndef _INC_STRING
#		include <string.h>
#	endif
#	ifndef _INC_STDLIB
#		include <stdlib.h>
#	endif
#	ifndef _INC_STDDEF
#		include <stddef.h>
#	endif
#	ifndef _INC_STDARG
#		include <stdarg.h>
#	endif
#	ifndef _INC_CTYPE
#		include <ctype.h>
#	endif
#	ifndef _INC_TIME
#		include <time.h>
#	endif
#	ifndef _INC_ERRNO
#		include <errno.h>
#	endif
#	ifndef _INC_SIGNAL
#		include <signal.h>
#	endif
#	ifndef _INC_SETJMP
#		include <setjmp.h>
#	endif
#	define longest_int __int64
#	define longest_uint __uint64
#	ifndef HAVE_LONG_LONG
#		define HAVE_LONG_LONG 1
#	endif
#	ifndef SCANF_LONG_LONG
#		define SCANF_LONG_LONG "%I64d"
#	endif
#	ifndef PRINTF_LONG_LONG
#		define PRINTF_LONG_LONG "%I64d"
#	endif
#	ifndef PRINTF_LONG_LONG_I64D
#		define PRINTF_LONG_LONG_I64D 1
#	endif
#	ifndef SCANF_LONG_LONG_I64D
#		define SCANF_LONG_LONG_I64D 1
#	endif
#	ifndef USE_SIO
#		define USE_SIO 1
#	endif
#	ifndef NO_SIGNALS
#		define NO_SIGNALS 1
#	endif
#	ifndef main_void_return_t
#		define main_void_return_t void
#	endif
#	ifndef strcasecmp
#		define strcasecmp _stricmp
#		define strncasecmp _strnicmp
#		define strdup _strdup
#		define fdopen _fdopen
#	endif
#	ifndef mode_t
#		define mode_t int
#	endif
#	ifndef S_ISREG
#		define S_ISREG(m)      (((m) & _S_IFMT) == _S_IFREG)
#		define S_ISDIR(m)      (((m) & _S_IFMT) == _S_IFDIR)
#		define S_ISLNK(m)      (0)
#	endif
#	ifndef open
#		define open _open
#		define write _write
#		define read _read
#		define close _close
#		define lseek _lseek
#		define stat _stat
#		define lstat _stat
#		define fstat _fstat
#		define dup _dup
#		define utime _utime
#		define utimbuf _utimbuf
#	endif
#	ifndef unlink
#		define unlink remove
#	endif
#	ifndef HAVE_LONG_FILE_NAMES
#		define HAVE_LONG_FILE_NAMES 1
#	endif
#	ifndef HAVE_MKTIME
#		define HAVE_MKTIME 1
#	endif
#	ifndef HAVE_MEMMOVE
#		define HAVE_MEMMOVE 1
#	endif
#	ifndef HAVE_STRSTR
#		define HAVE_STRSTR 1
#	endif
#	ifndef HAVE_VSNPRINTF
#		define HAVE_VSNPRINTF 1
#	endif
#	ifndef HAVE_SNPRINTF
#		define HAVE_SNPRINTF 1
#	endif
#	ifndef vsnprintf
#		define vsnprintf _vsnprintf
#	endif
#	ifndef snprintf
#		define snprintf _snprintf
#	endif
#else
	/* Includes for UNIX */
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/time.h>
#	include <sys/wait.h>
#	include <stdio.h>
#	include <string.h>
#	ifdef HAVE_STRINGS_H
#		include <strings.h>
#	endif
#	include <stdlib.h>
#	include <stddef.h>
#	include <stdarg.h>
#	include <ctype.h>
#	include <time.h>
#	include <errno.h>
#	include <signal.h>
#	include <setjmp.h>
#	if !defined(__ultrix) || !defined(XTI)
#		include <sys/socket.h>
#	endif
#	include <netinet/in.h>
#	include <fcntl.h>
#	include <netdb.h>
#	ifndef USE_SIO
#		define USE_SIO 1
#	endif
#	ifndef NO_SIGNALS
#		define NO_SIGNALS 1
#	endif
#endif

#include "ncftp_errno.h"

#ifndef longest_int
#define longest_int long long
#define longest_uint unsigned long long
#endif

#ifndef forever
#	define forever for ( ; ; )
#endif

typedef void (*FTPSigProc)(int);

typedef struct FTPLine *FTPLinePtr;
typedef struct FTPLine {
	FTPLinePtr prev, next;
	char *line;
} FTPLine;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

typedef struct FTPLineList {
	FTPLinePtr first, last;
	int nLines;
} FTPLineList, *FTPLineListPtr;

typedef struct Response {
	FTPLineList msg;
	int codeType;
	int code;
	int printMode;
	int eofOkay;
	int hadEof;
} Response, *ResponsePtr;

#if USE_SIO && !defined(defined_SReadlineInfo)
#define defined_SReadlineInfo 1
typedef struct SReadlineInfo {
	char *buf;		/* Pointer to beginning of buffer. */
	char *bufPtr;		/* Pointer to current position in buffer. */
	char *bufLim;		/* Pointer to end of buffer. */
	size_t bufSize;		/* Current size of buffer block. */
	size_t bufSizeMax;	/* Maximum size available for buffer. */
	int malloc;		/* If non-zero, malloc() was used for buf. */
	int fd;			/* File descriptor to use for I/O. */
	int timeoutLen;		/* Timeout to use, in seconds. */
	int requireEOLN;	/* When buffer is full, continue reading and discarding until \n? */
} SReadlineInfo;
#endif
    
#ifdef __clang__
#pragma clang diagnostic pop
#endif

typedef struct FTPLibraryInfo {
	char magic[16];				/* Don't modify this field. */
	int init;				/* Don't modify this field. */
	int socksInit;				/* Don't modify this field. */
	unsigned int defaultPort;		/* Don't modify this field. */
	char defaultAnonPassword[80];		/* You may set this after init. */
} FTPLibraryInfo, *FTPLIPtr;

typedef struct FTPConnectionInfo *FTPCIPtr;
typedef void (*FTPProgressMeterProc)(const FTPCIPtr, int);
typedef void (*FTPLogProc)(const FTPCIPtr, char *);
typedef void (*FTPConnectMessageProc)(const FTPCIPtr, ResponsePtr);
typedef void (*FTPLoginMessageProc)(const FTPCIPtr, ResponsePtr);
typedef void (*FTPRedialStatusProc)(const FTPCIPtr, int, int);
typedef void (*FTPPrintResponseProc)(const FTPCIPtr, ResponsePtr);
typedef void (*FTPGetPassphraseProc)(const FTPCIPtr, FTPLineListPtr pwPrompt, char *pass, size_t dsize);

typedef struct FTPConnectionInfo {
	char magic[16];				/* Don't modify this field. */
	
	char host[128];				/* REQUIRED input parameter. */
	char user[128];				/* OPTIONAL input parameter. */
	char pass[256];				/* OPTIONAL input parameter. */
	char acct[64];				/* OPTIONAL input parameter. */
	unsigned int passIsEmpty;		/* OPTIONAL input parameter. */
	unsigned int port;			/* OPTIONAL input parameter. */

	int errNo;				/* You may modify this if you want. */
	char lastFTPCmdResultStr[128];		/* You may modify this if you want. */
    int lastFTPCmdResultNum;		/* You may modify this if you want. */
	FTPLineList lastFTPCmdResultLL;		/* Use, but do not modify. */

	FILE *debugLog;				/* OPTIONAL input parameter. */
	FTPLogProc debugLogProc;		/* OPTIONAL input parameter. */
    int debugTimestamping;			/* OPTIONAL input parameter. */

	unsigned int xferTimeout;		/* OPTIONAL input parameter. */
	unsigned int connTimeout;		/* OPTIONAL input parameter. */
	unsigned int ctrlTimeout;		/* OPTIONAL input parameter. */
	unsigned int abortTimeout;		/* OPTIONAL input parameter. */

	int maxDials;				/* OPTIONAL input parameter. */
	int redialDelay;			/* OPTIONAL input parameter. */

	int dataPortMode;			/* OPTIONAL input parameter. */

	int firewallType;			/* OPTIONAL input parameter. */
	char firewallHost[64];			/* OPTIONAL input parameter. */
	char firewallUser[64];			/* OPTIONAL input parameter. */
	char firewallPass[64];			/* OPTIONAL input parameter. */
	unsigned int firewallPort;		/* OPTIONAL input parameter. */

	size_t ctrlSocketRBufSize;		/* OPTIONAL input parameter. */
	size_t ctrlSocketSBufSize;		/* OPTIONAL input parameter. */
	size_t dataSocketRBufSize;		/* OPTIONAL input parameter. */
	size_t dataSocketSBufSize;		/* OPTIONAL input parameter. */

	const char *asciiFilenameExtensions;	/* OPTIONAL input parameter. */
	int shutdownUnusedSideOfSockets;	/* OPTIONAL input parameter. */

	unsigned short ephemLo;			/* OPTIONAL input parameter. */
	unsigned short ephemHi;			/* OPTIONAL input parameter. */

	FTPConnectMessageProc onConnectMsgProc; /* OPTIONAL input parameter. */
	FTPRedialStatusProc redialStatusProc;	/* OPTIONAL input parameter. */
	FTPPrintResponseProc printResponseProc; /* OPTIONAL input parameter. */
	FTPLoginMessageProc onLoginMsgProc;	/* OPTIONAL input parameter. */

	FTPGetPassphraseProc passphraseProc;	/* OPTIONAL input parameter. */

	/* Used during transfers; valid during progress meter updates. */
	FTPProgressMeterProc progress;		/* OPTIONAL input parameter. */
	longest_int bytesTransferred;		/* Do not modify this field. */
	struct timeval t0;			/* Do not modify this field. */
	double sec;				/* Do not modify this field. */
	double secLeft;				/* Do not modify this field. */
	double kBytesPerSec;			/* Do not modify this field. */
	double percentCompleted;		/* Do not modify this field. */
	longest_int expectedSize;		/* Do not modify this field. */
	time_t mdtm;				/* Do not modify this field. */
	time_t nextProgressUpdate;		/* Do not modify this field. */
	const char *rname;			/* Do not modify this field. */
	const char *lname;			/* Do not modify this field. */
    int useProgressMeter;			/* Used internally. */
	int stalled;				/* Do not modify this field. */
	int dataTimedOut;			/* Do not modify this field. */
	int cancelXfer;				/* You may modify this. */
	int canceling;				/* Use, but do not modify. */
	int canceled;				/* Use, but do not modify. */

	char actualHost[64];			/* Do not modify this field. */
	char ip[32];				/* Do not modify this field. */

	char *startingWorkingDirectory;		/* Use, but do not modify. */
	char *currentWorkingDirectory;		/* Use, but do not modify. */
	size_t currentWorkingDirectorySize;	/* Use, but do not modify. */

	int connected;				/* Do not modify this field. */
	int loggedIn;				/* Do not modify this field. */
	int curTransferType;			/* Do not modify this field. */
    int reserved2;
	longest_int startPoint;			/* Do not modify this field. */
	FILE *errLog;				/* DEPRECATED input parameter. */
	FTPLogProc errLogProc;			/* DEPRECATED input parameter. */
	FTPLIPtr lip;				/* Do not modify this field. */

	int hasPASV;				/* Do not modify this field. */
	int hasSIZE;				/* Do not modify this field. */
	int hasMDTM;				/* Do not modify this field. */
	int hasMDTM_set;			/* Do not modify this field. */
	int hasMFMT;				/* Do not modify this field. */
	int hasMFF;				/* Do not modify this field. */
	int hasREST;				/* Do not modify this field. */
	int hasAPPE;				/* You may modify this. */
	int hasNLST_a;				/* Do not modify this field. */
	int hasNLST_d;				/* Do not modify this field. */
	int hasFEAT;				/* Do not modify this field. */
	int hasMLSD;				/* Do not modify this field. */
	int hasMLST;				/* Do not modify this field. */
	int usedMLS;				/* Do not modify this field. */
	int hasCLNT;				/* Do not modify this field. */
	int hasHELP_SITE;			/* Do not modify this field. */
	int hasSITE_UTIME;			/* Do not modify this field. */
	int hasSITE_RETRBUFSIZE;		/* Do not modify this field. */
	int hasSITE_RBUFSIZ;			/* Do not modify this field. */
	int hasSITE_RBUFSZ;			/* Do not modify this field. */
	int hasSITE_STORBUFSIZE;		/* Do not modify this field. */
	int hasSITE_SBUFSIZ;			/* Do not modify this field. */
	int hasSITE_SBUFSZ;			/* Do not modify this field. */
	int hasSITE_BUFSIZE;			/* Do not modify this field. */
	int hasRETR_tar;			/* Do not modify this field. */
	int mlsFeatures;			/* Do not modify this field. */
	int STATfileParamWorks;			/* Do not modify this field. */
	int NLSTfileParamWorks;			/* Do not modify this field. */

	struct sockaddr_in servCtlAddr;		/* Do not modify this field. */
	struct sockaddr_in servDataAddr;	/* Do not modify this field. */
	struct sockaddr_in ourCtlAddr;		/* Do not modify this field. */
	struct sockaddr_in ourDataAddr;		/* Do not modify this field. */
	int netMode;				/* Do not use or modify. */
    int doAllocBuf;				/* Do not modify this field. */
	char *buf;				/* Do not modify this field. */
	size_t bufSize;				/* Do not modify this field. */
	FILE *cin;				/* Do not use or modify. */
	FILE *cout;				/* Do not use or modify. */
	int ctrlSocketR;			/* You may use but not modify/close. */
	int ctrlSocketW;			/* You may use but not modify/close. */
	int dataSocket;				/* You may use but not modify/close. */
	int dataSocketConnected;		/* Do not use or modify. */
	int leavePass;				/* You may modify this. */
	int eofOkay;				/* Do not use or modify. */
	int require20;				/* You may modify this. */
	int allowProxyForPORT;			/* You may modify this. */
	int usingTAR;				/* Use, but do not modify. */
	int serverType;				/* Do not use or modify. */
	int ietfCompatLevel;			/* Do not use or modify. */

	int numDials;				/* Do not modify. */
	int totalDials;				/* Do not modify. */
    int doNotGetStartingWorkingDirectory;	/* You may modify this. */
    struct timeval initTime;		/* Do not modify. */
	struct timeval startTime;		/* Do not modify. */
	struct timeval connectTime;		/* Do not modify. */
	struct timeval loginTime;		/* Do not modify. */
	struct timeval disconnectTime;		/* Do not modify. */
	struct timeval lastCmdStart;		/* Do not modify. */
	struct timeval lastCmdFinish;		/* Do not modify. */
	int numDownloads;			/* Do not modify. */
	int numUploads;				/* Do not modify. */
	int numListings;			/* Do not modify. */
    int reserved3;
	char textEOLN[4];			/* Set automatically per platform. */
	int asciiTranslationMode;		/* You may modify this. */
	const char *manualOverrideFeatures;	/* You may modify this. */

	/* Only set the following field if all the following are true:
	 *   Your client FTP program is running on a private network (e.g. 192.168.1.0); and
	 *   You are accessing a server outside your network (e.g., on the Internet); and
	 *   Your network's firewall does NOT fix private addresses in PORT for you; and
	 *   You know what your external IP address is.
	 */
	struct sockaddr_in clientKnownExternalAddr;	

	struct sockaddr_in preferredLocalAddr;	/* You may modify this. */
	int maxNumberOfSuccessivePASVAttempts;	/* You may modify this. */
	int recentMdtmHeuristic;		/* You may modify this. */
	time_t recentMdtmThreshold;		/* You may modify this. */
	time_t timeBetweenUtimeBlocks;		/* You may modify this. */
    int utimeBlocks;			/* You may modify this. */
	int useSendfile;			/* You may modify this. */
	int usingSendfile;			/* Do not use or modify. */

#if USE_SIO
    int srlReserved;
	char srlBuf[1024];
	SReadlineInfo ctrlSrl;			/* Do not use or modify. */
#endif
    int iUser;				/* Scratch integer field you can use. */
    int iUser2;				/* Scratch integer field you can use. */
	void *pUser;				/* Scratch pointer field you can use. */
	longest_int llUser;			/* Scratch long long field you can use. */
	int reserved[32];			/* Do not use or modify. */
	char tailMagic[16];			/* Do not use or modify. */
} FTPConnectionInfo;

typedef struct FTPFileInfo *FTPFileInfoPtr, **FTPFileInfoVec;
typedef struct FTPFileInfo {
	FTPFileInfoPtr prev, next;
	char *relname;
	char *rname;
	char *rlinkto;
	char *lname;
	char *plug;	/* permissions, links, user, group */
	int type;
    int mode;	/* Only set by UnMlsD() */
	time_t mdtm;
	longest_int size;
	size_t relnameLen;
} FTPFileInfo;

typedef struct FTPFileInfoList {
	FTPFileInfoPtr first, last;
	FTPFileInfoVec vec;
	size_t maxFileLen;
	size_t maxPlugLen;
	int nFileInfos;
	int sortKey;
	int sortOrder;
    int reserved;
} FTPFileInfoList, *FTPFileInfoListPtr;

/* Used with UnMlsT() */
typedef struct MLstItem{
	char fname[512];
	char linkto[512];
	longest_int fsize;
	time_t ftime;
    int ftype;
	int mode;		/* "UNIX.mode" fact */
	int uid;		/* "UNIX.uid" fact */
	int gid;		/* "UNIX.gid" fact */
	char perm[16];		/* "perm" fact */
	char owner[16];		/* "UNIX.owner" fact */
	char group[16];		/* "UNIX.group" fact */
} MLstItem, *MLstItemPtr;

/* Messages we pass to the current progress meter function. */
#define kPrInitMsg			1
#define kPrUpdateMsg			2
#define kPrEndMsg			3

/* Parameter for OpenDataConnection() */
#define kSendPortMode			0
#define kPassiveMode			1
#define kFallBackToSendPortMode		2

/* Parameter for AcceptDataConnection() */
#define kAcceptForWriting		00100
#define kAcceptForReading		00101
#define kNetWriting			kAcceptForWriting
#define kNetReading			kAcceptForReading

/* Value for printMode field of Response structure.
 * Generally, don't worry about this.
 */
#define kResponseNoPrint 00001
#define kResponseNoSave  00002
#define kResponseNoProc  00002

#define kDefaultFTPPort			21

#if ((defined(AIX) || (defined(_AIX))))
#	define kDefaultFTPBufSize	4096
#else
#	define kDefaultFTPBufSize	32768
#endif

#define kDefaultPathBufSize		4096

#ifdef USE_SIO
/* This version of the library can handle timeouts without
 * a user-installed signal handler.
 */
#define kDefaultXferTimeout		600
#define kDefaultConnTimeout		10
#define kDefaultCtrlTimeout		135
#define kDefaultAbortTimeout		10
#else
/* The library doesn't use timeouts by default because it would
 * break apps that don't have a SIGALRM handler.
 */
#define kDefaultXferTimeout		(0)	/* No timeout. */
#define kDefaultConnTimeout		(0)	/* No timeout. */
#define kDefaultCtrlTimeout		(0)	/* No timeout. */
#define kDefaultAbortTimeout		10
#endif


/* Suggested timeout values, in seconds, if you use timeouts. */
#define kSuggestedDefaultXferTimeout	600
#define kSuggestedDefaultConnTimeout	10
#define kSuggestedDefaultCtrlTimeout	135	/* 2*MSL, + slop */ 
#define kSuggestedAbortTimeout		10

#define kDefaultMaxDials		3
#define kDefaultRedialDelay		20	/* seconds */

#define kDefaultDataPortMode		kFallBackToSendPortMode

#define kDefaultMaxNumberOfSuccessivePASVAttempts	3

#define kRedialStatusDialing		0
#define kRedialStatusSleeping		1

#ifndef INADDR_NONE
#	define INADDR_NONE		(0xffffffff)	/* <netinet/in.h> should have it. */
#endif
#ifndef INADDR_ANY
#	define INADDR_ANY		(0x00000000)
#endif

#define kTypeAscii			'A'
#define kTypeBinary			'I'
#define kTypeEbcdic			'E'

#define kGlobChars 			"[*?"
#define GLOBCHARSINSTR(a)		(strpbrk(a, kGlobChars) != NULL)

#define kGlobYes			1
#define kGlobNo				0
#define kRecursiveYes			1
#define kRecursiveNo			0
#define kAppendYes			1
#define kAppendNo			0
#define kResumeYes			1
#define kResumeNo			0
#define kDeleteYes			1
#define kDeleteNo			0
#define kTarYes				1
#define kTarNo				0

#define FTP_UNIMPLEMENTED_CMD(a)	((a == 500) || (a == 502) || (a == 504))
#define FTP_SYNTAX_ERROR_IN_PARAMETERS(a)	(a == 501)	

/* Possible values returned by GetDateAndTime. */
#define kSizeUnknown			((longest_int) (-1))
#define kModTimeUnknown			((time_t) (-1))

#define kCommandAvailabilityUnknown	(-1)
#define kCommandAvailable		1
#define kCommandNotAvailable		0

/* Values returned by FTPDecodeURL. */
#define kNotURL				(-1)
#define kMalformedURL			(-2)

/* Values for the firewall/proxy open. */
#define kFirewallNotInUse			0
#define kFirewallUserAtSite			1
#define kFirewallLoginThenUserAtSite		2
#define kFirewallSiteSite			3
#define kFirewallOpenSite			4
#define kFirewallUserAtUserPassAtPass		5
#define kFirewallFwuAtSiteFwpUserPass		6
#define kFirewallUserAtSiteFwuPassFwp		7
#define kFirewallUserAtSitePort			8
#define kFirewallUserAtSitePort2		9
#define kFirewallLastType			kFirewallUserAtSitePort2

/* For MLSD, MLST, and STAT. */
#define kPreferredMlsOpts	(kMlsOptType | kMlsOptSize | kMlsOptModify | kMlsOptUNIXmode | kMlsOptUNIXowner | kMlsOptUNIXgroup | kMlsOptUNIXuid | kMlsOptUNIXgid | kMlsOptPerm)

#define kMlsOptType		00001
#define kMlsOptSize		00002
#define kMlsOptModify		00004
#define kMlsOptUNIXmode		00010
#define kMlsOptUNIXowner	00020
#define kMlsOptUNIXgroup	00040
#define kMlsOptPerm		00100
#define kMlsOptUNIXuid		00200
#define kMlsOptUNIXgid		00400
#define kMlsOptUnique		01000

/* For FTPChdir3(). */
#define kChdirOnly		00000
#define kChdirAndMkdir		00001
#define kChdirAndGetCWD		00002
#define kChdirOneSubdirAtATime	00004
#define kChdirFullPath		00010

/* Return codes for custom ConfirmResumeDownloadProcs. */
#define kConfirmResumeProcNotUsed 0
#define kConfirmResumeProcSaidSkip 1
#define kConfirmResumeProcSaidResume 2
#define kConfirmResumeProcSaidOverwrite 3
#define kConfirmResumeProcSaidAppend 4
#define kConfirmResumeProcSaidBestGuess 5
#define kConfirmResumeProcSaidCancel 6

typedef int (*FTPConfirmResumeDownloadProc)(
	const FTPCIPtr cip,
	const char **localpath,
	longest_int localsize,
	time_t localmtime,
	const char *remotepath,
	longest_int remotesize,
	time_t remotetime,
	longest_int *startPoint
);

typedef int (*FTPConfirmResumeUploadProc)(
	const FTPCIPtr cip,
	const char *localpath,
	longest_int localsize,
	time_t localmtime,
	const char **remotepath,
	longest_int remotesize,
	time_t remotetime,
	longest_int *startPoint
);

#define kNoFTPConfirmResumeDownloadProc	((FTPConfirmResumeDownloadProc) 0)
#define kNoFTPConfirmResumeUploadProc	((FTPConfirmResumeUploadProc) 0)
#define kNoFTPGetPassphraseProc		((FTPGetPassphraseProc) 0)
#define kNoFTPLogProcProc		((FTPLogProc) 0)
#define kNoFTPConnectMessageProc	((FTPConnectMessageProc) 0)
#define kNoFTPLoginMessageProc		((FTPLoginMessageProc) 0)
#define kNoFTPRedialStatusProc		((FTPRedialStatusProc) 0)

/* Types of FTP server software.
 *
 * We try to recognize a few of these, for information
 * only, and occasional uses to determine some additional
 * or broken functionality.
 */
#define kServerTypeUnknown		0
#define kServerTypeWuFTPd		1
#define kServerTypeNcFTPd		2
#define kServerTypeProFTPD		3
#define kServerTypeMicrosoftFTP		4
#define kServerTypeWarFTPd		5
#define kServerTypeServ_U		6
#define kServerTypeWFTPD		7
#define kServerTypeVFTPD		8
#define kServerTypeFTP_Max		9
#define kServerTypeRoxen		10
#define kServerTypeNetWareFTP		11
#define kServerTypeWS_FTP		12
#define kServerTypeDguxFTP		13
#define kServerTypeIBMFTPCS		14
#define kServerTypePyramid		15

/* This specifies the algorithm to use (if any) for processing the
 * text during ASCII transfers.
 */
#define kAsciiTranslationModeNone	(-1)
#define kAsciiTranslationModeFixEOLNs	0
#define kAsciiTranslationModeStripCRs	1
#define kAsciiTranslationModeDefault	kAsciiTranslationModeFixEOLNs

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	/* Windows has separate functions to close and ioctl sockets. */
#else
	/* UNIX uses close() and ioctl() for all types of descriptors */
#	define closesocket close
#	define ioctlsocket ioctl
#endif

#ifndef STDIN_FILENO
#	define STDIN_FILENO 0
#	define STDOUT_FILENO 1
#	define STDERR_FILENO 2
#endif

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#	define LOCAL_PATH_DELIM '\\'
#	define LOCAL_PATH_DELIM_STR "\\"
#	define LOCAL_PATH_ALTDELIM '/'
#	define IsLocalPathDelim(c) ((c == LOCAL_PATH_DELIM) || (c == LOCAL_PATH_ALTDELIM))
#	define UNC_PATH_PREFIX "\\\\"
#	define IsUNCPrefixed(s) (IsLocalPathDelim(s[0]) && IsLocalPathDelim(s[1]))
#elif defined(__CYGWIN__)
#	define LOCAL_PATH_DELIM '/'
#	define LOCAL_PATH_DELIM_STR "/"
#	define LOCAL_PATH_ALTDELIM '\\'
#	define IsLocalPathDelim(c) ((c == LOCAL_PATH_DELIM) || (c == LOCAL_PATH_ALTDELIM))
#	define UNC_PATH_PREFIX "//"
#	define IsUNCPrefixed(s) (IsLocalPathDelim(s[0]) && IsLocalPathDelim(s[1]))
#else
#	define LOCAL_PATH_DELIM '/'
#	define LOCAL_PATH_DELIM_STR "/"
#	define StrFindLocalPathDelim(a) strchr(a, LOCAL_PATH_DELIM)
#	define StrRFindLocalPathDelim(a) strrchr(a, LOCAL_PATH_DELIM)
#	define StrRemoveTrailingLocalPathDelim StrRemoveTrailingSlashes
#	define IsLocalPathDelim(c) (c == LOCAL_PATH_DELIM)
#	define TVFSPathToLocalPath(s)
#	define LocalPathToTVFSPath(s)
#endif
#define LocalPathBuild(dst,dsize,dlen,pdir,fn) PathBuild(dst, dsize, dlen, LOCAL_PATH_DELIM, pdir, fn)
#define TVFSPathBuild(dst,dsize,dlen,pdir,fn) PathBuild(dst, dsize, dlen, '/', pdir, fn)

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
typedef struct dirent {
	char d_name[MAX_PATH];
} dirent;

typedef struct DIR {
	HANDLE searchHandle;
	char *dirpath;
	WIN32_FIND_DATA ffd;
	dirent dent;
} DIR;

/* Identical in declaration to struct _stati64.
 * Unfortunately, we don't get _stati64 unless we're on an IA-64
 */
struct WinStat64 {
	_dev_t st_dev;
	_ino_t st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	_dev_t st_rdev;
	__int64 st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
};

int WinFStat64(const int h0, struct WinStat64 *const stp);
int WinStat64(const char *const path, struct WinStat64 *const stp);

#	ifndef Stat
#		define Stat WinStat64
#		define Lstat WinStat64
#		define Fstat WinFStat64
#	endif
#endif	/* WIN32 */

/* The following block may be changed by configure script */
#ifndef Stat
#define Stat stat
#define Lstat lstat
#define Fstat fstat
#endif

#define kFtwNoAutoGrowButContinue (-1)
#define kFtwNoAutoGrowAndFail 0
#define kFtwAutoGrow 1
#define kFtwMagic 0xF234567F

typedef struct FtwInfo *FtwInfoPtr;

typedef int (*FtwProc)(const FtwInfoPtr ftwip);

typedef struct FtwInfo {
	unsigned int magic, init;
	FtwProc proc;
	char *curPath;
	size_t curPathLen;
	size_t curPathAllocSize;
	size_t startPathLen;
	char *curFile;
	size_t curFileLen;
	struct Stat curStat;
    int curType;
	int noAutoMallocAndFree;
	int dirSeparator;
	char rootDir[4];
	int autoGrow;
	int isBaseDir;
	size_t depth;
	size_t maxDepth;
	size_t maxDepthAllowed;
	size_t numDirs;
	size_t numFiles;
	size_t numLinks;
	const char *rlinkto;	/* only valid during FTPFtw() */
	size_t direntbufSize;
	void *direntbuf;
	void *cip;		/* only valid during FTPFtw() */
	void *userdata;
} FtwInfo;

typedef struct GetoptInfo {
	unsigned int init;	/* When properly initialized, set to 0xF123456F */
	int err;	/* if error message should be printed */
	int ind;	/* index into parent argv vector */
	int opt;	/* character checked for validity */
	char *arg;	/* argument associated with option */
	char *place;	/* saved position in an arg */
} GetoptInfo, *GetoptInfoPtr;

#if (defined(__APPLE_CC__)) || (defined(MACOSX))
#else
	/* These definitions are here to preserve backwards-compatibility
	 * with code which was designed using an earlier version of the
	 * library.
	 */
#	define Line FTPLine
#	define LinePtr FTPLinePtr
#	define LineList FTPLineList
#	define LineListPtr FTPLineListPtr
#	define FileInfo FTPFileInfo
#	define FileInfoPtr FTPFileInfoPtr
#	define FileInfoList FTPFileInfoList
#	define FileInfoListPtr FTPFileInfoListPtr
#	define FileInfoListVec FTPFileInfoListVec
#endif

extern const char gLibNcFTPVersion[];
extern const char *gLibNcFTP_Uses_Me_To_Quiet_Variable_Unused_Warnings;

#if (defined(__APPLE_CC__)) && (__APPLE_CC__ < 10000)
#	define LIBNCFTP_USE_VAR(a) gLibNcFTP_Uses_Me_To_Quiet_Variable_Unused_Warnings = (const char *) (&a)
#	ifndef UNUSED
#		define UNUSED(a) a
#	endif
#elif (defined(__GNUC__)) && (__GNUC__ >= 3) && (! defined(__cplusplus))
#	ifndef UNUSED
#		define UNUSED(a) a __attribute__ ((__unused__))
#	endif
#	define LIBNCFTP_USE_VAR(a)
#elif (defined(__GNUC__)) && (__GNUC__ == 2) && (! defined(__cplusplus))
#	ifndef UNUSED
#		define UNUSED(a) a __attribute__ ((unused))
#	endif
#	define LIBNCFTP_USE_VAR(a)
#else
#	define LIBNCFTP_USE_VAR(a) gLibNcFTP_Uses_Me_To_Quiet_Variable_Unused_Warnings = (const char *) (&a)
#	ifndef UNUSED
#		define UNUSED(a) a
#	endif
#endif

/* Public routines */
void FTPAbortDataTransfer(const FTPCIPtr cip);
int FTPChdir(const FTPCIPtr cip, const char *const cdCwd);
int FTPChdirAndGetCWD(const FTPCIPtr cip, const char *const cdCwd, char *const newCwd, const size_t newCwdSize);
int FTPChdir3(FTPCIPtr cip, const char *const cdCwd, char *const newCwd, const size_t newCwdSize, int flags);
int FTPChdirList(FTPCIPtr cip, FTPLineListPtr const cdlist, char *const newCwd, const size_t newCwdSize, int flags);
int FTPChmod(const FTPCIPtr cip, const char *const pattern, const char *const mode, const int doGlob);
int FTPCloseHost(const FTPCIPtr cip);
int FTPCmd(const FTPCIPtr cip, const char *const cmdspec, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;
int FTPDecodeHostName(const FTPCIPtr cip, const char *const hstr0);
int FTPDecodeURL(const FTPCIPtr cip, char *const url, FTPLineListPtr cdlist, char *const fn, const size_t fnsize, int *const xtype, int *const wantnlst);
int FTPDelete(const FTPCIPtr cip, const char *const pattern, const int recurse, const int doGlob);
int FTPFileExists(const FTPCIPtr cip, const char *const file);
int FTPFileModificationTime(const FTPCIPtr cip, const char *const file, time_t *const mdtm);
int FTPFileSize(const FTPCIPtr cip, const char *const file, longest_int *const size, const int type);
longest_int FTPLocalASCIIFileSize(const char *const fn, char *buf, const size_t bufsize);
int FTPFileSizeAndModificationTime(const FTPCIPtr cip, const char *const file, longest_int *const size, const int type, time_t *const mdtm);
int FTPFileType(const FTPCIPtr cip, const char *const file, int *const ftype);
int FTPFtw(const FTPCIPtr cip, const FtwInfoPtr ftwip, const char *const path, FtwProc proc);
int FTPGetCWD(const FTPCIPtr cip, char *const newCwd, const size_t newCwdSize);
int FTPGetFileToMemory(const FTPCIPtr cip, const char *const file, char *memBuf, const size_t maxNumberOfBytesToWriteToMemBuf, size_t *const numberOfBytesWrittenToMemBuf, const longest_int startPoint, const int deleteflag);
int FTPGetFiles3(const FTPCIPtr cip, const char *pattern1, const char *const dstdir1, const int recurse, int doGlob, const int xtype, const int resumeflag, int appendflag, const int deleteflag, const int tarflag, const FTPConfirmResumeDownloadProc resumeProc, int UNUSED(reserved));
int FTPGetOneFile3(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int resumeflag, const int appendflag, const int deleteflag, const FTPConfirmResumeDownloadProc resumeProc, int UNUSED(reserved));
int FTPInitConnectionInfo(const FTPLIPtr lip, const FTPCIPtr cip, size_t bufsize);
int FTPInitLibrary(const FTPLIPtr lip);
int FTPIsDir(const FTPCIPtr cip, const char *const dir);
int FTPIsRegularFile(const FTPCIPtr cip, const char *const file);
int FTPList(const FTPCIPtr cip, const int outfd, const int longMode, const char *const lsflag);
int FTPListToMemory(const FTPCIPtr cip, const char *const pattern, const FTPLineListPtr llines, const char *const lsflags);
int FTPListToMemory2(const FTPCIPtr cip, const char *const pattern, const FTPLineListPtr llines, const char *const lsflags, const int blanklines, int *const tryMLSD);
int FTPLocalGlob(FTPCIPtr cip, FTPLineListPtr fileList, const char *pattern, int doGlob);
int FTPLoginHost(const FTPCIPtr cip);
int FTPMkdir(const FTPCIPtr cip, const char *const newDir, const int recurse);
int FTPMkdir2(const FTPCIPtr cip, const char *const newDir, const int recurse, const char *const curDir);
int FTPMkParentDir(const FTPCIPtr cip, const char *const path, const int recurse, const char *const curDir);
int FTPOpenHost(const FTPCIPtr cip);
int FTPOpenHostNoLogin(const FTPCIPtr cip);
void FTPPerror(const FTPCIPtr cip, const int err, const int eerr, const char *const s1, const char *const s2);
int FTPPutFileFromMemory(const FTPCIPtr cip, const char *volatile dstfile, const char *volatile src, const size_t srcLen, const int appendflag);
int FTPPutOneFile4(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int appendflag, const char *const tmppfx, const char *const tmpsfx, const int resumeflag, const int deleteflag, const FTPConfirmResumeUploadProc resumeProc, const time_t batchStartTime, const time_t origLmtime); 
int FTPPutFiles4(const FTPCIPtr cip, const char *const pattern, const char *const dstdir1, const int recurse, const int doGlob, const int xtype, const int appendflag, const char *const tmppfx, const char *const tmpsfx, const int resumeflag, const int deleteflag, const FTPConfirmResumeUploadProc resumeProc, const time_t batchStartTime, const time_t origLmtime);
int FTPReadLoginConfigFile(FTPCIPtr cip, const char *const fn);
int FTPRemoteGlob(FTPCIPtr cip, FTPLineListPtr fileList, const char *pattern, int doGlob);
int FTPRename(const FTPCIPtr cip, const char *const oldname, const char *const newname);
int FTPRmdir(const FTPCIPtr cip, const char *const pattern, const int recurse, const int doGlob);
void FTPShutdownHost(const FTPCIPtr cip);
const char *FTPStrError(int e);
char *FTPStrError2(const FTPCIPtr cip, int e, char *const dst, const size_t dstsize, int eerr);
int FTPSymlink(const FTPCIPtr cip, const char *const lfrom, const char *const lto);
int FTPUmask(const FTPCIPtr cip, const char *const umsk);
int FTPUtime(const FTPCIPtr cip, const char *const file, time_t actime, time_t modtime, time_t crtime);

/* FTPLineList routines */
int CopyLineList(FTPLineListPtr, FTPLineListPtr);
void DisposeLineListContents(FTPLineListPtr);
void InitLineList(FTPLineListPtr);
FTPLinePtr RemoveLine(FTPLineListPtr, FTPLinePtr);
FTPLinePtr AddLine(FTPLineListPtr, const char *);

/* Ftw routines */
void FtwInit(FtwInfo *const ftwip);
void FtwDispose(FtwInfo *const ftwip);
int Ftw(FtwInfo *const ftwip, const char *const path, FtwProc proc);
void FtwSetBuf(FtwInfo *const ftwip, char *const buf, const size_t bufsize, int autogrow);
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
DIR *opendir(const char *const path);
struct dirent *readdir(DIR *dir);
void closedir(DIR *dir);
#endif	/* WIN32 */

/* Getopt routines */
void GetoptReset(GetoptInfo *const opt);
int Getopt(GetoptInfo *const opt, int nargc, char **const nargv, const char *const ostr);

/* Other routines that might be useful. */
void InitWinsock(void);
void DisposeWinsock(void);
char *StrDup(const char *);
char *FGets(char *, size_t, FILE *);
void GetHomeDir(char *const dst, const size_t size);
void GetTmpDir(char *const dst, const size_t size);
void GetUsrName(char *, size_t);
void Scramble(unsigned char *dst, size_t dsize, unsigned char *src, char *key);
#ifndef tvcopy
#	define tvcopy(dst,src) dst.tv_sec = src.tv_sec; dst.tv_usec = src.tv_usec
#endif
double FTPDuration(struct timeval *const t0);
double FTPDuration2(struct timeval *const xt0, struct timeval *const xt1);
struct tm *Gmtime(time_t t, struct tm *const tp);
struct tm *Localtime(time_t t, struct tm *const tp);
time_t GetUTCOffset(const int mon, const int mday);
time_t GetUTCOffset2(const int year, const int mon, const int mday, const int hour, const int min);
time_t UnMDTMDate(char *);
int MkDirs(const char *const, int mode1);
char *GetPass(const char *const prompt, char *const pwbuf, const size_t pwbufsize);
int FilenameExtensionIndicatesASCII(const char *const pathName, const char *const extnList);
void StrRemoveTrailingSlashes(char *dst);
int StrToBoolOrInt(const char *s);
int FTPFixPrivateAddr(struct sockaddr_in *maybePrivateAddr, struct sockaddr_in *knownNonPrivateAddrToUseIfNeeded);
#if defined(WIN32) || defined(_WINDOWS) || defined(__CYGWIN__)
char *StrFindLocalPathDelim(const char *src);
char *StrRFindLocalPathDelim(const char *src);
void StrRemoveTrailingLocalPathDelim(char *dst);
void TVFSPathToLocalPath(char *dst);
void LocalPathToTVFSPath(char *dst);
#endif
int PathBuild(char *const dst, const size_t dsize, size_t *const dResultingLength, int dirSep, const char *const parentDir, const char *const fileName);
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
int gettimeofday(struct timeval *const tp, void *junk);
void WinSleep(unsigned int seconds);
#endif

#ifdef HAVE_SIGACTION
void (*NcSignal(int signum, void (*handler)(int)))(int);
#elif !defined(NcSignal)
#	define NcSignal signal
#endif

/* Obselete routines. */
int FTPGetOneFile(const FTPCIPtr cip, const char *const file, const char *const dstfile);
int FTPGetOneFile2(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int resumeflag, const int appendflag);
int FTPGetFiles(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob);
int FTPGetFiles2(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob, const int xtype, const int resumeflag, const int appendflag);
int FTPGetOneFileAscii(const FTPCIPtr cip, const char *const file, const char *const dstfile);
int FTPGetFilesAscii(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob);
int FTPPutOneFile(const FTPCIPtr cip, const char *const file, const char *const dstfile);
int FTPPutOneFile2(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int appendflag, const char *const tmppfx, const char *const tmpsfx);
int FTPPutOneFile3(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int appendflag, const char *const tmppfx, const char *const tmpsfx, const int resumeflag, const int deleteflag, const FTPConfirmResumeUploadProc resumeProc, int UNUSED(reserved));
int FTPPutFiles(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob);
int FTPPutFiles2(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob, const int xtype, const int appendflag, const char *const tmppfx, const char *const tmpsfx);
int FTPPutFiles3(const FTPCIPtr cip, const char *const pattern, const char *const dstdir1, const int recurse, const int doGlob, const int xtype, int appendflag, const char *const tmppfx, const char *const tmpsfx, const int resumeflag, const int deleteflag, const FTPConfirmResumeUploadProc resumeProc, int UNUSED(reserved));
int FTPPutOneFileAscii(const FTPCIPtr cip, const char *const file, const char *const dstfile);
int FTPPutFilesAscii(const FTPCIPtr cip, const char *const pattern, const char *const dstdir, const int recurse, const int doGlob);

/* Everything else below are private routines, or stuff for testing */
int FTPInitConnectionInfo2(const FTPLIPtr lip, const FTPCIPtr cip, char *const buf, size_t bufSize);
int FTPSendPort(const FTPCIPtr cip, struct sockaddr_in *saddr);
int FTPSendPassive(const FTPCIPtr cip, struct sockaddr_in *saddr, int *weird);
int FTPSetStartOffset(const FTPCIPtr cip, longest_int restartPt);
void FTPCloseControlConnection(const FTPCIPtr cip);
int FTPSendCommandStr(const FTPCIPtr cip, char *const command, const size_t siz);
int FTPSendCommand(const FTPCIPtr cip, const char *const cmdspec, va_list ap)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 0)))
#endif
;
char *FTPGetLocalCWD(char *buf, size_t size);
int FTPQueryFeatures(const FTPCIPtr);
void FTPManualOverrideFeatures(const FTPCIPtr cip);
int FTPMListOneFile(const FTPCIPtr cip, const char *const file, const MLstItemPtr mlip);
void FTPInitializeAnonPassword(const FTPLIPtr);
void FTPInitIOTimer(const FTPCIPtr);
int FTPStartDataCmd(const FTPCIPtr, const int, const int, const longest_int, const char *const ,...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 5, 6)))
#endif
;
int FTPStartDataCmd2(const FTPCIPtr cip, const int netMode, const int type, const longest_int startPoint, char *const cmdstr, const size_t cmdstrSize, const char *variableCommandSpec, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 7, 8)))
#endif
;
int FTPStartDataCmd3(const FTPCIPtr cip, const int netMode, const int type, const longest_int startPoint, char *const cmdstr, const size_t cmdstrSize, const char *const variableCommandSpec, va_list ap)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 7, 0)))
#endif
;
void FTPStartIOTimer(const FTPCIPtr);
void FTPStopIOTimer(const FTPCIPtr);
void FTPUpdateIOTimer(const FTPCIPtr);
int FTPSetTransferType(const FTPCIPtr, int);
int FTPEndDataCmd(const FTPCIPtr, int);
int FTPRemoteHelp(const FTPCIPtr, const char *const, const FTPLineListPtr);
int FTPCmdNoResponse(const FTPCIPtr, const char *const cmdspec,...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;
void FTPInitialLogEntry(const FTPCIPtr cip);
int WaitResponse(const FTPCIPtr, unsigned int);

/* We suggest using the Ftw() routines rather than these two. */
int FTPLocalRecursiveFileList(FTPCIPtr, FTPLineListPtr, FTPFileInfoListPtr);
int FTPLocalRecursiveFileList2(FTPCIPtr cip, FTPLineListPtr fileList, FTPFileInfoListPtr files, int erelative);

/* FTPFtw() is recommended since it is more reliable (read: thorough),
 * but these two work faster.
 */
int FTPRemoteRecursiveFileList(FTPCIPtr, FTPLineListPtr, FTPFileInfoListPtr);
int FTPRemoteRecursiveFileList1(FTPCIPtr, char *const, FTPFileInfoListPtr);
int FTPRemoteFtwProc(const FtwInfoPtr ftwip);
int FTPRemoteRecursiveFileList2(FTPCIPtr cip, char *const rdir, FTPFileInfoListPtr files);

int FTPRebuildConnectionInfo(const FTPLIPtr lip, const FTPCIPtr cip);

/* FTPFileExists() calls these as needed. */
int FTPFileExistsStat(const FTPCIPtr cip, const char *const file);
int FTPFileExistsNlst(const FTPCIPtr cip, const char *const file);
int FTPFileExists2(const FTPCIPtr cip, const char *const file, const int tryMDTM, const int trySIZE, const int tryMLST, const int trySTAT, const int tryNLST);

void FTPGetDateStr(time_t t, const char *fmt, char *const ltstr1, const size_t ltstr1size, char *const gtstr1, const size_t gtstr1size);
int BindToEphemeralPortNumber(const int sockfd, struct sockaddr_in *const addrp, const int ephemLo, const int ephemHi);
int BufferGets(char *, size_t, int, char *, char **, char **, size_t);
void DisposeFileInfoListContents(FTPFileInfoListPtr);
void InitFileInfoList(FTPFileInfoListPtr);
void InitFileInfo(FTPFileInfoPtr);
FTPFileInfoPtr RemoveFileInfo(FTPFileInfoListPtr, FTPFileInfoPtr);
FTPFileInfoPtr AddFileInfo(FTPFileInfoListPtr, FTPFileInfoPtr);
void SortFileInfoList(FTPFileInfoListPtr, int, int);
void VectorizeFileInfoList(FTPFileInfoListPtr);
void UnvectorizeFileInfoList(FTPFileInfoListPtr);
int IsValidUNCPath(const char *const src);
void CompressPath(char *const dst, const char *const src, const size_t dsize, int dosCompat);
void PathCat(char *const dst, const size_t dsize, const char *const cwd, const char *const src, int dosCompat);
int DPathCat(char **const dst0, const char *const cwd, const char *const src, int dosCompat);
int ComputeRNames(FTPFileInfoListPtr, const char *, int, int);
int ComputeLNames(FTPFileInfoListPtr, const char *, const char *, int);
int ConcatFileInfoList(FTPFileInfoListPtr, FTPFileInfoListPtr);
int ConcatFileToFileInfoList(FTPFileInfoListPtr, char *);
int LineListToFileInfoList(FTPLineListPtr, FTPFileInfoListPtr);
int LineToFileInfoList(FTPLinePtr, FTPFileInfoListPtr);
void URLCopyToken(char *, size_t, const char *, size_t);
int UnMlsT(const FTPCIPtr, const char *const, const MLstItemPtr);
int UnMlsD(const FTPCIPtr, FTPFileInfoListPtr, FTPLineListPtr);
int UnLslR(const FTPCIPtr, FTPFileInfoListPtr, FTPLineListPtr, int);
void TraceResponse(const FTPCIPtr, ResponsePtr);
void PrintResponse(const FTPCIPtr, FTPLineListPtr);
void DoneWithResponse(const FTPCIPtr, ResponsePtr);
ResponsePtr InitResponse(void);
void ReInitResponse(const FTPCIPtr, ResponsePtr);
int GetTelnetString(const FTPCIPtr, char *, size_t, FILE *, FILE *);
int GetResponse(const FTPCIPtr, ResponsePtr);
int RCmd(const FTPCIPtr, ResponsePtr, const char *, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 3, 4)))
#endif
;
void PrintF(const FTPCIPtr cip, const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;
void FTPLogError(const FTPCIPtr cip, const int pError, const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 3, 4)))
#endif
;

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif /* _ncftp_h_ */
