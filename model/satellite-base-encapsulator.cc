/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jani Puttonen <jani.puttonen@magister.fi>
 */


#include "ns3/log.h"
#include "ns3/simulator.h"
#include "satellite-queue.h"
#include "satellite-time-tag.h"
#include "satellite-mac-tag.h"
#include "satellite-base-encapsulator.h"


NS_LOG_COMPONENT_DEFINE ("SatBaseEncapsulator");

namespace ns3 {


NS_OBJECT_ENSURE_REGISTERED (SatBaseEncapsulator);


SatBaseEncapsulator::SatBaseEncapsulator ()
:m_sourceAddress (),
 m_destAddress (),
 m_flowId (0)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (true);
}

SatBaseEncapsulator::SatBaseEncapsulator (Mac48Address source, Mac48Address dest, uint8_t flowId)
:m_sourceAddress (source),
 m_destAddress (dest),
 m_flowId (flowId)
{
  NS_LOG_FUNCTION (this);
}

SatBaseEncapsulator::~SatBaseEncapsulator ()
{
  NS_LOG_FUNCTION (this);
  m_txQueue = NULL;
  m_rxCallback.Nullify ();
}

TypeId SatBaseEncapsulator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SatBaseEncapsulator")
    .SetParent<Object> ()
    .AddTraceSource ("TxPDU",
                     "PDU transmission notified to the MAC.",
                     MakeTraceSourceAccessor (&SatBaseEncapsulator::m_txPdu))
    .AddTraceSource ("RxPDU",
                     "PDU received.",
                     MakeTraceSourceAccessor (&SatBaseEncapsulator::m_rxPdu))
    ;
  return tid;
}

void
SatBaseEncapsulator::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  if (m_txQueue)
    {
      m_txQueue->DoDispose ();
    }
  m_rxCallback.Nullify ();
}

void
SatBaseEncapsulator::TransmitPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p->GetSize ());

  // Store packet arrival time
  SatTimeTag timeTag (Simulator::Now ());
  p->AddPacketTag (timeTag);

  NS_LOG_LOGIC ("Tx Buffer: New packet added of size: " << p->GetSize ());

  if (!m_txQueue->Enqueue (p))
    {
      NS_LOG_LOGIC ("Packet is dropped!");
    }

  NS_LOG_LOGIC ("NumPackets = " << m_txQueue->GetNPackets() );
  NS_LOG_LOGIC ("NumBytes = " << m_txQueue->GetNBytes ());
}


Ptr<Packet>
SatBaseEncapsulator::NotifyTxOpportunity (uint32_t bytes, uint32_t &bytesLeft)
{
  NS_LOG_FUNCTION (this << bytes << bytesLeft);

  Ptr<Packet> packet;

  // No packets in buffer
  if ( m_txQueue->GetNPackets () == 0 )
    {
      NS_LOG_LOGIC ("No data pending, return NULL packet");
      return packet;
    }

  // Peek the first PDU from the buffer.
  Ptr<const Packet> peekPacket = m_txQueue->Peek ();

  if (peekPacket->GetSize () <= bytes)
    {
      // Peek the first PDU from the buffer.
      packet = m_txQueue->Dequeue ();
    }
  else
    {
      NS_FATAL_ERROR ("Packet of size: " << peekPacket->GetSize () << " does not fit into allocation of: " << bytes << " Bytes!");
    }

  // Add MAC tag to identify the packet in lower layers
  SatMacTag mTag;
  mTag.SetDestAddress (m_destAddress);
  mTag.SetSourceAddress (m_sourceAddress);
  packet->AddPacketTag (mTag);

  SatRcIndexTag rcTag;
  rcTag.SetRcIndex (m_flowId);
  packet->AddPacketTag (rcTag);

  // Update bytes left
  bytesLeft = m_txQueue->GetNBytes ();
  return packet;
}


void
SatBaseEncapsulator::ReceivePdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (true);

  /**
   * The base encapsulator should not be used at receiving packets
   * at all, since all the packets are terminated currently already
   * in lower layers
   */
}

uint32_t
SatBaseEncapsulator::GetTxBufferSizeInBytes () const
{
  NS_LOG_FUNCTION (this);

  return m_txQueue->GetNBytes ();
}

Time
SatBaseEncapsulator::GetHolDelay () const
{
  NS_LOG_FUNCTION (this);

  Time delay (Seconds (0.0));

  if (m_txQueue->GetNPackets () > 0)
    {
      // Peek the first PDU from the buffer.
      Ptr<const Packet> peekPacket = m_txQueue->Peek ();

      SatTimeTag timeTag;
      peekPacket->PeekPacketTag (timeTag);

      delay = Simulator::Now () - timeTag.GetSenderTimestamp ();
    }
  return delay;
}

void
SatBaseEncapsulator::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this << &cb);

  m_rxCallback = cb;
}

void
SatBaseEncapsulator::SetQueue (Ptr<SatQueue> queue)
{
  NS_LOG_FUNCTION (this);

  m_txQueue = queue;
}

Ptr<SatQueue>
SatBaseEncapsulator::GetQueue ()
{
  NS_LOG_FUNCTION (this);

  return m_txQueue;
}

uint32_t
SatBaseEncapsulator::GetMinTxOpportunityInBytes () const
{
  NS_LOG_FUNCTION (this);

  return m_txQueue->Peek ()->GetSize ();
}



} // namespace ns3