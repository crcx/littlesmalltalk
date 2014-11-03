METHODS FOR File
fileIn [
  | parser |
  parser := GSTParser newWith: self.
  ^parser parse.
]
!
