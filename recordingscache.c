/*
 *      vdr-plugin-vnsi - XBMC server plugin for VDR
 *
 *      Copyright (C) 2011 Alexander Pipelka
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "config.h"
#include "recordingscache.h"
#include "vnsiclient.h"
#include "hash.h"

cRecordingsCache::cRecordingsCache() {
}

cRecordingsCache::~cRecordingsCache() {
}

cRecordingsCache& cRecordingsCache::GetInstance() {
  static cRecordingsCache singleton;
  return singleton;
}

uint32_t cRecordingsCache::Register(cRecording* recording, bool deleted) {
  cString filename = recording->FileName();
  uint32_t uid = CreateStringHash(filename);

  m_mutex.Lock();
  if(m_recordings.find(uid) == m_recordings.end())
  {
    DEBUGLOG("%s - uid: %08x '%s'", __FUNCTION__, uid, (const char*)filename);
    m_recordings[uid].filename = filename;
    m_recordings[uid].isDeleted = deleted;
  }
  m_mutex.Unlock();

  return uid;
}

cRecording* cRecordingsCache::Lookup(uint32_t uid) {
  DEBUGLOG("%s - lookup uid: %08x", __FUNCTION__, uid);

  if(m_recordings.find(uid) == m_recordings.end()) {
    DEBUGLOG("%s - not found !", __FUNCTION__);
    return NULL;
  }

  m_mutex.Lock();
  cString filename = m_recordings[uid].filename;
  DEBUGLOG("%s - filename: %s", __FUNCTION__, (const char*)filename);

  cRecording* r;
  if (!m_recordings[uid].isDeleted)
    r = Recordings.GetByName(filename);
  else
    r = DeletedRecordings.GetByName(filename);

  DEBUGLOG("%s - recording %s", __FUNCTION__, (r == NULL) ? "not found !" : "found");
  m_mutex.Unlock();

  return r;
}
