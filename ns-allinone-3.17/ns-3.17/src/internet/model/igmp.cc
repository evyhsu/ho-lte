/*this file is written by yenchu hsu
2014 spring
yenchsu@gmail.com
*/



#include "igmp.h"
#include "ns3/packet.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("IgmpHeader");

namespace ns3 {

/********************************************************
 *        Igmpv4Header
 ********************************************************/
NS_OBJECT_ENSURE_REGISTERED (IgmpHeader);

TypeId 
IgmpHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::IgmpHeader")
    .SetParent<Header> ()
    .AddConstructor<IgmpHeader> ()
  ;
  return tid;
}

IgmpHeader::IgmpHeader ()
  : m_type (0),
    m_maxreptime (0),
    m_calcChecksum (false)

{
}

IgmpHeader::~IgmpHeader ()
{
}

void
IgmpHeader::EnableChecksum (void)
{
  NS_LOG_FUNCTION (this);
  m_calcChecksum = true;
}
TypeId
IgmpHeader::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}
uint32_t
IgmpHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 8;
}
void
IgmpHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_type);
  i.WriteU8 (m_maxreptime);
  i.WriteHtonU16 (0);
  if (m_calcChecksum)
    {
      i = start;
      uint16_t checksum = i.CalculateIpChecksum (i.GetSize ());
      i = start;
      i.Next (2);
      i.WriteU16 (checksum);
    }
  i.WriteHtonU32 (m_groupaddress.Get ());


}
uint32_t 
IgmpHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  m_type = start.ReadU8 ();
  m_maxreptime = start.ReadU8 ();
  start.Next (2); // uint16_t checksum = start.ReadNtohU16 ();
  m_groupaddress.Set (start.ReadNtohU32 ());

  return 8;
}
void 
IgmpHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "type=" << (uint32_t)m_type << ", maxreptime=" << (uint32_t)m_maxreptime << ", groupaddress=" << (Ipv4Address) m_groupaddress;
}

void 
IgmpHeader::SetType (uint8_t type)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (type));
  m_type = type;
}
void 
IgmpHeader::SetMaxRepTime (uint8_t maxreptime)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (maxreptime));
  m_maxreptime = maxreptime;
}
void
IgmpHeader::SetGroupAddress (Ipv4Address groupaddress)
{
  m_groupaddress = groupaddress;
}

uint8_t 
IgmpHeader::GetType (void) const
{
  NS_LOG_FUNCTION (this);
  return m_type;
}
uint8_t 
IgmpHeader::GetMaxRepTime (void) const
{
  NS_LOG_FUNCTION (this); 
  return m_maxreptime;
}
Ipv4Address
IgmpHeader::GetGroupAddress (void) const
{
  return m_groupaddress;
}

} // namespace ns3

