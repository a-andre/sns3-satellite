/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-module.h"
#include "ns3/applications-module.h"
#include "ns3/cbr-helper.h"


using namespace ns3;

/**
* \ingroup satellite
*
* \brief  Cbr example application to use satellite network.
*         Interval, packet size and test scenario can be given
*         in command line as user argument.
*         To see help for user arguments:
*         execute command -> ./waf --run "sat-cbr-example --PrintHelp"
*
*         Cbr example application sends first packets from GW connected user
*         to UT connected users and after that from UT connected user to GW connected
*         user.
*
*/

NS_LOG_COMPONENT_DEFINE ("sat-cbr-example");

int
main (int argc, char *argv[])
{
  uint32_t packetSize = 512;
  std::string interval = "1s";
  std::string scenario = "simple";
  std::string scenarioLogFile = "";
  SatHelper::PreDefinedScenario_t satScenario = SatHelper::SIMPLE;

  // read command line parameters given by user
  CommandLine cmd;
  cmd.AddValue("packetSize", "Size of constant packet (bytes)", packetSize);
  cmd.AddValue("interval", "Interval to sent packets in seconds, (e.g. (1s)", interval);
  cmd.AddValue("scenario", "Test scenario to use. (simple, larger or full", scenario);
  cmd.AddValue("logFile", "File name for scenario creation log", scenarioLogFile);
  cmd.Parse (argc, argv);

  if ( scenario == "larger")
    {
      satScenario = SatHelper::LARGER;
    }
  else if ( scenario == "full")
    {
      satScenario = SatHelper::FULL;
    }

  // enable info logs
  //LogComponentEnable ("CbrApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  //LogComponentEnable ("sat-cbr-example", LOG_LEVEL_INFO);

  // remove next line from comments to run real time simulation
  //GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));

  // create satellite helper with given scenario default=simple

  // Create reference system, two options:
  // - "Scenario72"
  // - "Scenario98"
  std::string scenarioName = "Scenario72";
  //std::string scenarioName = "Scenario98";

  Ptr<SatHelper> helper = CreateObject<SatHelper> (scenarioName);

  if ( scenarioLogFile != "" )
    {
      helper->EnableCreationTraces(scenarioLogFile, false);
    }

  helper->CreateScenario(satScenario);

  helper->EnablePacketTrace ();

  // get users
  NodeContainer utUsers = helper->GetUtUsers();
  NodeContainer gwUsers = helper->GetGwUsers();

  uint16_t port = 9;

  // create application on GW user
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress(helper->GetUserAddress(gwUsers.Get(0)), port));
  CbrHelper cbrHelper ("ns3::UdpSocketFactory", InetSocketAddress(helper->GetUserAddress(utUsers.Get(0)), port));
  cbrHelper.SetAttribute("Interval", StringValue (interval));
  cbrHelper.SetAttribute("PacketSize", UintegerValue (packetSize) );

  ApplicationContainer gwSink = sinkHelper.Install (gwUsers.Get (0));
  gwSink.Start (Seconds (1.0));
  gwSink.Stop (Seconds (10.0));

  ApplicationContainer gwCbr = cbrHelper.Install (gwUsers.Get (0));
  gwCbr.Start (Seconds (1.0));
  gwCbr.Stop (Seconds (2.1));

  // create application on UT user
  sinkHelper.SetAttribute("Local", AddressValue(Address (InetSocketAddress (helper->GetUserAddress (utUsers.Get(0)), port))));
  cbrHelper.SetAttribute("Remote", AddressValue(Address (InetSocketAddress (helper->GetUserAddress (gwUsers.Get(0)), port))));

  ApplicationContainer utSink = sinkHelper.Install (utUsers.Get (0));
  utSink.Start (Seconds (1.0));
  utSink.Stop (Seconds (10.0));

  ApplicationContainer utCbr = cbrHelper.Install (utUsers.Get (0));
  utCbr.Start (Seconds (7.0));
  utCbr.Stop (Seconds (9.1));

  //LogComponentEnable ("SatStatsHelper", LOG_PREFIX_ALL);
  //LogComponentEnable ("SatStatsHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("SatStatsFwdThroughputHelper", LOG_PREFIX_ALL);
  //LogComponentEnable ("SatStatsFwdThroughputHelper", LOG_LEVEL_ALL);
  Ptr<SatStatsHelperContainer> stats = CreateObject<SatStatsHelperContainer> (helper);
  stats->AddGlobalFwdThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  stats->AddGlobalFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  stats->AddGlobalFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);
  stats->AddPerGwFwdThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  stats->AddPerGwFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  stats->AddPerGwFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);
  stats->AddPerBeamFwdThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  stats->AddPerBeamFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  stats->AddPerBeamFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);
  stats->AddPerUtFwdThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  stats->AddPerUtFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  stats->AddPerUtFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);
  stats->AddPerUtUserFwdThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  stats->AddPerUtUserFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  stats->AddPerUtUserFwdThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);

  NS_LOG_INFO("--- Cbr-example ---");
  NS_LOG_INFO("  Scenario used: " << scenario);
  NS_LOG_INFO("  PacketSize: " << packetSize);
  NS_LOG_INFO("  Interval: " << interval);
  NS_LOG_INFO("  Creation logFile: " << scenarioLogFile);
  NS_LOG_INFO("  ");

  Simulator::Stop (Seconds(11));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
