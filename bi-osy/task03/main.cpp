#ifndef __PROGTEST__
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
using namespace std;
//======================================================================================================================
const int SECTOR_SIZE = 512;
const int MAX_RAID_DEVICES = 16;
const int MAX_DEVICE_SECTORS = 1024 * 1024 * 2;
const int MIN_DEVICE_SECTORS = 1 * 1024 * 2;

const int RAID_STOPPED = 0;
const int RAID_OK = 1;
const int RAID_DEGRADED = 2;
const int RAID_FAILED = 3;
//======================================================================================================================
struct TBlkDev
{
  int m_Devices; /**< Number of devices used in RAID. */
  int m_Sectors; /**< Number of sectors at each disk */

  int (* m_Read ) ( int, int, void *, int );

  int (* m_Write ) ( int, int, const void *, int );
};
#endif /* __PROGTEST__ */
//======================================================================================================================
struct TRaidServiceData
{
  TRaidServiceData ( void );

  int     m_Devices;
  int     m_Sectors;
  int     (* m_Read )  ( int, int, void *, int );
  int     (* m_Write ) ( int, int, const void *, int );
  int32_t m_RaidStatus;
  int32_t m_Timestamp;
  int32_t m_BadDisk;
};
//----------------------------------------------------------------------------------------------------------------------
TRaidServiceData::TRaidServiceData ( void ) :
    m_Devices    ( 0 ),
    m_Sectors    ( 0 ),
    m_Read       ( nullptr ),
    m_Write      ( nullptr ),
    m_RaidStatus ( RAID_STOPPED ),
    m_Timestamp  ( 1 ),
    m_BadDisk    ( -1 ) { }
//======================================================================================================================
class CRaidVolume
{
public:
  static bool Create        ( const TBlkDev & dev );
  int         Start         ( const TBlkDev & dev );
  int         Stop          ( void );
  int         Resync        ( void );
  int         Status        ( void ) const;
  int         Size          ( void ) const;
  bool        Read          ( int secNr, void * data, int secCnt );
  bool        Write         ( int secNr, const void * data, int secCnt );

private:
  struct TDiskServiceData
  {
    int m_Timestamp;
    int m_BadDisk;
  };

  struct TCoord
  {
    TCoord    ( void );

    int m_PhDisk; /**< Physical disk of data. */
    int m_PhSect; /**< Physical sector of data. */
    int m_PrDisk; /**< Physical disk, of the parity. */
    int m_PrSect; /**< Physical sector of the parity. */

    static TCoord  ComputeCoord ( int lgSect );
  };

  static bool CheckDev            ( const TBlkDev & dev );
  static void SaveDev             ( const TBlkDev & dev );
  bool        ComputeMissingData  ( int phDisk, int phSect, char * mDataBuff );

  static TRaidServiceData s_RaidServiceData;
};
TRaidServiceData CRaidVolume::s_RaidServiceData;
//----------------------------------------------------------------------------------------------------------------------
CRaidVolume::TCoord::TCoord ( void ) :
    m_PhDisk ( 0 ),
    m_PhSect ( 0 ),
    m_PrDisk ( 0 ),
    m_PrSect ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
CRaidVolume::TCoord CRaidVolume::TCoord::ComputeCoord ( int lgSect )
{
  TCoord res;
  int shift = ( ( lgSect / s_RaidServiceData . m_Devices ) % ( s_RaidServiceData . m_Devices - 1 ) ) + 1;
  res . m_PhDisk = ( ( lgSect % s_RaidServiceData . m_Devices ) + shift ) % s_RaidServiceData . m_Devices;
  res . m_PhSect = lgSect / ( s_RaidServiceData . m_Devices - 1 );
  res . m_PrDisk = ( ( lgSect / ( s_RaidServiceData . m_Devices - 1 ) ) ) % s_RaidServiceData . m_Devices;
  res . m_PrSect = res . m_PhSect;
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
bool CRaidVolume::Create ( const TBlkDev & dev )
{
  if ( ! CheckDev ( dev ) )
  {
    s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
    return false;
  }

  SaveDev ( dev );
  s_RaidServiceData . m_RaidStatus =  RAID_STOPPED;
  s_RaidServiceData . m_Timestamp  =  1;
  s_RaidServiceData . m_BadDisk    = -1;

  char buffer[SECTOR_SIZE];

  memcpy ( buffer, & s_RaidServiceData . m_Timestamp, sizeof ( s_RaidServiceData . m_Timestamp ) );
  memcpy ( buffer + sizeof ( s_RaidServiceData . m_Timestamp ), & s_RaidServiceData . m_BadDisk, sizeof ( s_RaidServiceData . m_BadDisk ) );

  for ( int diskNr = 0; diskNr < s_RaidServiceData . m_Devices; ++ diskNr )
    if ( s_RaidServiceData . m_Write ( diskNr, s_RaidServiceData . m_Sectors - 1, buffer, 1 ) != 1 )
    {
      s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
      s_RaidServiceData . m_BadDisk = diskNr;
      return false;
    }

  s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
int CRaidVolume::Start ( const TBlkDev & dev )
{
  int failures = 0, failureIdx;
  const int MIN_RAID_DEVICES = 3;
  TDiskServiceData disksServiceData [MIN_RAID_DEVICES], tmp;
  char buffer[SECTOR_SIZE];

  if ( ! CheckDev ( dev ) )
  {
    s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
    return s_RaidServiceData . m_RaidStatus;
  }

  SaveDev ( dev );
  s_RaidServiceData . m_RaidStatus = RAID_STOPPED;

  if ( s_RaidServiceData . m_RaidStatus != RAID_STOPPED )
    return s_RaidServiceData . m_RaidStatus;

  /* Read service data from first three disks. */
  for ( int diskNr = 0; diskNr < MIN_RAID_DEVICES; ++ diskNr )
  {
    memset ( buffer, 0, SECTOR_SIZE );
    if ( s_RaidServiceData . m_Read ( diskNr, s_RaidServiceData . m_Sectors - 1, buffer, 1 ) != 1 )
    {
      ++ failures;
      failureIdx = diskNr;
    }
    else
    {
      memcpy ( & disksServiceData[diskNr] . m_Timestamp, buffer, sizeof ( disksServiceData[diskNr] . m_Timestamp ) );
      memcpy ( & disksServiceData[diskNr] . m_BadDisk, buffer + sizeof ( disksServiceData[diskNr] . m_Timestamp ), sizeof ( disksServiceData[diskNr] . m_BadDisk ) );

      if ( disksServiceData[diskNr] . m_BadDisk   < -1 || disksServiceData[diskNr] . m_BadDisk   >= s_RaidServiceData . m_Devices
           || disksServiceData[diskNr] . m_Timestamp <  1 || disksServiceData[diskNr] . m_Timestamp >  s_RaidServiceData . m_Timestamp )
      {
        ++ failures;
        failureIdx = diskNr;
      }
    }
  }

  /* 1 */
  if ( failures == 0
       && disksServiceData[0] . m_Timestamp == disksServiceData[1] . m_Timestamp
       && disksServiceData[1] . m_Timestamp == disksServiceData[2] . m_Timestamp )
  {
    s_RaidServiceData . m_Timestamp  = disksServiceData[0] . m_Timestamp;
    s_RaidServiceData . m_RaidStatus = RAID_OK;
    s_RaidServiceData . m_BadDisk    = disksServiceData[0] . m_BadDisk;
  }

  /* 2a - 0 */
  if ( failures == 0
       && disksServiceData[0] . m_Timestamp != disksServiceData[1] . m_Timestamp
       && disksServiceData[1] . m_Timestamp == disksServiceData[2] . m_Timestamp )
  {
    if ( disksServiceData[1] . m_BadDisk == disksServiceData[2] . m_BadDisk
         && ( disksServiceData[1] . m_BadDisk == -1 || disksServiceData[1] . m_BadDisk == 0 ) )
    {
      s_RaidServiceData . m_Timestamp  = disksServiceData[1] . m_Timestamp;
      s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
      s_RaidServiceData . m_BadDisk    = 0;
    }
    else
    {
      s_RaidServiceData . m_RaidStatus = RAID_FAILED;
      return s_RaidServiceData . m_RaidStatus;
    }
  }

  /* 2b - 1 */
  if ( failures == 0
       && disksServiceData[0] . m_Timestamp != disksServiceData[1] . m_Timestamp
       && disksServiceData[0] . m_Timestamp == disksServiceData[2] . m_Timestamp )
  {
    if ( disksServiceData[0] . m_BadDisk   == disksServiceData[2] . m_BadDisk
         && ( disksServiceData[0] . m_BadDisk == -1 || disksServiceData[0] . m_BadDisk == 1 ) )
    {
      s_RaidServiceData . m_Timestamp  = disksServiceData[0] . m_Timestamp;
      s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
      s_RaidServiceData . m_BadDisk    = 1;
    }
    else
    {
      s_RaidServiceData . m_RaidStatus = RAID_FAILED;
      return s_RaidServiceData . m_RaidStatus;
    }
  }

  /* 2c - 2 */
  if ( failures == 0
       && disksServiceData[1] . m_Timestamp != disksServiceData[2] . m_Timestamp
       && disksServiceData[0] . m_Timestamp == disksServiceData[1] . m_Timestamp )
  {
    if ( disksServiceData[0] . m_BadDisk   == disksServiceData[1] . m_BadDisk
         && ( disksServiceData[0] . m_BadDisk == -1 || disksServiceData[0] . m_BadDisk == 2 ) )
    {
      s_RaidServiceData . m_Timestamp  = disksServiceData[0] . m_Timestamp;
      s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
      s_RaidServiceData . m_BadDisk    = 2;
    }
    else
    {
      s_RaidServiceData . m_RaidStatus = RAID_FAILED;
      return s_RaidServiceData . m_RaidStatus;
    }
  }

  /* 3 */
  if ( failures == 1
       && disksServiceData[(failureIdx + 1) % MIN_RAID_DEVICES] . m_Timestamp == disksServiceData[(failureIdx + 2) % MIN_RAID_DEVICES] . m_Timestamp )
  {
    s_RaidServiceData . m_Timestamp = disksServiceData[(failureIdx + 1) % MIN_RAID_DEVICES] . m_Timestamp;
    s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
    s_RaidServiceData . m_BadDisk = failureIdx;
  }

  /* 4 + 5 */
  if ( ( failures == 1
         && disksServiceData[(failureIdx + 1) % MIN_RAID_DEVICES] . m_Timestamp != disksServiceData[(failureIdx + 2) % MIN_RAID_DEVICES] . m_Timestamp )
       || failures >= 2 || ( disksServiceData[0] . m_Timestamp != disksServiceData[1] . m_Timestamp
                          && disksServiceData[1] . m_Timestamp != disksServiceData[2] . m_Timestamp
                          && disksServiceData[0] . m_Timestamp != disksServiceData[2] . m_Timestamp ) )
  {
    s_RaidServiceData . m_RaidStatus = RAID_FAILED;
    return s_RaidServiceData . m_RaidStatus;
  }

  /* If status is RAID_OK or RAID_DEGRADED, continue loading service data from disks. */
  if ( s_RaidServiceData . m_RaidStatus == RAID_OK || s_RaidServiceData . m_RaidStatus == RAID_DEGRADED )
  {
    for ( int diskNr = MIN_RAID_DEVICES; diskNr < s_RaidServiceData . m_Devices; ++ diskNr )
    {
      /* Reading from disk failed. */
      memset ( buffer, 0, SECTOR_SIZE );
      if ( s_RaidServiceData . m_Read ( diskNr, s_RaidServiceData . m_Sectors - 1, buffer, 1 ) != 1 )
      {
        if ( s_RaidServiceData . m_RaidStatus == RAID_OK )
        {
          s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
          s_RaidServiceData . m_BadDisk    = diskNr;
        }
        else if ( s_RaidServiceData . m_RaidStatus == RAID_DEGRADED )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          return s_RaidServiceData . m_RaidStatus;
        }
      }
        /* Reading from disk succeeded. */
      else
      {
        memcpy ( & tmp . m_Timestamp, buffer, sizeof ( tmp . m_Timestamp ) );
        memcpy ( & tmp . m_BadDisk, buffer + sizeof ( tmp . m_Timestamp ), sizeof ( tmp . m_BadDisk ) );

        if ( tmp . m_Timestamp  != s_RaidServiceData . m_Timestamp )
        {
          if ( s_RaidServiceData . m_RaidStatus == RAID_OK )
          {
            s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
            s_RaidServiceData . m_BadDisk     = diskNr;
          }
          else
          {
            s_RaidServiceData . m_RaidStatus = RAID_FAILED;
            return s_RaidServiceData . m_RaidStatus;
          }
        }
      }
    }
  }
  return s_RaidServiceData . m_RaidStatus;
}
//----------------------------------------------------------------------------------------------------------------------
int CRaidVolume::Stop ( void )
{
  ++ s_RaidServiceData . m_Timestamp;

  if ( s_RaidServiceData . m_RaidStatus == RAID_FAILED )
    s_RaidServiceData . m_RaidStatus = RAID_STOPPED;

  if ( s_RaidServiceData . m_RaidStatus == RAID_STOPPED )
    return s_RaidServiceData . m_RaidStatus;

  char buffer[SECTOR_SIZE];
  memset ( buffer, 0, SECTOR_SIZE );
  memcpy ( buffer, & s_RaidServiceData . m_Timestamp, sizeof ( s_RaidServiceData . m_Timestamp ) );
  memcpy ( buffer + sizeof ( s_RaidServiceData . m_Timestamp ), & s_RaidServiceData . m_BadDisk, sizeof ( s_RaidServiceData . m_BadDisk ) );

  for ( int diskNr = 0; diskNr < s_RaidServiceData . m_Devices; ++ diskNr )
  {
    if ( diskNr == s_RaidServiceData . m_BadDisk )
      continue;

    if ( s_RaidServiceData . m_Write ( diskNr, s_RaidServiceData . m_Sectors - 1, buffer, 1 ) != 1 )
    {
      switch ( s_RaidServiceData . m_RaidStatus )
      {
        case RAID_OK:
          s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
          s_RaidServiceData . m_BadDisk    = diskNr;
          diskNr = -1;
          break;
        case RAID_DEGRADED:
          s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
          return s_RaidServiceData . m_RaidStatus;
        default:
          break;
      }
    }
  }
  s_RaidServiceData . m_RaidStatus = RAID_STOPPED;
  return s_RaidServiceData . m_RaidStatus;
}
//----------------------------------------------------------------------------------------------------------------------
int CRaidVolume::Resync ( void )
{
  if ( s_RaidServiceData . m_RaidStatus != RAID_DEGRADED )
    return s_RaidServiceData . m_RaidStatus;

  /* Compute missing data for all sectors except the last one (service data). */
  char missingData[SECTOR_SIZE], serviceData[SECTOR_SIZE];
  int diskToRestore = s_RaidServiceData . m_BadDisk;

  for ( int sectorIter = 0; sectorIter < s_RaidServiceData . m_Sectors - 1; ++ sectorIter )
  {
    /* Compute missing data and handle potential disk failure. */
    if ( ! ComputeMissingData ( diskToRestore, sectorIter, missingData ) )
    {
      if ( s_RaidServiceData . m_BadDisk != diskToRestore )
        s_RaidServiceData . m_RaidStatus = RAID_FAILED;
      return s_RaidServiceData . m_RaidStatus;
    }

    /* Write computed missing data to the disk under restoration. */
    if ( s_RaidServiceData . m_Write ( diskToRestore, sectorIter, missingData, 1 ) != 1 )
      return s_RaidServiceData . m_RaidStatus;
  }

  /* Read service data from another disk. */
  memset ( serviceData, 0, SECTOR_SIZE );
  if ( s_RaidServiceData . m_Read ( ( diskToRestore + 1 ) % s_RaidServiceData . m_Devices, s_RaidServiceData . m_Sectors - 1, serviceData, 1 ) != 1 )
  {
    s_RaidServiceData . m_RaidStatus = RAID_FAILED;
    return s_RaidServiceData . m_RaidStatus;
  }

  /* Write service data to the disk under restoration. */
  if ( s_RaidServiceData . m_Write ( diskToRestore, s_RaidServiceData . m_Sectors - 1, serviceData, 1 ) != 1 )
    return s_RaidServiceData . m_RaidStatus;

  s_RaidServiceData . m_RaidStatus = RAID_OK;
  return s_RaidServiceData . m_RaidStatus;
}
//----------------------------------------------------------------------------------------------------------------------
int CRaidVolume::Status ( void ) const
{
  return s_RaidServiceData . m_RaidStatus;
}
//----------------------------------------------------------------------------------------------------------------------
int CRaidVolume::Size ( void ) const
{
  return ( s_RaidServiceData . m_Devices - 1) * ( s_RaidServiceData . m_Sectors - 1 );
}
//----------------------------------------------------------------------------------------------------------------------
bool CRaidVolume::Read ( int secNr, void * data, int secCnt )
{
  TCoord coord;
  int offset = 0;
  while ( secCnt )
  {
    coord = TCoord::ComputeCoord ( secNr + offset );

    if ( s_RaidServiceData . m_RaidStatus == RAID_OK )
    {
      if ( s_RaidServiceData . m_Read ( coord . m_PhDisk, coord . m_PhSect, ((char *) data) + offset * SECTOR_SIZE, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
        s_RaidServiceData . m_BadDisk    = coord . m_PhDisk;
      }
    }

    if ( s_RaidServiceData . m_RaidStatus == RAID_DEGRADED )
    {
      /* Reading from bad disk. */
      if ( coord . m_PhDisk == s_RaidServiceData . m_BadDisk )
      {
        char missingData[SECTOR_SIZE];
        if ( ! ComputeMissingData ( coord . m_PhDisk, coord . m_PhSect, missingData ) )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
        memcpy ( ((char*)data) + offset * SECTOR_SIZE, missingData, SECTOR_SIZE );
      }
        /* Reading from good disk. */
      else
      {
        if ( s_RaidServiceData . m_Read ( coord . m_PhDisk, coord . m_PhSect, ((char *) data) + offset * SECTOR_SIZE, 1 ) != 1 )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
      }
    }
    ++ offset;
    -- secCnt;
  }
  return s_RaidServiceData . m_RaidStatus != RAID_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
bool CRaidVolume::Write ( int secNr, const void * data, int secCnt )
{
  TCoord coord;
  int offset = 0;
  char oldData[SECTOR_SIZE], newData[SECTOR_SIZE], parity[SECTOR_SIZE];

  if ( s_RaidServiceData . m_RaidStatus == RAID_OK )
  {
    while ( secCnt )
    {
      coord = TCoord::ComputeCoord ( secNr + offset );

      /* Read old data. */
      memset ( oldData, 0, SECTOR_SIZE );
      if ( s_RaidServiceData . m_Read ( coord . m_PhDisk, coord . m_PhSect, oldData, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
        s_RaidServiceData . m_BadDisk    = coord . m_PhDisk;
        break;
      }

      /* Read old parity. */
      memset ( parity, 0, SECTOR_SIZE );
      if ( s_RaidServiceData . m_Read ( coord . m_PrDisk, coord . m_PrSect, parity, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
        s_RaidServiceData . m_BadDisk    = coord . m_PrDisk;
        break;
      }

      /* XOR. */
      memset ( newData, 0, SECTOR_SIZE );
      memcpy ( newData, ( ((char *) data) + offset * SECTOR_SIZE ), SECTOR_SIZE );
      for ( int i = 0; i < SECTOR_SIZE; ++ i )
        parity[i] = parity[i] ^ oldData[i] ^ newData[i];

      /* Write new parity. */
      if ( s_RaidServiceData . m_Write ( coord . m_PrDisk, coord . m_PrSect, parity, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
        s_RaidServiceData . m_BadDisk    = coord . m_PrDisk;
        break;
      }

      /* Write new data. */
      if ( s_RaidServiceData . m_Write ( coord . m_PhDisk, coord . m_PhSect, newData, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_DEGRADED;
        s_RaidServiceData . m_BadDisk    = coord . m_PhDisk;
        break;
      }
      ++ offset;
      -- secCnt;
    }
  }

  if ( s_RaidServiceData . m_RaidStatus == RAID_DEGRADED )
  {
    while ( secCnt )
    {
      coord = TCoord::ComputeCoord ( secNr + offset );

      /* Read old data from bad disk. */
      if ( coord . m_PhDisk == s_RaidServiceData . m_BadDisk )
      {
        if ( ! ComputeMissingData ( coord . m_PhDisk, coord . m_PhSect, oldData ) )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
      }
        /* Reading old data from good disk. */
      else
      {
        memset ( oldData, 0, SECTOR_SIZE );
        if ( s_RaidServiceData . m_Read ( coord . m_PhDisk, coord . m_PhSect, oldData, 1 ) != 1 )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
      }

      /* Read old parity from bad disk. */
      if ( coord . m_PrDisk == s_RaidServiceData . m_BadDisk )
      {
        if ( ! ComputeMissingData ( coord . m_PrDisk, coord . m_PrSect, parity ) )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
      }
        /* Reading old parity from good disk. */
      else
      {
        memset ( parity, 0, SECTOR_SIZE );
        if ( s_RaidServiceData . m_Read ( coord . m_PrDisk, coord . m_PrSect, parity, 1 ) != 1 )
        {
          s_RaidServiceData . m_RaidStatus = RAID_FAILED;
          break;
        }
      }

      /* XOR. */
      memset ( newData, 0, SECTOR_SIZE );
      memcpy ( newData, ( ((char *) data) + offset * SECTOR_SIZE ), SECTOR_SIZE );
      for ( int i = 0; i < SECTOR_SIZE; ++ i )
        parity[i] = parity[i] ^ oldData[i] ^ newData[i];

      /* Write new data only if the corresponding disk is working. */
      if ( coord . m_PhDisk != s_RaidServiceData . m_BadDisk
           && s_RaidServiceData . m_Write ( coord . m_PhDisk, coord . m_PhSect, newData, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_FAILED;
        break;
      }

      /* Write new parity only if the corresponding disk is working. */
      if ( coord . m_PrDisk != s_RaidServiceData . m_BadDisk
           && s_RaidServiceData. m_Write ( coord . m_PrDisk, coord . m_PrSect, parity, 1 ) != 1 )
      {
        s_RaidServiceData . m_RaidStatus = RAID_FAILED;
        break;
      }
      ++ offset;
      -- secCnt;
    }
  }
  return s_RaidServiceData . m_RaidStatus != RAID_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
bool CRaidVolume::CheckDev ( const TBlkDev & dev )
{
  const int MIN_RAID_DEVICES = 3;
  return ! (   dev . m_Devices < MIN_RAID_DEVICES
          ||   dev . m_Sectors < MIN_DEVICE_SECTORS
          ||   dev . m_Devices > MAX_RAID_DEVICES
          || ! dev . m_Sectors
          || ! dev . m_Read
          || ! dev . m_Write );
}
//----------------------------------------------------------------------------------------------------------------------
void CRaidVolume::SaveDev ( const TBlkDev & dev )
{
  s_RaidServiceData . m_Devices =  dev . m_Devices;
  s_RaidServiceData . m_Sectors =  dev . m_Sectors;
  s_RaidServiceData . m_Read    =  dev . m_Read;
  s_RaidServiceData . m_Write   =  dev . m_Write;
}
//----------------------------------------------------------------------------------------------------------------------
bool CRaidVolume::ComputeMissingData ( int phDisk, int phSect, char * mDataBuff )
{
  char tmpBuff[SECTOR_SIZE];

  /* Initialise buffers. */
  memset ( mDataBuff, 0, SECTOR_SIZE );
  memset ( tmpBuff,   0, SECTOR_SIZE );

  for ( int diskIdxIter = 0; diskIdxIter < s_RaidServiceData . m_Devices; ++ diskIdxIter )
  {
    /* Exclude broken disk. */
    if ( diskIdxIter == phDisk )
      continue;

    memset ( tmpBuff, 0, SECTOR_SIZE );
    if ( s_RaidServiceData . m_Read ( diskIdxIter, phSect, tmpBuff, 1 ) != 1 )
    {
      s_RaidServiceData . m_BadDisk = diskIdxIter;
      return false;
    }

    for ( int i = 0; i < SECTOR_SIZE; ++ i )
      mDataBuff[i] ^= tmpBuff[i];
  }
  return true;
}
//======================================================================================================================
#ifndef __PROGTEST__
#include "tests.inc"
#endif /* __PROGTEST__ */