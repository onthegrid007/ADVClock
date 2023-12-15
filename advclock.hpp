/*
*   BSD 3-Clause License, see file labled 'LICENSE' for the full License.
*   Copyright (c) 2023, Peter Ferranti
*   All rights reserved.
*/

#ifndef ADVCLOCK_HPP_
#define ADVCLOCK_HPP_

#include "vendor/Timestamp/timestamp.hpp"

#ifdef _ADVClock_STATIC_INSTANCE_TYPE_OVERRIDE
  typedef _ADVClock_STATIC_INSTANCE_TYPE_OVERRIDE GlobalClockType;
#else
  typedef std::chrono::high_resolution_clock GlobalClockType;
#endif

inline const _Timestamp<GlobalClockType, __uint128_t> GLOBAL_CLOCK{};

template<typename C = GlobalClockType, typename T = std::uint64_t, typename D = std::chrono::duration<T, std::nano>>
class _ADVClock : public _Timestamp<C, T, D> {
  public:
  typedef C ClockType;
  typedef D Duration;
  typedef _Timestamp<ClockType, T, Duration> TS;
  _ADVClock() : TS(std::chrono::time_point_cast<Duration>(ClockType::now())) { }
  explicit constexpr _ADVClock(const TS& ts) : TS(ts) { }
  
  enum Precision : std::uint8_t {
    NanoS = 0,
    MicroS = 1,
    MilliS = 2,
    Seconds = 3,
    Minutes = 4,
    Hours = 5,
    Days = 6,
    Weeks = 7,
    Months = 8,
    Years = 9
  };
  
  const typename TS::Duration beginDur() const {
    return std::chrono::duration_cast<TS::Duration>(this->TS) - GLOBAL_CLOCK;
  }
  
  const typename TS::Duration nowDur() const {
    return ClockType::now() - GLOBAL_CLOCK; 
  }
  
  void tare() { *static_cast<TS::TP*>(this) = ClockType::now(); }
  
  const typename TS::Duration elapsedDur(const bool tareClock) {
    const auto rtn = nowDur() - beginDur();
    if(tareClock) tare();
    return rtn;
  }
  
  const T elapsed(const bool tareClock) {
    return elapsedDur(tareClock).count();
  }
  
  template<typename __RtnT, typename __T = uint64_t>
  static const __RtnT ElapsedRuntimeCast(std::chrono::duration<__T, std::nano> cast, Precision&& p = Precision::NanoS) {
    typedef __RtnT RT;
    typedef __T Type;
    #define __SecsInMin 60
    #define __MinsInHour 60
    #define __HoursInDay 24
    #define __DaysInWeek 7
    #define __DaysInYear 365.24
    #define __MonthsInYear 12
    #define __divb1k(__x) (__x / RT(1000.0))
    #define __nanos Type(cast.count())
    #define __micros __divb1k(__nanos)
    #define __millis __divb1k(__micros)
    #define __secs __divb1k(__millis)
    #define __mins (__secs / __SecsInMin)
    #define __hours (__mins / __MinsInHour)
    #define __days (__hours / __HoursInDay)
    #define __weeks (__days / __DaysInWeek)
    #define __years (__days / __DaysInYear)
    #define __months (__years / __MonthsInYear)
    switch(p) {
      case Precision::NanoS:
      return __nanos;
      case Precision::MicroS:
      return __micros;
      case Precision::MilliS:
      return __millis;
      case Precision::Seconds:
      return __secs;
      case Precision::Minutes:
      return __mins;
      case Precision::Hours:
      return __hours;
      case Precision::Days:
      return __days;
      case Precision::Weeks:
      return __weeks;
      case Precision::Months:
      return __months;
      case Precision::Years:
      return __years;
      default:
      return __nanos;
    };
  };

  template<typename _RtnT, typename __T = uint64_t>
  const _RtnT elapsedRuntimeCast(Precision&& p = Precision::NanoS, const bool tareClock = false) {
    return ElapsedRuntimeCast<_RtnT, __T>(elapsedDur(tareClock), p);
  }
};

typedef _ADVClock<> ADVClock;

#endif