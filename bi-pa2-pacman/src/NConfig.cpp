#include "NConfig.hpp"

//----------------------------------------------------------------------------------------------------------------------
int NConfig::Indent ( const int area, const int size )
{
  return ( area - size ) / 2;
}
//----------------------------------------------------------------------------------------------------------------------
int NConfig::Mod ( int nmb, int mod )
{
  return ( nmb + mod ) % mod;
}
//----------------------------------------------------------------------------------------------------------------------