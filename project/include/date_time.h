/*
* date_time.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef DATE_TIME_H_
#define DATE_TIME_H_

#include "primitive_types.h"

enum class DayOfWeek {
	Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
};

class Date;
class Time;

class TimeSpan {
private:
	static constexpr Int64 TicksPerDay = 864000000000L;
	static constexpr Int64 TicksPerHour = 36000000000L;
	static constexpr Int64 TicksPerMinute = 36000000000L;
	static constexpr Int64 TicksPerSecond = 10000000;
	static constexpr Int64 TicksPerMillisecond = 10000L;
	static constexpr Int64 HoursPerDay = 24;
	static constexpr Int64 MinutesPerHour = 60;
	static constexpr Int64 SecondsPerMinute = 60;
	static constexpr Int64 MillisPerSecond = 1000;
	UInt64 m_Tick;

public:
	inline TimeSpan(UInt64 ticks) :
		m_Tick(ticks) {
	}
	inline int GetTotalHour() {
		return int(m_Tick / TicksPerHour);
	}
	inline Int64 GetTotalMinute() {
		return m_Tick / TicksPerMinute;
	}
	inline Int64 GetTotalSecond() {
		return m_Tick / TicksPerSecond;
	}
	inline Int64 GetTotalMillisecond() {
		return m_Tick / TicksPerMillisecond;
	}

	inline int GetDay() {
		return int(m_Tick / TicksPerDay);
	}
	inline int GetHour() {
		return GetTotalHour() % HoursPerDay;
	}
	inline int GetMinute() {
		return GetTotalMinute() % MinutesPerHour;
	}
	inline int GetSecond() {
		return GetTotalSecond() % SecondsPerMinute;
	}
	inline int GetMillisecond() {
		return GetTotalMillisecond() % MillisPerSecond;
	}
	inline TimeSpan operator +(TimeSpan &addend) {
		return TimeSpan(m_Tick + addend.m_Tick);
	}

	static inline TimeSpan FromHour(int hours) {
		return TimeSpan(hours * TicksPerHour);
	}
	static inline TimeSpan FromMinute(Int64 minutes) {
		return TimeSpan(minutes * TicksPerMinute);
	}
	static inline TimeSpan FromSecond(Int64 seconds) {
		return TimeSpan(seconds * TicksPerSecond);
	}
	static inline TimeSpan FromMillisecond(Int64 milliseconds) {
		return TimeSpan(milliseconds * TicksPerMillisecond);
	}
	static inline TimeSpan GetZero() {
		return TimeSpan(0);
	}
	friend Time;
};
inline TimeSpan operator ""_h(unsigned long long int hours) {
	return TimeSpan::FromHour(int(hours));
}
inline TimeSpan operator ""_min(unsigned long long int minutes) {
	return TimeSpan::FromMinute(Int64(minutes));
}
inline TimeSpan operator ""_sec(unsigned long long int seconds) {
	return TimeSpan::FromSecond(Int64(seconds));
}
inline TimeSpan operator ""_msec(unsigned long long int milliseconds) {
	return TimeSpan::FromMillisecond(Int64(milliseconds));
}

class Time {
public:
	static const Time AD;
	inline Time(Time &time) :
		m_TimeData(time.m_TimeData) {
	}
	inline Time& operator =(const Time &source) {
		m_TimeData = source.m_TimeData;
		return *this;
	}
	inline TimeSpan operator -(const Time &base) {
		return TimeSpan(m_TimeData.m_Tick - base.m_TimeData.m_Tick);
	}

private:
	struct {
		UInt64 m_Zone : 6;
		UInt64 m_Tick : 58;
	} m_TimeData;
	inline int GetWholeDays() {
		TimeSpan timeFromAd = *this - AD;
		return timeFromAd.GetDay();
	}

public:
	inline Time(UInt64 time) :
		m_TimeData{0, time} {
	}
	Date GetDate();
	int GetDayOfMonth();
	DayOfWeek GetDayOfWeek();
	int GetDayOfYear();
	int GetMonth();
	int GetYear();
	TimeSpan GetTimeOfDay();
	int GetHour();
	int GetMinute();
	int GetSecond();
	int GetMillisecond();

	Time operator +(TimeSpan &difference);
	Time operator -(TimeSpan &difference);

	bool operator ==(Time &value);
	bool operator !=(Time &value);
	bool operator >(Time &value);
	bool operator >=(Time &value);
	bool operator <(Time &value);
	bool operator <=(Time &value);
};
Time GetNowTime();

class DateSpan {
public:
};

class Date {
private:
	unsigned m_Days;

public:
	Date(unsigned days);
	int GetDayOfMonth();
	DayOfWeek GetDayOfWeek();
	int GetDayOfYear();
	int GetMonth();
	int GetYear();
};
Date GetTodayDate();

#endif