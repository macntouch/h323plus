/*
 * h224handler.h
 *
 * H.224 protocol handler implementation for the OpenH323 Project.
 *
 * Copyright (c) 2006 Network for Educational Technology, ETH Zurich.
 * Written by Hannes Friederich.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h224handler.h,v $
 * Revision 1.3  2011/01/12 12:51:52  shorne
 * H.224 bi-directional support added
 *
 * Revision 1.2  2008/05/23 11:19:21  willamowius
 * switch BOOL to PBoolean to be able to compile with Ptlib 2.2.x
 *
 * Revision 1.1  2007/08/06 20:50:48  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:22  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.2  2006/04/23 18:52:19  dsandras
 * Removed warnings when compiling with gcc on Linux.
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#ifndef __H323_H224HANDLER_H
#define __H323_H224HANDLER_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
#include <rtp.h>
#include <h281handler.h>
#include <channels.h>

#define H281_CLIENT_ID 0x01

class H224_Frame;
class OpalH224Handler;

class OpalH224ReceiverThread : public PThread
{
  PCLASSINFO(OpalH224ReceiverThread, PThread);
	
public:
	
  OpalH224ReceiverThread(OpalH224Handler *h224Handler, RTP_Session & rtpSession);
  ~OpalH224ReceiverThread();
	
  virtual void Main();
	
  void Close();
	
private:
		
  OpalH224Handler *h224Handler;
  mutable PMutex inUse;
  unsigned timestamp;
  RTP_Session & rtpSession;
  PBoolean terminate;
};

class OpalH281Handler;
class H323Connection;

class OpalH224Handler : public PObject
{
  PCLASSINFO(OpalH224Handler, PObject);
	
public:
	
  OpalH224Handler(H323Channel::Directions dir, H323Connection & connection, unsigned sessionID);
  ~OpalH224Handler();
	
  virtual void StartTransmit();
  virtual void StopTransmit();
  virtual void StartReceive();
  virtual void StopReceive();
	
  PBoolean SendClientList();
  PBoolean SendExtraCapabilities();
  PBoolean SendClientListCommand();
  PBoolean SendExtraCapabilitiesCommand(BYTE clientID);

  PBoolean SendExtraCapabilitiesMessage(BYTE clientID, BYTE *data, PINDEX length);

  PBoolean TransmitClientFrame(BYTE clientID, H224_Frame & frame);
	
  virtual PBoolean OnReceivedFrame(H224_Frame & frame);
  virtual PBoolean OnReceivedCMEMessage(H224_Frame & frame);
  virtual PBoolean OnReceivedClientList(H224_Frame & frame);
  virtual PBoolean OnReceivedClientListCommand();
  virtual PBoolean OnReceivedExtraCapabilities(H224_Frame & frame);
  virtual PBoolean OnReceivedExtraCapabilitiesCommand();
	
  PMutex & GetTransmitMutex() { return transmitMutex; }
	
  RTP_Session * GetSession() const { return session; }
	
  virtual OpalH224ReceiverThread * CreateH224ReceiverThread();
	
  OpalH281Handler *GetH281Handler() { return h281Handler; }

  H323Channel::Directions GetDirection() { return sessionDirection; }
	
protected:

  RTP_Session * session;

  PBoolean canTransmit;
  PMutex transmitMutex;
  RTP_DataFrame *transmitFrame;
  BYTE transmitBitIndex;
  PTime *transmitStartTime;
	
  OpalH224ReceiverThread *receiverThread;
	
  OpalH281Handler *h281Handler;
  H323Channel::Directions sessionDirection;
	
private:
		
  void TransmitFrame(H224_Frame & frame);
	
};

#endif // __H323_H224HANDLER_H

