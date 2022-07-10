/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SixthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                8 Mbps, 3 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//

int dropped_pckts = 0,dropped_pckts1 = 0,dropped_pckts2 = 0,dropped_pckts3 = 0;
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  dropped_pckts++;
  if(Simulator::Now ().GetSeconds ()>=1. and Simulator::Now ().GetSeconds ()<=20.){
	dropped_pckts1++;
  }else if(Simulator::Now ().GetSeconds ()>=5. and Simulator::Now ().GetSeconds ()<=25.){
	dropped_pckts2++;
  }else if(Simulator::Now ().GetSeconds ()>=15. and Simulator::Now ().GetSeconds ()<=30.){
	dropped_pckts3++;
  }	
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
}

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  std::cout<<"Enter the configuration Number : ";
  int a; std::cin>>a;
  if(a==1){
	  // TCP NEW RENO
  }else if(a==2){
	  //TCP NEW RENO AT NODE 0 AND TCP NEW RENO CSE AT NODE 1
  }else if(a==3){
	  //TCP NEW RENO CSE 
  }else{
	  NS_LOG_DEBUG ("Configuration Not Found");
	  exit (1);
  }
  NodeContainer nodes;
  nodes.Create (3);

  PointToPointHelper pointToPointforN1;
  pointToPointforN1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPointforN1.SetChannelAttribute ("Delay", StringValue ("3ms"));

  PointToPointHelper pointToPointforN2;
  pointToPointforN2.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
  pointToPointforN2.SetChannelAttribute ("Delay", StringValue ("3ms"));
  
  NetDeviceContainer devices1;
  devices1 = pointToPointforN1.Install (nodes.Get(0),nodes.Get(1));
  
  Ptr<RateErrorModel> em1 = CreateObject<RateErrorModel> ();
  em1->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em1));
  
  NetDeviceContainer devices2;
  devices2 = pointToPointforN1.Install (nodes.Get(0),nodes.Get(1));
    
  Ptr<RateErrorModel> em2 = CreateObject<RateErrorModel> ();
  em2->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em2));
  
  NetDeviceContainer devices3;
  devices3 = pointToPointforN2.Install (nodes.Get(2),nodes.Get(1));

  Ptr<RateErrorModel> em3 = CreateObject<RateErrorModel> ();
  em3->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em3));
  
  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  
  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.2.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
  
  Ipv4AddressHelper address3;
  address3.SetBase ("10.1.3.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces3 = address3.Assign (devices3);

  uint16_t sinkPort1 = 8080,sinkPort2 = 8081,sinkPort3 = 8082;
  Address sinkAddress1 (InetSocketAddress (interfaces1.GetAddress (1), sinkPort1));
  PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
  ApplicationContainer sinkApps1 = packetSinkHelper1.Install (nodes.Get (1));
  sinkApps1.Start (Seconds (1.));
  sinkApps1.Stop (Seconds (20.));
  
  Address sinkAddress2 (InetSocketAddress (interfaces2.GetAddress (1), sinkPort2));
  PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort2));
  ApplicationContainer sinkApps2 = packetSinkHelper2.Install (nodes.Get (1));
  sinkApps2.Start (Seconds (5.));
  sinkApps2.Stop (Seconds (25.));
  
  Address sinkAddress3 (InetSocketAddress (interfaces3.GetAddress (1), sinkPort3));
  PacketSinkHelper packetSinkHelper3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort3));
  ApplicationContainer sinkApps3 = packetSinkHelper3.Install (nodes.Get (1));
  sinkApps3.Start (Seconds (15.));
  sinkApps3.Stop (Seconds (30.));
  
  if(a==1){
	TypeId tid1 = TypeId::LookupByName ("ns3::TcpNewReno");
	std::stringstream nodeId1;
	nodeId1 << nodes.Get (0)->GetId ();
	std::string specificNode1 = "/NodeList/" + nodeId1.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode1, TypeIdValue (tid1));
	Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app1 = CreateObject<MyApp> ();
	app1->Setup (ns3TcpSocket1, sinkAddress1, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app1);
	app1->SetStartTime (Seconds (1.));
	app1->SetStopTime (Seconds (20.));

	TypeId tid2 = TypeId::LookupByName ("ns3::TcpNewReno");
	std::stringstream nodeId2;
	nodeId2 << nodes.Get (0)->GetId ();
	std::string specificNode2 = "/NodeList/" + nodeId2.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode2, TypeIdValue (tid2));
	Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app2 = CreateObject<MyApp> ();
	app2->Setup (ns3TcpSocket2, sinkAddress2, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app2);
	app2->SetStartTime (Seconds (5.));
	app2->SetStopTime (Seconds (25.));

	TypeId tid3 = TypeId::LookupByName ("ns3::TcpNewReno");
	std::stringstream nodeId3;
	nodeId3 << nodes.Get (0)->GetId ();
	std::string specificNode3 = "/NodeList/" + nodeId3.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode3, TypeIdValue (tid3));
	Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (2), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app3 = CreateObject<MyApp> ();
	app3->Setup (ns3TcpSocket3, sinkAddress3, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (2)->AddApplication (app3);
	app3->SetStartTime (Seconds (15.));
	app3->SetStopTime (Seconds (30.));

	AsciiTraceHelper asciiTraceHelper;
	Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("Third1.cwnd");
	Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("Third2.cwnd");
	Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream ("Third3.cwnd");
	ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream1));
	ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream2));
	ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream3));

	PcapHelper pcapHelper;
	Ptr<PcapFileWrapper> file1 = pcapHelper.CreateFile ("Third1.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file2 = pcapHelper.CreateFile ("Third2.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file3 = pcapHelper.CreateFile ("Third3.pcap", std::ios::out, PcapHelper::DLT_PPP);
	devices1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file1));
	devices2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file2));
	devices3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file3));

	Simulator::Stop (Seconds (30));
	Simulator::Run ();
	std::cout<<"Total Number of Dropped Packets from 1 sec to 30 sec is : "<<dropped_pckts<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection1 : "<<dropped_pckts1<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection2 : "<<dropped_pckts2<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection3 : "<<dropped_pckts3<<"\n";
	Simulator::Destroy ();

	return 0;
  }else if(a==2){
	TypeId tid1 = TypeId::LookupByName ("ns3::TcpNewReno");
	std::stringstream nodeId1;
	nodeId1 << nodes.Get (0)->GetId ();
	std::string specificNode1 = "/NodeList/" + nodeId1.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode1, TypeIdValue (tid1));
	Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app1 = CreateObject<MyApp> ();
	app1->Setup (ns3TcpSocket1, sinkAddress1, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app1);
	app1->SetStartTime (Seconds (1.));
	app1->SetStopTime (Seconds (20.));

	TypeId tid2 = TypeId::LookupByName ("ns3::TcpNewReno");
	std::stringstream nodeId2;
	nodeId2 << nodes.Get (0)->GetId ();
	std::string specificNode2 = "/NodeList/" + nodeId2.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode2, TypeIdValue (tid2));
	Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app2 = CreateObject<MyApp> ();
	app2->Setup (ns3TcpSocket2, sinkAddress2, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app2);
	app2->SetStartTime (Seconds (5.));
	app2->SetStopTime (Seconds (25.));

	TypeId tid3 = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
	std::stringstream nodeId3;
	nodeId3 << nodes.Get (0)->GetId ();
	std::string specificNode3 = "/NodeList/" + nodeId3.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode3, TypeIdValue (tid3));
	Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (2), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app3 = CreateObject<MyApp> ();
	app3->Setup (ns3TcpSocket3, sinkAddress3, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (2)->AddApplication (app3);
	app3->SetStartTime (Seconds (15.));
	app3->SetStopTime (Seconds (30.));

	AsciiTraceHelper asciiTraceHelper;
	Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("Third1.cwnd");
	Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("Third2.cwnd");
	Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream ("Third3.cwnd");
	ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream1));
	ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream2));
	ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream3));

	PcapHelper pcapHelper;
	Ptr<PcapFileWrapper> file1 = pcapHelper.CreateFile ("Third1.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file2 = pcapHelper.CreateFile ("Third2.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file3 = pcapHelper.CreateFile ("Third3.pcap", std::ios::out, PcapHelper::DLT_PPP);
	devices1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file1));
	devices2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file2));
	devices3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file3));

	Simulator::Stop (Seconds (30));
	Simulator::Run ();
	std::cout<<"Total Number of Dropped Packets from 1 sec to 30 sec is : "<<dropped_pckts<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection1 : "<<dropped_pckts1<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection2 : "<<dropped_pckts2<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection3 : "<<dropped_pckts3<<"\n";
	Simulator::Destroy ();

	return 0;
  }else{
	TypeId tid1 = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
	std::stringstream nodeId1;
	nodeId1 << nodes.Get (0)->GetId ();
	std::string specificNode1 = "/NodeList/" + nodeId1.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode1, TypeIdValue (tid1));
	Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app1 = CreateObject<MyApp> ();
	app1->Setup (ns3TcpSocket1, sinkAddress1, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app1);
	app1->SetStartTime (Seconds (1.));
	app1->SetStopTime (Seconds (20.));

	TypeId tid2 = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
	std::stringstream nodeId2;
	nodeId2 << nodes.Get (0)->GetId ();
	std::string specificNode2 = "/NodeList/" + nodeId2.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode2, TypeIdValue (tid2));
	Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app2 = CreateObject<MyApp> ();
	app2->Setup (ns3TcpSocket2, sinkAddress2, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (app2);
	app2->SetStartTime (Seconds (5.));
	app2->SetStopTime (Seconds (25.));

	TypeId tid3 = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
	std::stringstream nodeId3;
	nodeId3 << nodes.Get (0)->GetId ();
	std::string specificNode3 = "/NodeList/" + nodeId3.str () + "/$ns3::TcpL4Protocol/SocketType";
	Config::Set (specificNode3, TypeIdValue (tid3));
	Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (2), TcpSocketFactory::GetTypeId ());
	Ptr<MyApp> app3 = CreateObject<MyApp> ();
	app3->Setup (ns3TcpSocket3, sinkAddress3, 3000, 11000, DataRate ("1.5Mbps"));
	nodes.Get (2)->AddApplication (app3);
	app3->SetStartTime (Seconds (15.));
	app3->SetStopTime (Seconds (30.));  

	AsciiTraceHelper asciiTraceHelper;
	Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("Third1.cwnd");
	Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("Third2.cwnd");
	Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream ("Third3.cwnd");
	ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream1));
	ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream2));
	ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream3));

	PcapHelper pcapHelper;
	Ptr<PcapFileWrapper> file1 = pcapHelper.CreateFile ("Third1.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file2 = pcapHelper.CreateFile ("Third2.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> file3 = pcapHelper.CreateFile ("Third3.pcap", std::ios::out, PcapHelper::DLT_PPP);
	devices1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file1));
	devices2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file2));
	devices3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,file3));

	Simulator::Stop (Seconds (30));
	Simulator::Run ();
	std::cout<<"Total Number of Dropped Packets from 1 sec to 30 sec is : "<<dropped_pckts<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection1 : "<<dropped_pckts1<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection2 : "<<dropped_pckts2<<"\n";
	std::cout<<"Total Number of Dropped Packets from Connection3 : "<<dropped_pckts3<<"\n";
	Simulator::Destroy ();

	return 0;
  }	  
}

