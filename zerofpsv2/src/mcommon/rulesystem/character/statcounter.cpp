#include "statcounter.h"

// ----------------------------------------------------------------------------------------------

void StatCounter::operator+= (float fValue)
{
   m_fValue += fValue;

   if ( m_fValue > m_fMax )
      m_fValue = m_fMax;
   if ( m_fValue < m_fMin )
      m_fValue = m_fMin;
}

// ----------------------------------------------------------------------------------------------

void StatCounter::operator- (float fValue)
{
   m_fValue -= fValue;

   if ( m_fValue > m_fMax )
      m_fValue = m_fMax;
   if ( m_fValue < m_fMin )
      m_fValue = m_fMin;
}

// ----------------------------------------------------------------------------------------------

void StatCounter::operator= (string kValue)
{
   if ( kValue == "Max" )
      m_fValue = m_fMax;
   else
   {
      m_fValue = atof ( kValue.c_str() );

      if ( m_fValue > m_fMax )
         m_fMax = m_fValue;

      if ( m_fValue < m_fMin )
         m_fValue = m_fMin;
   }

}

// ----------------------------------------------------------------------------------------------

StatCounter::StatCounter()
{
   m_fMax = m_fMin = m_fValue = 0;
}


// ----------------------------------------------------------------------------------------------







