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
 * Author: Frans Laakso <frans.laakso@magister.fi>
 */
#include "satellite-fading-output-trace-container.h"
#include "ns3/satellite-env-variables.h"
#include "ns3/satellite-helper.h"

NS_LOG_COMPONENT_DEFINE ("SatFadingOutputTraceContainer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SatFadingOutputTraceContainer);

TypeId 
SatFadingOutputTraceContainer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SatFadingOutputTraceContainer")
    .SetParent<SatBaseTraceContainer> ();
  return tid;
}

SatFadingOutputTraceContainer::SatFadingOutputTraceContainer () :
  m_currentWorkingDirectory ("")
{
  NS_LOG_FUNCTION (this);

  Ptr<SatEnvVariables> envVariables = CreateObject<SatEnvVariables> ();
  m_currentWorkingDirectory = envVariables->GetCurrentWorkingDirectory ();
}

SatFadingOutputTraceContainer::~SatFadingOutputTraceContainer ()
{
  NS_LOG_FUNCTION (this);

  Reset ();
}

void
SatFadingOutputTraceContainer::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  WriteToFile ();

  Reset ();

  SatBaseTraceContainer::DoDispose ();
}

void
SatFadingOutputTraceContainer::Reset ()
{
  NS_LOG_FUNCTION (this);

  if (!m_container.empty ())
    {
      m_container.clear ();
    }
  m_currentWorkingDirectory = "";
}

Ptr<SatOutputFileStreamDoubleContainer>
SatFadingOutputTraceContainer::AddNode (key_t key)
{
  NS_LOG_FUNCTION (this);

  std::stringstream filename;

  filename << m_currentWorkingDirectory << "/src/satellite/data/fadingtraces/output/id_" << SatHelper::m_satMacIdMacMapper->GetId (key.first) << "_channelType_" << SatEnums::GetChannelTypeName (key.second);

  std::pair <container_t::iterator, bool> result = m_container.insert (std::make_pair (key, CreateObject<SatOutputFileStreamDoubleContainer> (filename.str ().c_str (), std::ios::out, SatBaseTraceContainer::FADING_TRACE_DEFAULT_NUMBER_OF_COLUMNS)));

  if (result.second == false)
    {
      NS_FATAL_ERROR ("SatFadingOutputTraceContainer::AddNode failed");
    }

  NS_LOG_INFO ("SatFadingOutputTraceContainer::AddNode: Added node with MAC " << key.first << " channel type " << key.second);

  return result.first->second;
}

Ptr<SatOutputFileStreamDoubleContainer>
SatFadingOutputTraceContainer::FindNode (key_t key)
{
  NS_LOG_FUNCTION (this);

  container_t::iterator iter = m_container.find (key);

  if (iter == m_container.end ())
    {
      return AddNode (key);
    }

  return iter->second;
}

void
SatFadingOutputTraceContainer::WriteToFile ()
{
  NS_LOG_FUNCTION (this);

  container_t::iterator iter;

  for (iter = m_container.begin (); iter != m_container.end (); ++iter)
    {
      iter->second->WriteContainerToFile ();
    }
}

void
SatFadingOutputTraceContainer::AddToContainer (key_t key, std::vector<double> newItem)
{
  NS_LOG_FUNCTION (this);

  if (newItem.size () != SatBaseTraceContainer::FADING_TRACE_DEFAULT_NUMBER_OF_COLUMNS)
    {
      NS_FATAL_ERROR ("SatFadingOutputTraceContainer::AddToContainer - Incorrect vector size");
    }

  FindNode (key)->AddToContainer (newItem);
}

} // namespace ns3