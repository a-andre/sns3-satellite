/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Magister Solutions Ltd
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
 * Author: Sami Rantanen <sami.rantanen@magister.fi>
 */

#ifndef SAT_FRAME_ALLOCATOR_H
#define SAT_FRAME_ALLOCATOR_H

#include "ns3/simple-ref-count.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/satellite-frame-conf.h"
#include "satellite-control-message.h"

namespace ns3 {

/**
 * \ingroup satellite
 * \brief helper class for Satellite Superframe Allocator.
 *
 * SatFrameAllocator class is used by SatSuperframeAllocator to maintain information
 * of the pre-allocated symbols per Capacity Category (CC) in a frame.
 * It also knows physical constrains of the frame.
 *
 * SatFrameAllocator is created and used by SatSuperframeAllocator.
 *
 */
class SatFrameAllocator : public SimpleRefCount<SatFrameAllocator>
{
public:
  /**
   * Container to store generated TBTP messages.
   */
  typedef std::vector<Ptr<SatTbtpMessage> > TbtpMsgContainer_t;

  /**
   * Pair used to store UT allocation information.
   *
   * first is vector holding information of the allocated bytes, and
   * second is flag telling if control slot is allocated
   * for the UT/RC.
   */
  typedef std::pair< std::vector<uint32_t>, bool > UtAllocInfoItem_t;

  /**
   * Map container to store UT allocation information.
   *
   * key is UT's address and value is UtAllocInfoItem_t is vector holding information of the allocated bytes
   * for the UT/RC.
   */
  typedef std::map<Address, UtAllocInfoItem_t > UtAllocInfoContainer_t;

  /**
   * Allocation information item for the UT/RC requests [bytes].
   */
  class SatFrameAllocReqItem
  {
  public:
    uint32_t  m_craBytes;
    uint32_t  m_minRbdcBytes;
    uint32_t  m_rbdcBytes;
    uint32_t  m_vbdcBytes;
  };

  /**
   * Container to store SatFrameAllocReqItem items.
   */
  typedef std::vector<SatFrameAllocReqItem>   SatFrameAllocReqItemContainer_t;

  /**
   * SatFrameAllocReq is used to define frame allocation parameters when
   * requesting allocation from SatFrameAllocator (calling method AllocateSymbols).
   */
  class SatFrameAllocReq
  {
  public:
    bool                              m_generateCtrlSlot;
    double                            m_cno;
    Address                           m_address;
    SatFrameAllocReqItemContainer_t   m_reqPerRc;

    /**
     * Construct SatFrameAllocReq
     *
     * \param req Allocation request per RC/CC
     */
    SatFrameAllocReq (SatFrameAllocReqItemContainer_t req) : m_generateCtrlSlot (false), m_cno (NAN), m_reqPerRc (req) { }
  };

  /**
   * Container to store SatFrameAllocReq item pointers.
   */
  typedef std::vector<SatFrameAllocReq *> SatFrameAllocContainer_t;

  /**
   * Enum for CC levels
   */
  typedef enum
    {
      CC_LEVEL_CRA,          //!< CC level CRA
      CC_LEVEL_CRA_MIN_RBDC, //!< CC level CRA + Minimum RBDC
      CC_LEVEL_CRA_RBDC,     //!< CC level CRA + RBDC
      CC_LEVEL_CRA_RBDC_VBDC,//!< CC level CRA + RBDC + VBDC
    } CcLevel_t;

  /**
   * Construct frame info
   *
   * \param frameConf Frame configuration for the frame info
   * \param waveformConf Waveform configuration
   * \param frameId Id of the frame
   * \param m_configType Type of the configuration (0-2 supported)
   */
  SatFrameAllocator (Ptr<SatFrameConf> frameConf, Ptr<SatWaveformConf> waveformConf, uint8_t frameId, SatSuperframeConf::ConfigType_t m_configType);

  /**
   * Reset load counters in frame info.
   */
  void ResetCounters ();

  /**
   * Allocate symbols to all UTs with RCs allocated to the frame.
   * \param targetLoad Target load limits upper bound of the symbols in the frame. Valid values in range 0 and 1.
   * \param fcaEnabled FCA (free capacity allocation) enable status
   */
  void AllocateSymbols (double targetLoad, bool fcaEnabled);

  /**
   * Generate time slots for UT/RCs .
   *
   * \param tbtpContainer TBTP message container to add/fill TBTPs.
   * \param maxFrameSizeInBytes Maximum size for a TBTP message.
   * \param utAllocContainer Reference to UT allocation container to fill in info of the allocation
   * \param rcBasedAllocationEnabled If time slot generated per RC
   * \param waveformTrace Wave form trace callback
   * \param utLoadTrace UT load per the frame trace callback
   */
  void GenerateTimeSlots ( std::vector<Ptr<SatTbtpMessage> >& tbtpContainer, uint32_t maxSizeInBytes, UtAllocInfoContainer_t& utAllocContainer,
                           bool rcBasedAllocationEnabled, TracedCallback<uint32_t> waveformTrace, TracedCallback<uint32_t, long> utLoadTrace);

  /**
   * Get frame load by requested CC
   * \param ccLevel CC of the request
   * \return Load of the requested CC.
   */
  double GetCcLoad (CcLevel_t ccLevel);

  /**
   * Allocate symbols to this frame, if criteria are fulfilled
   *
   * \param ccLevel CC level of the request
   * \param allocReq Requested information
   * \param waveformId Waveform id selected
   * \return true allocation done, otherwise false
   */
  bool Allocate (CcLevel_t ccLevel, SatFrameAllocReq * allocReq, uint32_t waveformId);

  /**
   * Get the best waveform supported by this allocator based on given C/N0.
   *
   *  \param cno C/N0 value used to find the best waveform
   *  \param waveFormId variable to store the best waveform id
   *  \return true if allocator can support given C/N0
   **/
  bool GetBestWaveform (double cno, uint32_t & waveFormId) const;

private:
  /**
   * Allocation information item for requests and allocations [symbols] used
   * internally by SatFrameAllocator.
   */
  class SatFrameAllocInfoItem
  {
  public:
    double  m_craSymbols;
    double  m_minRbdcSymbols;
    double  m_rbdcSymbols;
    double  m_vbdcSymbols;

    /**
     * Construct SatFrameAllocInfoItem.
     */
    SatFrameAllocInfoItem () : m_craSymbols (0.0), m_minRbdcSymbols (0.0), m_rbdcSymbols (0.0), m_vbdcSymbols (0.0) {}

    /**
     * Get symbols allocated/requested by this item.
     *
     * \return Total symbols allocated/requested.
     */
    double GetTotalSymbols () { return (m_craSymbols + m_rbdcSymbols + m_vbdcSymbols); }
  };

  /**
   * Container to store SatFrameAllocInfoItem items.
   */
  typedef std::vector<SatFrameAllocInfoItem>  SatFrameAllocInfoItemContainer_t;

  /**
   * SatFrameAllocInfo is used to hold a frame's allocation info in symbols.
   *
   * It is used for both requested and actual allocations.
   */
  class SatFrameAllocInfo
  {
  public:
    bool    m_ctrlSlotPresent;
    double  m_craSymbols;
    double  m_minRbdcSymbols;
    double  m_rbdcSymbols;
    double  m_vbdcSymbols;

    /**
     * Information for the RCs.
     */
    SatFrameAllocInfoItemContainer_t  m_allocInfoPerRc;

    /**
     * Construct empty SatFrameAllocInfo.
     */
    SatFrameAllocInfo ();

    /**
     * Construct empty SatFrameAllocInfo with given number of RCs.
     */
    SatFrameAllocInfo (uint8_t countOfRcs);

    /**
     * Construct SatFrameAllocInfo from SatFrameAllocReqItem items.
     *
     * \param req Reference to container having SatFrameAllocReqItem items.
     * \param waveForm  Waveform to use in allocation for TRC slots.
     * \param ctrlSlotLength Slot length in symbols for control slots.
     */
    SatFrameAllocInfo (SatFrameAllocReqItemContainer_t &req, Ptr<SatWaveform> trcWaveForm, bool ctrlSlotPresent, double ctrlSlotLength);

    /**
     * Update total count of SatFrameAllocInfo from RCs.
     *
     * \return SatFrameAllocInfoItem holding information of the total request per category.
     */
    SatFrameAllocInfoItem UpdateTotalCounts ();

    /**
     * Get total symbols of the item.
     * \return
     */
    double GetTotalSymbols ();
  };

  /**
   * Allocation information for a UT.
   */
  typedef struct
  {
    double              m_cno;
    SatFrameAllocInfo   m_request;
    SatFrameAllocInfo   m_allocation;
  } UtAllocItem_t;

  /**
   * Pair used as RC allocation item.
   *
   * first = UT address, second = RC index
   */
  typedef std::pair<Address, uint8_t> RcAllocItem_t;

  /**
   * Map container for UT allocation items.
   */
  typedef std::map<Address, UtAllocItem_t> UtAllocContainer_t;

  /**
   * Container for RC allocation items.
   */
  typedef std::list<RcAllocItem_t> RcAllocContainer_t;

  /**
   *  CcReqCompare class for CC type comparisons.
   */
  class CcReqCompare
  {
  public:
    /**
     * Definition for different comparison types.
     */
    typedef enum
    {
      CC_TYPE_MIN_RBDC,//!< CC_TYPE_MIN_RBDC
      CC_TYPE_RBDC,    //!< CC_TYPE_RBDC
      CC_TYPE_VBDC,    //!< CC_TYPE_VBDC
    } CcReqType_t;

    /**
     * Construct CcReqCompare.
     *
     * \param utAllocContainer Reference to UT allocation container.
     * \param ccReqType Type used for comparisons.
     */
    CcReqCompare (const UtAllocContainer_t& utAllocContainer, CcReqCompare::CcReqType_t ccReqType);

    /**
     * Comparison operator to compare two RC allocations.
     *
     * \param rcAlloc1
     * \param rcAlloc2
     * \return false if first RC allocation is smaller than second.
     */
    bool operator() (RcAllocItem_t rcAlloc1, RcAllocItem_t rcAlloc2);

  private:
    /**
     * Reference to UT allocation container
     */
    const UtAllocContainer_t& m_utAllocContainer;

    /**
     * Type used for comparisons.
     */
    CcReqType_t m_ccReqType;
  };

  // total symbols in frame.
  double  m_totalSymbolsInFrame;

  // available (left) symbols in frame.
  double  m_availableSymbolsInFrame;

  // pre-allocated CRA symbols in frame
  double  m_preAllocatedCraSymbols;

  // pre-allocated minimum RBDC symbols in frame
  double  m_preAllocatedMinRdbcSymbols;

  // pre-allocated RBDC symbols in frame
  double  m_preAllocatedRdbcSymbols;

  // pre-allocated minimum VBDC symbols in frame
  double  m_preAllocatedVdbcSymbols;

  // maximum symbols available in frame
  double  m_maxSymbolsPerCarrier;

  // configuration type of the frame
  SatSuperframeConf::ConfigType_t  m_configType;

  // Id of the frame
  uint8_t  m_frameId;

  /**
   * flag indicating if RC based allocation is used. When flag is on then
   * RC is tried to allocate fully. UT total request is used allocation criteria when flag is off.
   */
  bool  m_rcBasedAllocation;

  // Burst lengths in use.
  SatWaveformConf::BurstLengthContainer_t m_burstLenghts;

  // Waveform configuration
  Ptr<SatWaveformConf> m_waveformConf;

  // Frame configuration
  Ptr<SatFrameConf>   m_frameConf;

  // UT allocation container
  UtAllocContainer_t  m_utAllocs;

  // RC allocation container
  RcAllocContainer_t  m_rcAllocs;

  /**
   * Share symbols between all UTs and RCs allocated to the frame.
   *
   * \param fcaEnabled FCA (free capacity allocation) enable status
   */
  void ShareSymbols (bool fcaEnabled);

  /**
   * Get optimal burst length in symbols.
   *
   * \param symbolsToUse Symbols can be used for time slot.
   * \param rcSymbolsLeft Symbols left for RC.
   * \return Optimal burst length for the symbols to allocate.
   */
  uint32_t GetOptimalBurtsLengthInSymbols (int64_t symbolsToUse, int64_t rcSymbolsLeft);

  /**
   * Create time slot according to configuration type.
   *
   * \param carrierId Id of the carrier into create time slot
   * \param utSymbolsToUse Symbols possible to allocated for the UT
   * \param carrierSymbolsToUse Symbols possible to allocate to carrier
   * \param utSymbolsLeft Symbols left for the UT
   * \param rcSymbolsLeft Symbols left for RC
   * \param cno Estimated C/N0 of the UT.
   * \param rcBasedAllocationEnabled If time slot generated per RC
   * \return Create time slot configuration
   */
  Ptr<SatTimeSlotConf> CreateTimeSlot (uint16_t carrierId, int64_t& utSymbolsToUse, int64_t& carrierSymbolsToUse, int64_t& utSymbolsLeft,
                                       int64_t& rcSymbolsLeft, double cno, bool rcBasedAllocationEnabled);

  /**
   * Create control time slot.
   *
   * \param carrierId Id of the carrier into create time slot
   * \param utSymbolsToUse Symbols possible to allocated for the UT
   * \param carrierSymbolsToUse Symbols possible to allocate to carrier
   * \param utSymbolsLeft Symbols left for the UT
   * \param rcSymbolsLeft Symbols left for RC
   * \param rcBasedAllocationEnabled If time slot generated per RC
   * \return Create time slot configuration
   */
  Ptr<SatTimeSlotConf> CreateCtrlTimeSlot (uint16_t carrierId, int64_t& utSymbolsToUse, int64_t& carrierSymbolsToUse, int64_t& utSymbolsLeft,
                                           int64_t& rcSymbolsLeft, bool rcBasedAllocationEnabled);

  /**
   * Update RC/CC requested according to carrier limit
   * \param req Allocation request to update
   */
  void UpdateAllocReq (SatFrameAllocInfo &req);

  /**
   * Accept UT/RC requests of the frame according to given CC level.
   *
   * \param ccLevel CC level for the acceptance
   */
  void AcceptRequests (CcLevel_t ccLevel);

};

} // namespace ns3

#endif /* SAT_FRAME_ALLOCATOR_H */
