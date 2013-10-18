/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/cbr-helper.h"

using namespace ns3;

/**
* \ingroup satellite
*
* \brief  Link budget example application for link budget calculation purposes in satellite network.
*         Some parameter can be set as command line arguments.
*         To see help for user arguments:
*         execute command -> ./waf --run "sat-link-budget-example --PrintHelp"
*
*         Application sends one packet from GW connected user to UT connected users and
*         from UT connected user to GW connected user. User defined scenario is created with given beam ID.
*
*         Used parameters and link budget calculation result are printed as long info.
*/

NS_LOG_COMPONENT_DEFINE ("sat-link-budget-example");

// callback called when packet is received by phy RX carrier
static void PacketTraceCb ( std::string context, Ptr<SatSignalParameters> params, Mac48Address ownAdd , Mac48Address destAdd,
                          double ifPower, double cSinr)
{
  // uncomment to set number of decimal places
  //std::cout.setf(std::ios::fixed, std::ios::floatfield);
  //std::cout.precision(20);

  // print only unicast message to prevent printing control messages like TBTP messages
  if ( !destAdd.IsBroadcast() )
    {
      SatChannel::ChannelType_t chType = params->m_channel->GetChannelType ();
      uint32_t beamId = params->m_beamId;
      double centerFreq = params->m_carrierFreq_hz;
      double rxPower_dbW = SatUtils::WToDbW ( params->m_rxPower_W );
      double ifPower_dbW = SatUtils::WToDbW (ifPower);
      double sinr_db = SatUtils::WToDbW ( params->m_sinr );
      double cSinr_db = SatUtils::WToDbW(cSinr);

      NS_LOG_INFO ( Simulator::Now() << " " << chType << " " << ownAdd << " " << destAdd << " " << beamId << " "
                    << centerFreq << " " << ifPower_dbW << " " << rxPower_dbW << " " << sinr_db << " " << cSinr_db );
    }
}

int
main (int argc, char *argv[])
{
  // parameters for link budget calculation
  uint32_t beamId = 1;
  double latitude = 45.00;
  double longitude = 0.00;
  double altitude = 0.00;

  double utOtherSysNoiseDbW = 0.00;
  double utTemperatureDbK = 24.62;

  double gwOtherSysNoiseDbW = 0.00;
  double gwTemperatureDbK = 24.62;

  double geoRtnLinkOtherSysNoiseDbW = 0.00;
  double geoRtnLinkTemperatureDbK = 28.40;

  double geoFwdLinkOtherSysNoiseDbW = 0.00;
  double geoFwdLinkTemperatureDbK = 28.40;

  double utRxMaxAntennaGainDb = 41.70;
  double utTxMaxAntennaGainDb = 45.20;
  double utTxMaxPowerDbW = 4.00;
  double utTxOutputLossDb = 0.50;
  double utTxPointingLossDb = 1.00;
  double utTxOboLossDb= 0.50;
  double utTxAntennaLossDb = 0.00;

  double gwRxMaxAntennaGainDb = 61.50;
  double gwTxMaxAntennaGainDb = 65.20;
  double gwTxMaxPowerDbW = 8.97;
  double gwTxOutputLossDb = 2.00;
  double gwTxPointingLossDb = 1.10;
  double gwTxOboLossDb = 6.00;
  double gwTxAntennaLossDb = 0.00;

  double geoUserRxMaxAntennaGainDb = 54.00;
  double geoUserTxMaxAntennaGainDb = 54.00;
  double geoUserTxMaxPowerDbW = 15.00;
  double geoUserTxOutputLossDb = 2.85;
  double geoUserTxPointingLossDb = 0.00;
  double geoUserTxOboLossDb = 0.00;
  double geoUserTxAntennaLossDb = 1.00;

  double geoFeederRxMaxAntennaGainDb = 54.00;
  double geoFeederTxMaxAntennaGainDb = 54.00;
  double geoFeederTxMaxPowerDbW = -10.40;
  double geoFeederTxOutputLossDb = 1.75;
  double geoFeederTxPointingLossDb = 0.00;
  double geoFeederTxOboLossDb = 4.00;
  double geoFeederTxAntennaLossDb = 1.00;

  // read command line parameters can be given by user
  CommandLine cmd;
  cmd.AddValue("beam", "Beam to use for testing. (1 - 72)", beamId);
  cmd.AddValue("latitude", "Latitude of UT position (-90 ... 90.0)", latitude);
  cmd.AddValue("longitude", "Longitude of UT position (-180 ... 180)", longitude);
  cmd.AddValue("altitude", "Altitude of UT position (meters)", altitude);
  cmd.Parse (argc, argv);

  // enable info logs
  LogComponentEnable ("sat-link-budget-example", LOG_LEVEL_INFO);

  // Create reference system "Scenario72"
  std::string scenarioName = "Scenario72";

  // create helpers
  Ptr<SatHelper> helper = CreateObject<SatHelper> (scenarioName);

  // configure helpers
  Ptr<SatBeamHelper> beamHelper = helper->GetBeamHelper();
  Ptr<SatUtHelper> utHelper = beamHelper->GetUtHelper();
  Ptr<SatGwHelper> gwHelper = beamHelper->GetGwHelper();
  Ptr<SatGeoHelper> geoHelper = beamHelper->GetGeoHelper();

  utHelper->SetAttribute("RxOtherSysNoiseDbW", DoubleValue (utOtherSysNoiseDbW));
  utHelper->SetAttribute("RxTemperatureDbK", DoubleValue (utTemperatureDbK));

  gwHelper->SetAttribute("RxOtherSysNoiseDbW", DoubleValue (gwOtherSysNoiseDbW));
  gwHelper->SetAttribute("RxTemperatureDbK", DoubleValue (gwTemperatureDbK));

  geoHelper->SetAttribute("RtnLinkOtherSysNoiseDbW", DoubleValue (geoRtnLinkOtherSysNoiseDbW));
  geoHelper->SetAttribute("RtnLinkRxTemperatureDbK", DoubleValue (geoRtnLinkTemperatureDbK));

  geoHelper->SetAttribute("FwdLinkOtherSysNoiseDbW", DoubleValue (geoFwdLinkOtherSysNoiseDbW));
  geoHelper->SetAttribute("FwdLinkRxTemperatureDbK", DoubleValue (geoFwdLinkTemperatureDbK));

  utHelper->SetPhyAttribute ("RxMaxAntennaGainDb", DoubleValue (utRxMaxAntennaGainDb));
  utHelper->SetPhyAttribute ("TxMaxAntennaGainDb", DoubleValue (utTxMaxAntennaGainDb));
  utHelper->SetPhyAttribute ("TxMaxPowerDbW", DoubleValue (utTxMaxPowerDbW));
  utHelper->SetPhyAttribute ("TxOutputLossDb", DoubleValue (utTxOutputLossDb));
  utHelper->SetPhyAttribute ("TxPointingLossDb", DoubleValue (utTxPointingLossDb));
  utHelper->SetPhyAttribute ("TxOboLossDb", DoubleValue (utTxOboLossDb));
  utHelper->SetPhyAttribute ("TxAntennaLossDb", DoubleValue (utTxAntennaLossDb));

  gwHelper->SetPhyAttribute ("RxMaxAntennaGainDb", DoubleValue (gwRxMaxAntennaGainDb));
  gwHelper->SetPhyAttribute ("TxMaxAntennaGainDb", DoubleValue (gwTxMaxAntennaGainDb));
  gwHelper->SetPhyAttribute ("TxMaxPowerDbW", DoubleValue (gwTxMaxPowerDbW));
  gwHelper->SetPhyAttribute ("TxOutputLossDb", DoubleValue (gwTxOutputLossDb));
  gwHelper->SetPhyAttribute ("TxPointingLossDb", DoubleValue (gwTxPointingLossDb));
  gwHelper->SetPhyAttribute ("TxOboLossDb", DoubleValue (gwTxOboLossDb));
  gwHelper->SetPhyAttribute ("TxAntennaLossDb", DoubleValue (gwTxAntennaLossDb));

  geoHelper->SetUserPhyAttribute ("RxMaxAntennaGainDb", DoubleValue (geoUserRxMaxAntennaGainDb));
  geoHelper->SetUserPhyAttribute ("TxMaxAntennaGainDb", DoubleValue (geoUserTxMaxAntennaGainDb));
  geoHelper->SetUserPhyAttribute ("TxMaxPowerDbW", DoubleValue (geoUserTxMaxPowerDbW));
  geoHelper->SetUserPhyAttribute ("TxOutputLossDb", DoubleValue (geoUserTxOutputLossDb));
  geoHelper->SetUserPhyAttribute ("TxPointingLossDb", DoubleValue (geoUserTxPointingLossDb));
  geoHelper->SetUserPhyAttribute ("TxOboLossDb", DoubleValue (geoUserTxOboLossDb));
  geoHelper->SetUserPhyAttribute ("TxAntennaLossDb", DoubleValue (geoUserTxAntennaLossDb));

  geoHelper->SetFeederPhyAttribute ("RxMaxAntennaGainDb", DoubleValue (geoFeederRxMaxAntennaGainDb));
  geoHelper->SetFeederPhyAttribute ("TxMaxAntennaGainDb", DoubleValue (geoFeederTxMaxAntennaGainDb));
  geoHelper->SetFeederPhyAttribute ("TxMaxPowerDbW", DoubleValue (geoFeederTxMaxPowerDbW));
  geoHelper->SetFeederPhyAttribute ("TxOutputLossDb", DoubleValue (geoFeederTxOutputLossDb));
  geoHelper->SetFeederPhyAttribute ("TxPointingLossDb", DoubleValue (geoFeederTxPointingLossDb));
  geoHelper->SetFeederPhyAttribute ("TxOboLossDb", DoubleValue (geoFeederTxOboLossDb));
  geoHelper->SetFeederPhyAttribute ("TxAntennaLossDb", DoubleValue (geoFeederTxAntennaLossDb));

  // create user defined scenario
  SatBeamUserInfo beamInfo = SatBeamUserInfo(1,1);
  std::map<uint32_t, SatBeamUserInfo > beamMap;
  beamMap[beamId] = beamInfo;
  helper->SetBeamUserInfo(beamMap);

  helper->CreateScenario (SatHelper::USER_DEFINED);

  // set callback traces where we want results out

  Config::Connect ("/NodeList/*/DeviceList/*/SatPhy/PhyRx/RxCarrierList/*/PacketTrace",
                               MakeCallback (&PacketTraceCb));

  Config::Connect ("/NodeList/*/DeviceList/*/UserPhy/*/PhyRx/RxCarrierList/*/PacketTrace",
                               MakeCallback (&PacketTraceCb));

  Config::Connect ("/NodeList/*/DeviceList/*/FeederPhy/*/PhyRx/RxCarrierList/*/PacketTrace",
                                 MakeCallback (&PacketTraceCb));
  // Set UT position
  NodeContainer ut = helper->UtNodes();
  Ptr<SatMobilityModel> utMob = ut.Get(0)->GetObject<SatMobilityModel> ();
  utMob->SetGeoPosition(GeoCoordinate(latitude, longitude, altitude));

  // get users
  NodeContainer utUsers = helper->GetUtUsers();
  NodeContainer gwUsers = helper->GetGwUsers();

  uint16_t port = 9;

  // create application on GW user
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress(helper->GetUserAddress(gwUsers.Get(0)), port));
  CbrHelper cbrHelper ("ns3::UdpSocketFactory", InetSocketAddress(helper->GetUserAddress(utUsers.Get(0)), port));
  cbrHelper.SetAttribute("Interval", StringValue ("0.1s"));
  cbrHelper.SetAttribute("PacketSize", UintegerValue (512) );

  ApplicationContainer gwSink = sinkHelper.Install (gwUsers.Get (0));
  gwSink.Start (Seconds (0.1));
  gwSink.Stop (Seconds (1.0));

  ApplicationContainer gwCbr = cbrHelper.Install (gwUsers.Get (0));
  gwCbr.Start (Seconds (0.1));
  gwCbr.Stop (Seconds (0.25));

  // create application on UT user
  sinkHelper.SetAttribute("Local", AddressValue(Address (InetSocketAddress (helper->GetUserAddress (utUsers.Get(0)), port))));
  cbrHelper.SetAttribute("Remote", AddressValue(Address (InetSocketAddress (helper->GetUserAddress (gwUsers.Get(0)), port))));

  ApplicationContainer utSink = sinkHelper.Install (utUsers.Get (0));
  utSink.Start (Seconds (0.1));
  utSink.Stop (Seconds (1.0));

  ApplicationContainer utCbr = cbrHelper.Install (utUsers.Get (0));
  utCbr.Start (Seconds (0.1));
  utCbr.Stop (Seconds (0.25));

  NodeContainer gw = helper->GwNodes();
  Ptr<SatMobilityModel> gwMob = gw.Get(0)->GetObject<SatMobilityModel> ();

  Ptr<Node> geo = helper->GeoSatNode();
  Ptr<SatMobilityModel> geoMob = geo->GetObject<SatMobilityModel> ();

  // print used parameters usign log info
  NS_LOG_INFO ("--- Satellite link budget ---");
  NS_LOG_INFO (" Beam ID: " << beamId);
  NS_LOG_INFO (" Geo position: " << geoMob->GetGeoPosition());
  NS_LOG_INFO (" GW position: " << gwMob->GetGeoPosition());
  NS_LOG_INFO (" UT position: " << utMob->GetGeoPosition());
  NS_LOG_INFO ("  ");
  NS_LOG_INFO ( "Link params (Rx Antenna gain, Tx Antenna gain, TxPower, TxOutputLoss, TxPointingLoss, TxOboLoss, TxAntennaLoss, OtherSyssLoss, RxTemp) :");
  NS_LOG_INFO (" UT: " << utRxMaxAntennaGainDb << " " << utTxMaxAntennaGainDb << " "
                       << utTxMaxPowerDbW << " " << utTxOutputLossDb << " " << utTxPointingLossDb << " "
                       << utTxOboLossDb << " " << utTxAntennaLossDb << " "
                       << utOtherSysNoiseDbW << " " << utTemperatureDbK );

  NS_LOG_INFO (" GW: " << gwRxMaxAntennaGainDb << " " << gwTxMaxAntennaGainDb << " "
                       << gwTxMaxPowerDbW << " " <<  gwTxOutputLossDb << " " << gwTxPointingLossDb << " "
                       << gwTxOboLossDb << " " << gwTxAntennaLossDb << " "
                       << gwOtherSysNoiseDbW << " " << gwTemperatureDbK );

  NS_LOG_INFO (" GEO user: " << geoUserRxMaxAntennaGainDb << " " << geoUserTxMaxAntennaGainDb << " "
                             << geoUserTxMaxPowerDbW << " " << geoUserTxOutputLossDb << " " << geoUserTxPointingLossDb << " "
                             << geoUserTxOboLossDb << " " << geoUserTxAntennaLossDb << " "
                             << geoRtnLinkOtherSysNoiseDbW << " " << geoRtnLinkTemperatureDbK );

  NS_LOG_INFO (" GEO feeder: " << geoFeederRxMaxAntennaGainDb << " " << geoFeederTxMaxAntennaGainDb << " "
                               << geoFeederTxMaxPowerDbW << " " << geoFeederTxOutputLossDb << " " << geoFeederTxPointingLossDb << " "
                               << geoFeederTxOboLossDb << " " << geoFeederTxAntennaLossDb << " "
                               << geoFwdLinkOtherSysNoiseDbW << " " << geoFwdLinkTemperatureDbK);

  NS_LOG_INFO ("  ");
  NS_LOG_INFO ("Link results (Time, Channel type, Own address, Dest. address, Beam ID, Carrier Center freq, IF Power, RX Power, SINR, Composite SINR) :");
  // results are printed out in callback (PacketTraceCb)

  Simulator::Stop (Seconds(1.1));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}