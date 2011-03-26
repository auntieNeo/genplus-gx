/****************************************************************************
 * Visual Boy Advance GX
 *
 * Tantric December 2008
 *
 * networkop.cpp
 *
 * Network and SMB support routines
 ****************************************************************************/

#include <network.h>
#include <malloc.h>
#include <ogc/lwp_watchdog.h>
#include <smb.h>
//#include <mxml.h>

#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <fat.h>
#include <unistd.h>
#include <errno.h>

#include "gx_video.h"
#include "gui.h"

static bool networkInit = false;
static bool networkShareInit = false;
char wiiIP[16] = { 0 };

#ifdef HW_RVL
static int netHalt = 0;

bool isSmbMounted = false;

/****************************************************************************
 * InitializeNetwork
 * Initializes the Wii/GameCube network interface
 ***************************************************************************/

static lwp_t networkthread = LWP_THREAD_NULL;
static u8 netstack[32768] ATTRIBUTE_ALIGN (32);

static void * netcb (void *arg)
{
	s32 res=-1;
	int retry;
	int wait;
	static bool prevInit = false;

	while(netHalt != 2)
	{
		retry = 5;
		
		while (retry>0 && (netHalt != 2))
		{			
			if(prevInit) 
			{
				bool reset=false;
				int i;
				for(i=0; i < 500 && (netHalt != 2); i++) // 10 seconds to try to reset
				{
					res = net_get_status();
					if(res != -EBUSY) // trying to init net so we can't kill the net
					{
						usleep(2000);
						net_wc24cleanup(); //kill the net 
						reset=true;
						break;					
					}
					usleep(20000);
				}
				if(!reset) 
				{
					retry--;
					continue;
				}
			}

			net_deinit();			
			usleep(2000);
			res = net_init_async(NULL, NULL);

			if(res != 0)
			{
				sleep(1);
				retry--;
				continue;
			}

			res = net_get_status();
			wait = 400; // only wait 8 sec
			while (res == -EBUSY && wait > 0  && (netHalt != 2))
			{
				usleep(20000);
				res = net_get_status();
				wait--;
			}

			if(res==0) break;
			retry--;
			usleep(2000);
		}
		if (res == 0)
		{
			struct in_addr hostip;
			hostip.s_addr = net_gethostip();
			if (hostip.s_addr)
			{
				strcpy(wiiIP, inet_ntoa(hostip));
				networkInit = true;	
				prevInit = true;
			}
		}
		if(netHalt != 2) LWP_SuspendThread(networkthread);
	}
	return NULL;
}

/****************************************************************************
 * StartNetworkThread
 *
 * Signals the network thread to resume, or creates a new thread
 ***************************************************************************/
void StartNetworkThread()
{
	netHalt = 0;

	if(networkthread == LWP_THREAD_NULL)
		LWP_CreateThread(&networkthread, netcb, NULL, netstack, 8192, 40);
	else
		LWP_ResumeThread(networkthread);
}

/****************************************************************************
 * StopNetworkThread
 *
 * Signals the network thread to stop
 ***************************************************************************/
void StopNetworkThread()
{
	if(networkthread == LWP_THREAD_NULL || !LWP_ThreadIsSuspended(networkthread))
		return;

	netHalt = 2;
	LWP_ResumeThread(networkthread);

	// wait for thread to finish
	LWP_JoinThread(networkthread, NULL);
	networkthread = LWP_THREAD_NULL;
}

#endif

bool InitializeNetwork(bool silent)
{
#ifdef HW_RVL
	StopNetworkThread();

	if(networkInit && net_gethostip() > 0)
		return true;

	networkInit = false;
#else
	if(networkInit)
		return true;
#endif

	int retry = 1;

	while(retry)
	{
		GUI_MsgBoxOpen("Network", "Initializing network...", true);

#ifdef HW_RVL
		u64 start = gettime();
		StartNetworkThread();

		while (!LWP_ThreadIsSuspended(networkthread))
		{
			usleep(50 * 1000);

			if(diff_sec(start, gettime()) > 10) // wait for 10 seconds max for net init
				break;
		}
#else
		networkInit = !(if_config(wiiIP, NULL, NULL, true) < 0);
#endif

		GUI_MsgBoxClose();

		if(networkInit || silent)
			break;

		retry = false; GUI_WaitPrompt("Error", "Unable to initialize network!");  // TODO: allow the user to retry
		
#ifdef HW_RVL  	
		if(networkInit && net_gethostip() > 0)
#else
		if(networkInit)
#endif
			return true;
	}
	return networkInit;
}

void CloseShare()
{
	if(networkShareInit)
		smbClose("smb");
	networkShareInit = false;
	isSmbMounted = false;
}

/****************************************************************************
 * Mount SMB Share
 ****************************************************************************/

bool
ConnectShare (bool silent)
{
	if(!InitializeNetwork(silent))
		return false;

	if(networkShareInit)
		return true;

	int retry = 1;
	int chkS = (strlen("public") > 0) ? 0:1;  // FIXME: make this configurable
	int chkI = (strlen("192.168.1.104") > 0) ? 0:1;  // FIXME

	// check that all parameters have been set
	if(chkS + chkI > 0)
	{
		if(!silent)
		{
			char msg[50];
			char msg2[100];
			if(chkS + chkI > 1) // more than one thing is wrong
				sprintf(msg, "Check settings.xml.");
			else if(chkS)
				sprintf(msg, "Share name is blank.");
			else if(chkI)
				sprintf(msg, "Share IP is blank.");

			sprintf(msg2, "Invalid network settings - %s", msg);
			GUI_WaitPrompt("Error", msg2);
		}
		return false;
	}

	while(retry)
	{
		if(!silent)
			GUI_MsgBoxOpen("Network", "Connecting to network share...", true);
		
		if(smbInit("nobody", "password", "public", "192.168.1.104"))  // TODO: allow the user to configure these settings
			networkShareInit = true;

		if(networkShareInit || silent)
			break;

		retry = false; GUI_WaitPrompt("Error", "Failed to connect to network share.");  // TODO: allow the user to retry
	}

	if(!silent)
		;//GUI();  // FIXME: call the GUI loop I guess

	return networkShareInit;
}
