

#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H


//if build release version lib
//#define KERNEL_RELEASE 0


//kernel version
#define KERNEL_VERSION_NUM "1.1.9"

#if (defined(DEBUG_MODE))
	#define KERNEL_VERSION (KERNEL_VERSION_NUM##"_debug")
#else
	#define KERNEL_VERSION (KERNEL_VERSION_NUM)
#endif

//if contain hardware test audio file
//#define HARDWARE_TEST 0

//if contain full audio file
//#define AUDIO_FILE_INCLUDE 0

#define KERNEL_RELEASE 0

//build lib
#if KERNEL_RELEASE==0
	#define BUILD_LIB 0		//dont build a lib
#else
	#define BUILD_LIB 1		//build a lib in release version
#endif

//enable log output
#if KERNEL_RELEASE==0
	#define LIB_DEBUG_ENABLE 1	//debug version has log output
#else
	#define LIB_DEBUG_ENABLE 0 	//release version has no log output
#endif

#ifndef PCI_ENABLE
#define CERT_DEFAULT_TYPE_UROVO
#else
#define CERT_DEFAULT_TYPE_PCI
#endif

//#define SUPPORT_TRIG

#define SUPPORT_ADMIN

// mcu platform.
#define __MH1903__

#if (KERNEL_RELEASE==0)||(defined(DEBUG_MODE))
#undef SUPPORT_TRIG		//dont use trig in debug mode
#undef SUPPORT_ADMIN
#endif

#endif

