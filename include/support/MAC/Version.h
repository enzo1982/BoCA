#pragma once

/* build batch file */
/*
#ifdef APE_BATCH_FILE_VERSION
Set _MA=1235
Set _MAV=1235
#endif
*/

/* major version number */
#define APE_VERSION_MAJOR 12

/* build version number */
#define APE_VERSION_REVISION 35
#define APE_VERSION_REVISION_NUMBER 35 // needed because a number like 08 is interpreted as octal

/* library interface version, update this whenever the signature of an exported function changes */
#define APE_INTERFACE_VERSION 14

/* leave this so the end of file doesn't get truncated */
