#ifndef IGMP_H
#define IGMP_H

#include "ns3/header.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-header.h"
#include <stdint.h>

namespace ns3 {
class Packet;

class IgmpHeader : public Header
{
public:

  void EnableChecksum (void);
  void SetType (uint8_t type);
  void SetMaxRepTime (uint8_t maxreptime);
  void SetGroupAddress (Ipv4Address groupaddress);

  uint8_t GetType (void) const;
  uint8_t GetMaxRepTime (void) const;
  Ipv4Address GetGroupAddress (void) const;

  static TypeId GetTypeId (void);
  IgmpHeader ();
  virtual ~IgmpHeader ();
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

private:
  uint8_t m_type;
  uint8_t m_maxreptime;
  bool m_calcChecksum;
  Ipv4Address m_groupaddress;
};

}// namespace ns3

#endif /* IGMP_H */
