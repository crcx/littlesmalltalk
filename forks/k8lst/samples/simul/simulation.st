class: Simulation [
  | currentTime nextEvent nextEventTime |

  ^new [
    | obj |
    obj := self basicNew.
    self in: obj var: #currentTime put: 0.
    ^obj
  ]

  time [
    ^currentTime
  ]

  addEvent: event at: eventTime [
    nextEvent := event.
    nextEventTime := eventTime.
  ]

  proceed [
    currentTime := nextEventTime.
    ^self processEvent: nextEvent
  ]
]
