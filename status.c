/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2005-2014 Team XBMC
 *      Copyright (C) 2015 Team KODI
 *
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "status.h"
#include "vnsi.h"

#include <vdr/tools.h>
#include <vdr/recording.h>
#include <vdr/videodir.h>
#include <vdr/shutdown.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <algorithm>
#include <thread>

namespace {

class ChannelsState
{
public:
	ChannelsState()
		: mChannels( cChannels::GetChannelsRead(mChanState) )
	{
		mChanState.Remove(false);
	}

	bool
	isModified()
	{
		bool modified = mChannels->Lock(mChanState);
		if ( modified )
		{
			mChanState.Remove(false);
		}
		return modified;
	}

private:
	cStateKey 			mChanState;
	const cChannels*	mChannels;
};

class RecordingsState
{
public:
	RecordingsState()
		: mRecordings( cRecordings::GetRecordingsRead(mRecState) )
	{
		mRecState.Remove(false);
	}

	bool
	isModified()
	{
		bool modified = mRecordings->Lock(mRecState);
		if ( modified )
		{
			mRecState.Remove(false);
		}
		return modified;
	}
private:
	cStateKey 			mRecState;
	const cRecordings*	mRecordings;
};

class TimersState
{
public:
	TimersState()
		: mTimers(cTimers::GetTimersRead(mState))
	{
		mState.Remove(false);
	}

	bool
	isModified()
	{
		bool modified = mTimers->Lock(mState);
		if ( modified )
		{
			mState.Remove(false);
		}
		return modified;
	}
private:
	cStateKey 		mState;
	const cTimers* 	mTimers;
};

class VNSITimersState
{
public:
	VNSITimersState( CVNSITimers& timers )
		: mTimers( timers )
		, mState( -1 )
	{
		mTimers.StateChange( mState );
	}

	bool
	isModified()
	{
		return mTimers.StateChange( mState );
	}

private:
	CVNSITimers& 	mTimers;
	int 			mState;
};

class EPGState
{
public:
	EPGState()
		: mEpg( cSchedules::GetSchedulesRead(mState) )
	{
		mState.Remove(false);
	}

	bool
	isModified()
	{
		bool modified = mEpg->Lock(mState);
		if ( modified )
		{
			mState.Remove(false);
		}
		return modified;
	}

	void
	reset()
	{
		mState.Reset();
	}
private:
	cStateKey 			mState;
	const cSchedules*	mEpg;
};

} // end namespace

cVNSIStatus::cVNSIStatus() : cThread("VNSIStatus")
{
}

cVNSIStatus::~cVNSIStatus()
{
  Shutdown();
}

void cVNSIStatus::Init(CVNSITimers *timers)
{
  m_vnsiTimers = timers;
  Start();
}

void cVNSIStatus::Shutdown()
{
	// Stop the thread
	Cancel(5);

	// No need to lock. The thread is already stopped
	m_clients.clear();
}

static bool CheckFileSuffix(const char *name,
                            const char *suffix, size_t suffix_length)
{
  size_t name_length = strlen(name);
  return name_length > suffix_length &&
    memcmp(name + name_length - suffix_length, suffix, suffix_length) == 0;
}

static void DeleteFiles(const char *directory_path, const char *suffix)
{
  const size_t suffix_length = strlen(suffix);

  DIR *dir = opendir(directory_path);
  if (dir == nullptr)
    return;

  std::string path(directory_path);
  path.push_back('/');
  const size_t start = path.size();

  while (auto *e = readdir(dir))
  {
    if (CheckFileSuffix(e->d_name, suffix, suffix_length))
    {
      path.replace(start, path.size(), e->d_name);

      if (unlink(path.c_str()) < 0)
      {
        ERRORLOG("Failed to delete %s: %s", path.c_str(), strerror(errno));
      }
    }
  }

  closedir(dir);
}

void cVNSIStatus::AddClient(int fd, unsigned int id, const char *ClientAdr, CVNSITimers &timers)
{
	std::unique_lock<std::mutex> lock( m_mutex );
	m_clients.push_back(std::make_shared<cVNSIClient>(fd, id, ClientAdr, timers));
}

void cVNSIStatus::Action(void)
{
	cTimeMs 			chanTimer;
	cTimeMs 			epgTimer;
	cTimeMs 			recTimer;

	ChannelsState 		channelState;
	RecordingsState 	recordingState;
	TimersState 		timersState;
	VNSITimersState 	vnsiTimersState(*m_vnsiTimers);
	EPGState 			epgState;

	// delete old timeshift file
	struct stat sb;
	if ((*TimeshiftBufferDir) && stat(TimeshiftBufferDir, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		DeleteFiles(TimeshiftBufferDir, ".vnsi");
	}
	else
	{
#if VDRVERSNUM >= 20102
		DeleteFiles(cVideoDirectory::Name(), ".vnsi");
#else
		DeleteFiles(VideoDirectory, ".vnsi");
#endif
	}

	// set thread priority
	SetPriority(1);

	while (Running())
	{
		std::unique_lock<std::mutex> lock( m_mutex );

		// remove disconnected clients
		std::list<cVNSIClientSharedPtr> connected;
		std::copy_if( m_clients.begin(), m_clients.end(), std::back_inserter(connected),
					  [](const cVNSIClientSharedPtr& client)
					  {
						  bool active = client->Active();
						  if ( !active )
						  {
							  INFOLOG("removing client with ID %u from client list", client->GetID());
						  }
						  return active;
					  });
		std::swap( m_clients, connected );
		lock.unlock();

		// Don't to updates during running channel scan, KODI's PVR manager becomes
		//restarted of finished scan.
		if (!cVNSIClient::InhibidDataUpdates())
		{
			// reset inactivity timeout as long as there are clients connected
			if (!m_clients.empty())
			{
				ShutdownHandler.SetUserInactiveTimeout();
			}

			// trigger clients to reload the modified channel list
			if (chanTimer.TimedOut())
			{
				if ( channelState.isModified() )
				{
					INFOLOG("Requesting clients to reload channels list");
					lock.lock();
					std::for_each( m_clients.begin(), m_clients.end(),
							       [](const cVNSIClientSharedPtr& client)
								   {
									   client->ChannelsChange();
								   } );
					lock.unlock();
				}
				chanTimer.Set(5000);
			}

			// trigger clients to reload the modified recordings list
			if (recTimer.TimedOut())
			{
				if ( recordingState.isModified() )
				{
					INFOLOG("Requesting clients to reload recordings list");
					lock.lock();
					std::for_each( m_clients.begin(), m_clients.end(),
							       [](const cVNSIClientSharedPtr& client)
								   {
						 	 	       client->ChannelsChange();
								   } );
					lock.unlock();
				}
				recTimer.Set(2500);
			}

			// trigger clients to reload the modified timers list
			if ( timersState.isModified() )
			{
				INFOLOG("Requesting clients to reload timers");
				lock.lock();
				std::for_each( m_clients.begin(), m_clients.end(),
						       [](const cVNSIClientSharedPtr& client)
							   {
							       client->SignalTimerChange();
							   } );
				lock.unlock();
			}

			if ( vnsiTimersState.isModified() )
			{
				INFOLOG("Requesting clients to reload timers");
				lock.lock();
				std::for_each( m_clients.begin(), m_clients.end(),
			   	               [](const cVNSIClientSharedPtr& client)
			   				   {
			  				       client->SignalTimerChange();
			   				   } );
				lock.unlock();
			}


			if (epgTimer.TimedOut())
			{
				if ( epgState.isModified() )
				{
					INFOLOG("Requesting clients to reload timers");
					lock.lock();
					std::for_each( m_clients.begin(), m_clients.end(),
					               [](const cVNSIClientSharedPtr& client)
								   {
					                   client->EpgChange();
								   } );
				    lock.unlock();
				}
				epgTimer.Set(5000);
				m_vnsiTimers->Scan();
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds(250) );
	}
}
