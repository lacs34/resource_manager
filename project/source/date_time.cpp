#include "date_time.h"
static constexpr Int64 TicksPerDay = 864000000000L;
constexpr Int64 DaysPerWeek = 7;

constexpr int DaysPer400Years = 146097;
constexpr int DaysPer100Years = 36524;
constexpr int DaysPer4Years = 1461;
constexpr int DaysPerYear = 365;

static const int DaysToMonth366[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
static const int DaysToMonth365[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

const Time Time::AD(0);
Date Time::GetDate() {
	return Date(GetWholeDays());
}

int Time::GetDayOfMonth() {
	Date date(GetWholeDays());
	return date.GetDayOfMonth();
}

DayOfWeek Time::GetDayOfWeek() {
	Date date(GetWholeDays());
	return date.GetDayOfWeek();
}

int Time::GetDayOfYear() {
	Date date(GetWholeDays());
	return date.GetDayOfYear();
}

int Time::GetMonth() {
	Date date(GetWholeDays());
	return date.GetMonth();
}

int Time::GetYear() {
	Date date(GetWholeDays());
	return date.GetYear();
}

TimeSpan Time::GetTimeOfDay() {
	return TimeSpan(m_TimeData.m_Tick % TicksPerDay);
}

int Time::GetHour() {
	return (*this - AD).GetHour();
}

int Time::GetMinute() {
	return (*this - AD).GetMinute();
}

int Time::GetSecond() {
	return (*this - AD).GetSecond();
}

int Time::GetMillisecond() {
	return (*this - AD).GetMillisecond();
}

Time Time::operator +(TimeSpan &difference) {
	Time sum(*this);
	sum.m_TimeData.m_Tick += difference.m_Tick;
	return sum;
}

Time Time::operator -(TimeSpan &difference) {
	Time sum(*this);
	sum.m_TimeData.m_Tick -= difference.m_Tick;
	return sum;
}

bool Time::operator ==(Time &value) {
	return m_TimeData.m_Tick == value.m_TimeData.m_Tick;
}

bool Time::operator !=(Time &value) {
	return m_TimeData.m_Tick != value.m_TimeData.m_Tick;
}

bool Time::operator >(Time &value) {
	return m_TimeData.m_Tick > value.m_TimeData.m_Tick;
}

bool Time::operator >=(Time &value) {
	return m_TimeData.m_Tick >= value.m_TimeData.m_Tick;
}

bool Time::operator <(Time &value) {
	return m_TimeData.m_Tick < value.m_TimeData.m_Tick;
}

bool Time::operator <=(Time &value) {
	return m_TimeData.m_Tick <= value.m_TimeData.m_Tick;
}

constexpr Int64 MaxTicks = Int64(3155378975999999999);
constexpr Int64 MinTicks = Int64(0);
#include "windows_platform.h"

Time GetNowTime() {
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&fileTime, &localFileTime);
	LARGE_INTEGER time;
	time.HighPart = localFileTime.dwHighDateTime;
	time.LowPart = localFileTime.dwLowDateTime;
	return Time(Int64(time.QuadPart));
}

class Calendar {
private:
	int m_Count400Years;
	int m_Count100Years;
	int m_Count4Years;
	int m_Years;
	inline int Prepare(int days) {
		m_Count400Years = days / DaysPer400Years;
		days -= m_Count400Years * DaysPer400Years;
		m_Count100Years = days / DaysPer100Years;
		if (m_Count100Years == 4)
		{
			m_Count100Years = 3;
		}
		days -= m_Count100Years * DaysPer100Years;
		m_Count4Years = days / DaysPer4Years;
		days -= m_Count4Years * DaysPer4Years;
		m_Years = days / DaysPerYear;
		if (m_Years == 4)
		{
			m_Years = 3;
		}
		return days;
	}
	inline int PrepareDays(int days) {
		days = Prepare(days);
		days -= m_Years * DaysPerYear;
		return days;
	}
public:
	int CalculateYear(int days) {
		Prepare(days);
		return m_Count400Years * 400 + m_Count100Years * 100 + m_Count4Years * 4 + m_Years + 1;
	}
	int CalculateDayOfYear(int days) {
		days = PrepareDays(days);
		return days + 1;
	}
	inline bool IsLeapYear() {
		return m_Years == 3 && (m_Count4Years != 24 || m_Count100Years == 3);
	}
	int CalculateMonth(int days) {
		days = PrepareDays(days);
		const int *daysToMonth = IsLeapYear() ? DaysToMonth366 : DaysToMonth365;
		int month = days >> 6;
		while (days >= daysToMonth[month])
		{
			month++;
		}
		return month;
	}
	int CalculateDay(int days) {
		days = PrepareDays(days);
		const int *daysToMonth = IsLeapYear() ? DaysToMonth366 : DaysToMonth365;
		int month = days >> 6;
		while (days >= daysToMonth[month])
		{
			month++;
		}
		return days - daysToMonth[month - 1] + 1;
	}
};

Date::Date(unsigned days) :
    m_Days(days) {
}
int Date::GetDayOfMonth() {
	Calendar calendar;
	return calendar.CalculateMonth(m_Days);
}
DayOfWeek Date::GetDayOfWeek() {
	return DayOfWeek((m_Days + 1) % DaysPerWeek);
}
int Date::GetDayOfYear() {
	Calendar calendar;
	return calendar.CalculateDayOfYear(m_Days);
}
int Date::GetMonth() {
	Calendar calendar;
	return calendar.CalculateMonth(m_Days);
}
int Date::GetYear() {
	Calendar calendar;
	return calendar.CalculateYear(m_Days);
}