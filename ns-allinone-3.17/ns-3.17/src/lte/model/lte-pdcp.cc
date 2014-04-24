/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "ns3/lte-pdcp.h"
#include "ns3/lte-pdcp-header.h"
#include "ns3/lte-pdcp-sap.h"
#include "ns3/lte-pdcp-tag.h"
//yen
#include "ns3/ipv4.h"
#include "ns3/igmp.h"

NS_LOG_COMPONENT_DEFINE ("LtePdcp");

namespace ns3 {


class LtePdcpSpecificLteRlcSapUser : public LteRlcSapUser
{
public:
  LtePdcpSpecificLteRlcSapUser (LtePdcp* pdcp);

  // Interface provided to lower RLC entity (implemented from LteRlcSapUser)
  virtual void ReceivePdcpPdu (Ptr<Packet> p);

private:
  LtePdcpSpecificLteRlcSapUser ();
  LtePdcp* m_pdcp;
};

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser (LtePdcp* pdcp)
  : m_pdcp (pdcp)
{
}

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser ()
{
}

void
LtePdcpSpecificLteRlcSapUser::ReceivePdcpPdu (Ptr<Packet> p)
{
  m_pdcp->DoReceivePdu (p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (LtePdcp);

LtePdcp::LtePdcp ()
  : m_pdcpSapUser (0),
    m_rlcSapProvider (0),
    m_rnti (0),
    m_lcid (0),
    m_multicastgroup (0),//yen
    m_txSequenceNumber (0),
    m_rxSequenceNumber (0)
{
  NS_LOG_FUNCTION (this);
  m_pdcpSapProvider = new LtePdcpSpecificLtePdcpSapProvider<LtePdcp> (this);
  m_rlcSapUser = new LtePdcpSpecificLteRlcSapUser (this);
}

LtePdcp::~LtePdcp ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
LtePdcp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LtePdcp")
    .SetParent<Object> ()
    .AddTraceSource ("TxPDU",
                     "PDU transmission notified to the RLC.",
                     MakeTraceSourceAccessor (&LtePdcp::m_txPdu))
    .AddTraceSource ("RxPDU",
                     "PDU received.",
                     MakeTraceSourceAccessor (&LtePdcp::m_rxPdu))
    ;
  return tid;
}

void
LtePdcp::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete (m_pdcpSapProvider);
  delete (m_rlcSapUser);
}


void
LtePdcp::SetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  m_rnti = rnti;
}
//yen add
void
LtePdcp::SetMulticastGroup (uint16_t multicastgroup)
{
  NS_LOG_FUNCTION (this << (uint16_t) multicastgroup);
  m_multicastgroup = multicastgroup;
}

void
LtePdcp::SetLcId (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << (uint32_t) lcId);
  m_lcid = lcId;
}

void
LtePdcp::SetLtePdcpSapUser (LtePdcpSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_pdcpSapUser = s;
}

LtePdcpSapProvider*
LtePdcp::GetLtePdcpSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_pdcpSapProvider;
}

void
LtePdcp::SetLteRlcSapProvider (LteRlcSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rlcSapProvider = s;
}

LteRlcSapUser*
LtePdcp::GetLteRlcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_rlcSapUser;
}

LtePdcp::Status 
LtePdcp::GetStatus ()
{
  Status s;
  s.txSn = m_txSequenceNumber;
  s.rxSn = m_rxSequenceNumber;
  return s;
}

void 
LtePdcp::SetStatus (Status s)
{
  m_txSequenceNumber = s.txSn;
  m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void
LtePdcp::DoTransmitPdcpSdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION ( this << m_rnti << (uint32_t) m_lcid << p->GetSize () << m_multicastgroup/*yen*/);


  LtePdcpHeader pdcpHeader;
  pdcpHeader.SetSequenceNumber (m_txSequenceNumber);

  m_txSequenceNumber++;
  if (m_txSequenceNumber > m_maxPdcpSn)
    {
      m_txSequenceNumber = 0;
    }
//yen
  /*IgmpHeader igmpheader; 
  uint8_t type = igmpheader.GetType ();
  NS_LOG_FUNCTION ("type " << type );*/
  /*Ipv4Header ipHeader; 
  uint16_t Protocol =  ipHeader.GetProtocol ();
  NS_LOG_FUNCTION ("Protocol: " << Protocol);*/


  pdcpHeader.SetDcBit (LtePdcpHeader::DATA_PDU);

  NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
  p->AddHeader (pdcpHeader);

  // Sender timestamp
  PdcpTag pdcpTag (Simulator::Now ());
  p->AddByteTag (pdcpTag);
  m_txPdu (m_rnti, m_lcid, p->GetSize ());//yen m_multicastgroup

  LteRlcSapProvider::TransmitPdcpPduParameters params;
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  params.pdcpPdu = p;
  params.multicastgroup = m_multicastgroup;//yen

  m_rlcSapProvider->TransmitPdcpPdu (params);
//yen
if (m_multicastgroup == 20)
  {
  NS_LOG_INFO("UE_PDCP "<<"yen_rnti: " << m_rnti << " yen_transmitSequenceNumber: " << m_txSequenceNumber);
  NS_LOG_INFO ("m_multicastgroup" << m_multicastgroup);
  }

else if (m_multicastgroup == 60)
   {
   NS_LOG_INFO("ENB_PDCP "<< "yen_rnti: " << m_rnti << " yen_transmitSequenceNumber: " << m_txSequenceNumber);
   NS_LOG_INFO ("m_multicastgroup" << m_multicastgroup);
   }
}

void
LtePdcp::DoReceivePdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << m_rnti << (uint32_t) m_lcid << p->GetSize ());

  // Receiver timestamp
  PdcpTag pdcpTag;
  Time delay;
  if (p->FindFirstMatchingByteTag (pdcpTag))
    {
      delay = Simulator::Now() - pdcpTag.GetSenderTimestamp ();
    }
  m_rxPdu(m_rnti, m_lcid, p->GetSize (), delay.GetNanoSeconds ());

  LtePdcpHeader pdcpHeader;
  p->RemoveHeader (pdcpHeader);
  NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);

  m_rxSequenceNumber = pdcpHeader.GetSequenceNumber () + 1;
  if (m_rxSequenceNumber > m_maxPdcpSn)
    {
      m_rxSequenceNumber = 0;
    }

  LtePdcpSapUser::ReceivePdcpSduParameters params;
  params.pdcpSdu = p;
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  m_pdcpSapUser->ReceivePdcpSdu (params);

//yen
  /*Ptr<Packet> pCopy = p->Copy ();
  Ipv4Header ipv4Header;
  pCopy->RemoveHeader (ipv4Header);
  uint16_t Identification =  ipv4Header.GetIdentification ();
  uint16_t Protocol =  ipv4Header.GetProtocol ();*/
//yen
if (m_multicastgroup == 20)
  {
  NS_LOG_INFO(" pkt size: " << p->GetSize ()/*<< "Identification: " << Identification << " Protocol: " <<Protocol*/);
  NS_LOG_INFO("UE_PDCP "<<"yen_rnti: " << m_rnti << " yen_receiveSequenceNumber: " << m_txSequenceNumber);
  NS_LOG_INFO ("m_multicastgroup" << m_multicastgroup);
  }

else if (m_multicastgroup == 60)
   {
  NS_LOG_INFO(" pkt size: " << p->GetSize ()/*<< "Identification: " << Identification << " Protocol: " <<Protocol*/);
   NS_LOG_INFO("ENB_PDCP "<< "yen_rnti: " << m_rnti << " yen_receiveSequenceNumber: " << m_txSequenceNumber);
   NS_LOG_INFO ("m_multicastgroup" << m_multicastgroup);
   }


}


} // namespace ns3
