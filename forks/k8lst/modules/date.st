"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [
  DateTime
]

Object subclass: Date | secondsInDay monthNames firstDayOfMonth daysInMonth weekDayNames | [
| date time |
^initialize [
  "Initialize class variables with common date stuff"
  daysInMonth := #(31 28 31 30 31 30 31 31 30 31 30 31).
  firstDayOfMonth := #(1 32 60 91 121 152 182 213 244 274 305 335).
  monthNames := #(January February March April May June July August September October November December).
  secondsInDay := 24 * 60 * 60.
  weekDayNames := #(Sunday Monday Tuesday Wednesday Thursday Friday Saturday)
]

^daysInMonth [
  ^daysInMonth
]

^firstDayOfMonth [
  ^firstDayOfMonth
]

^monthNames [
  ^monthNames
]

^secondsInDay [
  ^secondsInDay
]

^weekDayNames [
  ^weekDayNames
]

^dayOfWeek: dayName [
  | ds |
  ds := dayName asSymbol.
  (1 to: 7) do: [:x | ((weekDayNames at: x) == ds) ifTrue: [ ^x ]].
  ^nil
]

^isLeapYear: aYear [
  "return true if aYear is a leap year"
  (aYear / 4 = 0) ifTrue: [
    (aYear / 100 ~= 0) ifTrue: [ ^true ].
    (aYear / 400 = 0) ifTrue: [ ^true ]
  ].
  ^false
]

^new [
  | obj |
  obj := super new.
  obj unixTime: System unixTime.
  ^obj
]

^newWithUnixTime: ut [
  | obj |
  obj := super new.
  obj unixTime: ut.
  ^obj
]

^newFrom: aDate [
  | obj |
  obj := super new.
  obj unixTime: aDate unixTime.
  ^obj
]

^now [
  "assign the current date and time"
  ^self new
]

^today [
  ^self now
]

^localTimeFor: aDateTime [
  ^(System localTimeFor: aDateTime unixTime)
]


localTime [
  ^(System localTimeFor: (self unixTime))
]

unixTime [
  ^(date * 86400) + time
]

unixTime: ut [
  date := ut / 86400.
  time := ut % 86400.
]

= aDate [
  (self class = aDate class) ifFalse: [ ^false ].
  ^self unixTime = aDate unixTime
]

date [
  ^date
]

date: aDate [
  date := aDate
]

time [
  ^time
]

time: aTime [
  time := aTime
]

day [
  ^(self localTime) at: 4
]

dayOfWeek [
  ^(self localTime) at: 7
]

dayOfWeekName [
  ^(Date weekDayNames) at: (self dayOfWeek + 1)
]

dayOfYear [
  ^(self localTime) at: 8
]

hours [
  ^(self localTime) at: 3
]

isLeapYear [
  ^Date isLeapYear: (self year)
]

minutes [
  ^(self localTime) at: 2
]

month [
  ^(self localTime) at: 5
]

monthName [
  ^(Date monthNames) at: (self month)
]

seconds [
  ^(self localTime) at: 1
]

year [
  ^(self localTime) at: 6
]

toNow [
  "assign the current date and time to a Date instance"
  self unixTime: System unixTime.
]

next: dayName [
  "Return the next date whose weekday name is dayName"
  | diff |
  diff := (self class dayOfWeek: dayName) - self dayOfWeek.
  diff = 0
    ifTrue: [ diff := 7 ]
    ifFalse: [ (diff < 0) ifTrue: [ diff := 7 + diff ]].
  ^self addDays: diff
]

previous: dayName [
  "Return the previous date whose weekday name is dayName"
  ^self subtractDays:(self dayOfWeek - (self class dayOfWeek: dayName) - 1 / 7)
]

addDays: days [
  | nDate |
  nDate := self class newFrom: self.
  ^nDate date: date + days.
]

subtractDays: days [
  | nDate |
  nDate := self class newFrom: self.
  ^nDate date: date - days.
]

printString [
  ^(self year asString) + '/' +
  (self month printWidth: 2) + '/' +
  (self day printWidth: 2) + ' ' +
  (self hours printWidth: 2) + ':' +
  (self minutes printWidth: 2) + ':' +
  (self seconds printWidth: 2)
]
]


{ Date initialize }
