#ifndef STATCOUNTER_H
#define STATCOUNTER_H

#include "../../mcommon_x.h"
#include <string>
   using namespace std;

class MCOMMON_API StatCounter
{
private:   
   int
      m_iValue,
      m_iMax,
      m_iMin;
public:
   StatCounter();
   
   void operator+= (int fValue);
   void operator- (int fValue);
   StatCounter& operator= (string kValue);
   void operator= (int fValue);

   int Value()                          { return m_iValue; }
   float Percent()                      { return float(m_iValue) / float(m_iMax); }

   void SetMaxValue (int iMax)          { m_iMax = iMax; }
   void SetMinValue (int iMin)          { m_iMin = iMin; }

   int Max()                            { return m_iMax; }

};


#endif








