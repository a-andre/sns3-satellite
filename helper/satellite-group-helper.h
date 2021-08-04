/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions Ltd
 * Copyright (c) 2018 CNES
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
 * Author: Bastien Tauran <bastien.tauran@viveris.fr>
 */

#ifndef SATELLITE_GROUP_HELPER_H
#define SATELLITE_GROUP_HELPER_H

#include <set>
#include <list>
#include <map>

#include <ns3/node-container.h>

namespace ns3 {

/**
 * \brief This helper is used to create groups of UTs.
 */
class SatGroupHelper : public Object
{
public:

  /**
   * \brief Get the type ID
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Get the type ID of instance
   * \return the object TypeId
   */
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Default constructor for SatGroupHelper
   */
  SatGroupHelper ();

  /**
   * Destructor for SatGroupHelper.
   */
  virtual ~SatGroupHelper ()
  {
  }

  /**
   * \brief Add the list of UTs in the database
   * \param uts The list of all UTs in the simulation
   */
  void SetUtNodes (NodeContainer uts);

  /**
   * \brief Add a node to a group
   * \param groupId The group ID where the node is added
   * \param node The node to add
   */
  void AddUtNodeToGroup (uint32_t groupId, Ptr<Node> node);

  /**
   * \brief Add several nodes to a group
   * \param groupId The group ID where the nodes are added
   * \param nodes The nodes to add
   */
  void AddUtNodesToGroup (uint32_t groupId, NodeContainer nodes);

  /**
   * \param groupId The group ID
   * \return container having all UT nodes associated to a group
   */
  NodeContainer GetUtNodes (uint32_t groupId) const;

  /**
   * \brief Count the number of groups created
   * \return The number of groups
   */
  uint32_t GetN ();

  /**
   * \brief Get the list of groups created
   * \return The list of groups created
   */
  std::list<uint32_t> GetGroups ();

  /**
   * Dispose of this class instance
   */
  virtual void DoDispose ();

private:
  /**
   * Tells if the groupId is already existing in the database
   */
  bool IsGroupExisting (uint32_t groupId) const;

  /**
   * The list of all the UTs in the simulation
   */
  NodeContainer                                     m_uts;

  /**
   * Container to associate nodes to the groups
   */
  std::map<uint32_t, std::set<Ptr<Node> > >         m_groupsMap;

  /**
   * List of group ID created
   */
  std::list<uint32_t>                               m_groupsList;
};

} // namespace ns3

#endif /* SATELLITE_GROUP_HELPER_H */
