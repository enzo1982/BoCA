/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */
/*                Modified on 2000.05.25 by N. Iwakami */
/*                Released on 2000.09.06 by N. Iwakami */

#ifndef tvqdec_h
#define tvqdec_h

#ifdef _MSC_VER
#	ifdef DLL_MODULE
#		define DllPort    __declspec( dllexport )
#	else
#		define DllPort    __declspec( dllimport )
#	endif
#else
#	define DllPort
#endif

#ifdef __cplusplus
extern "C" {  // only need to import/export C interface if used by C++ source code
#endif

// TwinVQ decoder initialization/termination functions
DllPort int  TvqInitialize( headerInfo *setupInfo, INDEX *index, int dispErrorMessageBox );
DllPort void TvqTerminate( INDEX *index );
DllPort void TvqGetVectorInfo(int *bits0[], int *bits1[]);
DllPort void TvqResetFrameCounter();

// TwinVQ decoder function
DllPort void TvqDecodeFrame(INDEX  *indexp, float out[]);
DllPort int  TvqWtypeToBtype( int w_type, int *btype );
DllPort void TvqUpdateVectorInfo(int varbits, int *ndiv, int bits0[], int bits1[]);
DllPort void TvqSetFrameCounter( int position );

// TwinVQ query functions
DllPort int   TvqCheckVersion(char *versionID);
DllPort void  TvqGetSetupInfo(headerInfo *setupInfo); // setup information
DllPort void  TvqGetConfInfo(tvqConfInfo *cf);  // configuration information
DllPort int   TvqGetFrameSize();   // frame size
DllPort int   TvqGetNumChannels(); // number of channels
DllPort int   TvqGetBitRate();                        // total bitrate
DllPort float TvqGetSamplingRate();                   // sampling rate
DllPort int   TvqGetNumFixedBitsPerFrame();           // number of fixed bits per frame
DllPort int   TvqGetNumFrames();   // number of decoded frame
DllPort int   TvqGetModuleVersion( char* versionString );

#ifdef V2PLUS_SUPPORT
// TwinVQ FB coding tool control
DllPort void  TvqFbCountUsedBits(int nbit);  // count number of used bits 
DllPort float TvqGetFbCurrentBitrate();  // query average bitrate for the tool
DllPort int   TvqGetFbTotalBits();  // query total number of used bits 
#endif

#ifdef __cplusplus
}
#endif


#endif
