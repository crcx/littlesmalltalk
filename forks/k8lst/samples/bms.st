"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"

"Unholy lexicon and all invoked titles (c) Rob Waldie, June 98"
Package [ BlackMetal ]


class: BMTitle | words | [
^initialize [
  words := #(
  #('And' 'Of' 'Under' 'In' 'Through' 'Between' 'Beneath' 'Beyond'
    'Towards' 'Into' 'From' 'Conquering' 'Praise' 'Praising' 'Storming'
    'Hailing' 'Beholding' 'Worshipping' 'Bleeding For' 'Dwelling In'
    'Rising From' 'Descending Into' 'War For' 'Battling For'
    'Gazing Over' 'Hearing' 'Blaspheming' 'Crossing' 'Invoking'
    'Traversing' 'Forever Reigns' 'Conquer' 'Storm' 'Hail' 'Behold'
    'Invoke' 'Over')
  #('a' 'The')
  #('Black' 'Hateful' 'Evil' 'Melancholic' 'Satanic' 'Eternal'
    'Frostbitten' 'Freezing' 'Funeral' 'Blood Red' 'Nocturnal' 'Winter'
    'Satanic' 'Unholy' 'Grim' 'Frozen' 'Spectral' 'Dead' 'Grey'
    'Bleakest' 'Burning' 'Silent' 'Still' 'Dark' 'Pagan' 'Vast'
    'Mighty' 'Infernal' 'Majestic' 'Blasphemic' 'Heathen' 'Everlasting'
    'Ancient' 'Northern' 'Antichristian' 'Blazing' 'Crucified')
  #('Wood' 'Forest' 'Mountain' 'Night' 'Realm' 'Kingdom' 'Dominion'
    'Northland' 'North' 'Hell' 'Underworld' 'Infinity' 'Eternity'
    'Chasm' 'Sky' 'Blood' 'Wrath' 'Power' 'Might' 'Glory' 'Forevermore'
    'Despair' 'Shadows' 'Throne' 'Fullmoon' 'Cosmos' 'Death' 'Darkness'
    'Frost' 'Horizon' 'Fog' 'Mist' 'Hordes' 'Legion' 'Passage'
    'Blasphemy' 'Fall Of Heaven' 'Death Of Christ' 'Twilight' 'Furnace'
    'Chalice' 'Sword' 'Dagger' 'Coffin' 'Pentagram' 'Cloak' 'Chant'
    'Scream' 'Cry' 'Wolf' 'Raven' 'Winter' 'Demon' 'Storm' 'Whore'
    'Goat' 'Dawn' 'Crucifixion' 'Oath' 'Pact')
  #('Beyond' 'Towards' 'Into' 'From' 'And' 'Of' 'Under' 'In' 'Through'
    'Between' 'Beneath' 'With' 'Surrounded By' 'Bewitched By'
    'For The Glory Of' 'Over' 'In The Name Of')
  #('Black' 'Hateful' 'Evil' 'Melancholic' 'Satanic' 'Eternal'
    'Frostbitten' 'Freezing' 'Funeral' 'Blood Red' 'Nocturnal' 'Winter'
    'Satanic' 'Unholy' 'Grim' 'Frozen' 'Spectral' 'Dead' 'Grey'
    'Bleakest' 'Burning' 'Silent' 'Still' 'Dark' 'Pagan' 'Vast'
    'Mighty' 'Infernal' 'Majestic' 'Blasphemic' 'Heathen' 'Everlasting'
    'Ancient' 'Northern' 'Antichristian' 'Blazing' 'Crucified')
  #('Woods' 'Forests' 'Mountains' 'Night' 'Realms' 'Kingdoms'
    'Dominions' 'Northlands' 'North' 'Hell' 'Underworlds' 'Infinity'
    'Eternity' 'Chasms' 'Skies' 'Wolves' 'Wrath' 'Power' 'Might'
    'Glory' 'Forevermore' 'Despair' 'Shadows' 'Thrones' 'Necromancy'
    'Cosmos' 'Death' 'Darkness' 'Frosts' 'Horizons' 'Fog' 'Mist'
    'Hordes' 'Legions' 'Passages' 'Blasphemy' 'Lust' 'Sin' 'Twilight'
    'Sodomy' 'Chalices' 'Swords' 'Daggers' 'Coffins' 'Pentagrams'
    'Flagellation' 'Chants' 'Screams' 'Cries' 'Winter' 'Demons'
    'Witchery' 'Spellcraft' 'Hellfire' 'Supremecy' 'Desolation' 'Light'
    'Storms' 'Winds' 'Horns' 'Damnation' 'Evil' 'Immortality' 'Inferno'
    'Spears'))
]


^title [
  | res |
  words ifNil: [ self initialize ].
  res := ''.
  words do: [:i |
    res := res + (i at: (SmallInt atRandom % i size) + 1) + ' '.
  ].
  ^res removeTrailingBlanks
]

^new [
  ^self title
]
]


{ BMTitle new printNl. }
