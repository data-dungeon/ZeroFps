#ifndef STATCOUNTER_H
#define STATCOUNTER_H

#include <string>
   using namespace std;

class StatCounter
{
private:   
   float
      m_fValue,
      m_fMax,
      m_fMin;
public:
   StatCounter();
   
   void operator+= (float fValue);
   void operator- (float fValue);
   void operator= (string kValue);
   void operator= (float fValue);

   float Value()                          { return m_fValue; }
   float Percent()                        { return m_fValue / m_fMax; }

   void SetMaxValue (float fMax)          { m_fMax = fMax; }
   void SetMinValue (float fMin)          { m_fMin = fMin; }

   float Max()                            { return m_fMax; }

};


#endif








