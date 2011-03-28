/****************************************************************************
 * Visual Boy Advance GX
 *
 * Tantric December 2008
 *
 * networkop.h
 *
 * Network and SMB support routines
 ****************************************************************************/

#ifndef _NETWORKOP_H_
#define _NETWORKOP_H_

#ifdef HW_RVL
void StartNetworkThread();
bool InitializeNetwork(bool silent);
bool ConnectShare (bool silent);
void CloseShare();
#endif

#endif
