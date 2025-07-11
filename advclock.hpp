/*
*   BSD 3-Clause License, see file labled 'LICENSE' for the full License.
*   Copyright (c) 2025, Peter Ferranti
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

inline const _Timestamp<GlobalClockType, std::uint64_t> GLOBAL_CLOCK{};

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
		MicroS,
		MilliS,
		Seconds,
		Minutes,
		Hours,
		Days,
		Weeks,
		Months,
		Years
	};
  
	const typename TS::Duration beginDur() const {
		return *this - GLOBAL_CLOCK;
	}

	const typename TS::Duration nowDur() const {
		return ClockType::now() - GLOBAL_CLOCK; 
	}

	void tare() { *static_cast<typename TS::TP*>(this) = ClockType::now(); }

	const typename TS::Duration elapsedDur(const bool tareClock) {
		const auto rtn{nowDur() - beginDur()};
		if(tareClock) tare();
		return rtn;
	}

	const T elapsed(const bool tareClock) {
		return elapsedDur(tareClock).count();
	}
  
	template<typename _ADVC_RtnT, typename _ADVC_T = uint64_t>
	static const _ADVC_RtnT elapsedRuntimeCast(std::chrono::duration<_ADVC_T, std::nano> cast, Precision&& p = Precision::NanoS) {
		typedef _ADVC_RtnT RT;
		typedef _ADVC_T Type;
		#define _ADVC_SecsInMin 60
		#define _ADVC_MinsInHour 60
		#define _ADVC_HoursInDay 24
		#define _ADVC_DaysInWeek 7
		#define _ADVC_DaysInYear 365.24
		#define _ADVC_MonthsInYear 12
		#define _ADVC_divb1k(_ADVC_x) (_ADVC_x / RT(1000.0))
		#define _ADVC_nanos Type(cast.count())
		#define _ADVC_micros _ADVC_divb1k(_ADVC_nanos)
		#define _ADVC_millis _ADVC_divb1k(_ADVC_micros)
		#define _ADVC_secs _ADVC_divb1k(_ADVC_millis)
		#define _ADVC_mins (_ADVC_secs / _ADVC_SecsInMin)
		#define _ADVC_hours (_ADVC_mins / _ADVC_MinsInHour)
		#define _ADVC_days (_ADVC_hours / _ADVC_HoursInDay)
		#define _ADVC_weeks (_ADVC_days / _ADVC_DaysInWeek)
		#define _ADVC_years (_ADVC_days / _ADVC_DaysInYear)
		#define _ADVC_months (_ADVC_years / _ADVC_MonthsInYear)
		switch(p) {
			case Precision::NanoS:
			return _ADVC_nanos;
			case Precision::MicroS:
			return _ADVC_micros;
			case Precision::MilliS:
			return _ADVC_millis;
			case Precision::Seconds:
			return _ADVC_secs;
			case Precision::Minutes:
			return _ADVC_mins;
			case Precision::Hours:
			return _ADVC_hours;
			case Precision::Days:
			return _ADVC_days;
			case Precision::Weeks:
			return _ADVC_weeks;
			case Precision::Months:
			return _ADVC_months;
			case Precision::Years:
			return _ADVC_years;
			default:
			return _ADVC_nanos;
		};
	};

	template<typename _RtnT, typename _ADVC_T = uint64_t>
	const _RtnT elapsedRuntimeCast(Precision&& p = Precision::NanoS, const bool tareClock = false) {
		return ElapsedRuntimeCast<_RtnT, _ADVC_T>(elapsedDur(tareClock), p);
	}

	static const std::string GetDateTime(const std::string& fmt) {
		const auto time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
		std::stringstream ss;
		ss << std::put_time(std::localtime(&time), fmt.c_str());
		return ss.str();
	}
};

typedef _ADVClock<> ADVClock;

#endif