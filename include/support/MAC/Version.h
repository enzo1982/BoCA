#pragma once

/* build batch file */
/*
#ifdef APE_BATCH_FILE_VERSION
Set _MA=1137
Set _MAV=1137
#endif
*/

/* major version number */
#define APE_VERSION_MAJOR 11

/* build version number */
#define APE_VERSION_REVISION 37
#define APE_VERSION_REVISION_NUMBER 37 // needed because a number like 08 is interpreted as octal

/* library interface version, update this whenever the signature of an exported function changes */
#define APE_INTERFACE_VERSION 14

/* leave this so the end of file doesn't get truncated */
