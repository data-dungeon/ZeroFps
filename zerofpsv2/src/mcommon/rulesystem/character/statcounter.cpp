#include "statcounter.h"
#include <iostream>
   using namespace std;

// ----------------------------------------------------------------------------------------------

void StatCounter::operator+= (int iValue)
{
   m_iValue += iValue;

   if ( m_iValue > m_iMax )
      m_iValue = m_iMax;
   if ( m_iValue < m_iMin )
      m_iValue = m_iMin;
}

// ----------------------------------------------------------------------------------------------

void StatCounter::operator- (int iValue)
{
   m_iValue -= iValue;

   if ( m_iValue > m_iMax )
      m_iValue = m_iMax;
   if ( m_iValue < m_iMin )
      m_iValue = m_iMin;
}

// ----------------------------------------------------------------------------------------------

void StatCounter::operator= (string kValue)
{
   if ( kValue == "Max" )
      m_iValue = m_iMax;
   else
   {
      m_iValue = atoi ( kValue.c_str() );

      if ( m_iValue > m_iMax )
         m_iValue = m_iMax;
      else if ( m_iValue < m_iMin )
         m_iValue = m_iMin;

   }

   cout << "Set LifeTo (str)" << m_iValue << endl;

}
// ----------------------------------------------------------------------------------------------

void StatCounter::operator= (int iValue)
{
   m_iValue = iValue;

   if ( m_iValue > m_iMax )
      m_iValue = m_iMax;
   else if ( m_iValue < m_iMin )
      m_iValue = m_iMin;

   cout << "setLifeto(int)" << m_iValue << endl;
}
// ----------------------------------------------------------------------------------------------

StatCounter::StatCounter()
{
   m_iMax = m_iMin = m_iValue = 0;
}


// ----------------------------------------------------------------------------------------------







