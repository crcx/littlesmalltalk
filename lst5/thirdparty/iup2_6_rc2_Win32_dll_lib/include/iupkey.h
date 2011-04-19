/** \file
 * \brief Keyboard Keys definitions.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPKEY_H 
#define __IUPKEY_H

#define iup_isprint(_c) ((_c) > 31 && (_c) < 127)

#define xCODE(c)        ((c) | 0x0100)
#define isxkey(c)       ((c) & 0x0100)

#define	K_exclam      '!'
#define	K_quotedbl    '\"'
#define	K_numbersign  '#'
#define	K_dollar      '$'
#define	K_percent     '%'
#define	K_ampersand   '&'
#define	K_apostrophe  '\''
#define	K_parentleft  '('
#define	K_parentright ')'
#define	K_asterisk    '*'
#define	K_plus        '+'
#define	K_comma       ','
#define	K_minus       '-'
#define	K_period      '.'
#define	K_slash       '/'
#define	K_0           '0'
#define	K_1           '1'
#define	K_2           '2'
#define	K_3           '3'
#define	K_4           '4'
#define	K_5           '5'
#define	K_6           '6'
#define	K_7           '7'
#define	K_8           '8'
#define	K_9           '9'
#define	K_colon       ':'
#define	K_semicolon   ';'
#define	K_less        '<'
#define	K_equal       '='
#define	K_greater     '>'
#define	K_question    '?'
#define	K_at          '@'
#define	K_A           'A'
#define	K_B           'B'
#define	K_C           'C'
#define	K_D           'D'
#define	K_E           'E'
#define	K_F           'F'
#define	K_G           'G'
#define	K_H           'H'
#define	K_I           'I'
#define	K_J           'J'
#define	K_K           'K'
#define	K_L           'L'
#define	K_M           'M'
#define	K_N           'N'
#define	K_O           'O'
#define	K_P           'P'
#define	K_Q           'Q'
#define	K_R           'R'
#define	K_S           'S'
#define	K_T           'T'
#define	K_U           'U'
#define	K_V           'V'
#define	K_W           'W'
#define	K_X           'X'
#define	K_Y           'Y'
#define	K_Z           'Z'
#define	K_bracketleft '['
#define	K_backslash   '\\'
#define	K_bracketright ']'
#define	K_circum      '^'
#define	K_underscore  '_'
#define	K_grave       '`'
#define	K_a           'a'
#define	K_b           'b'
#define	K_c           'c'
#define	K_d           'd'
#define	K_e           'e'
#define	K_f           'f'
#define	K_g           'g'
#define	K_h           'h'
#define	K_i           'i'
#define	K_j           'j'
#define	K_k           'k'
#define	K_l           'l'
#define	K_m           'm'
#define	K_n           'n'
#define	K_o           'o'
#define	K_p           'p'
#define	K_q           'q'
#define	K_r           'r'
#define	K_s           's'
#define	K_t           't'
#define	K_u           'u'
#define	K_v           'v'
#define	K_w           'w'
#define	K_x           'x'
#define	K_y           'y'
#define	K_z           'z'
#define	K_braceleft   '{'
#define	K_bar         '|'
#define	K_braceright  '}'
#define	K_tilde       '~'

#define	K_quoteleft   K_grave
#define	K_quoteright  K_apostrophe

#define K_cA     1
#define K_cB     2
#define K_cC     3
#define K_cD     4
#define K_cE     5
#define K_cF     6
#define K_cG     7
#define K_cH     8
#define K_cI     9
#define K_cJ     10
#define K_cK     11
#define K_cL     12
#define K_cM     13
#define K_cN     14
#define K_cO     15
#define K_cP     16
#define K_cQ     17
#define K_cR     18
#define K_cS     19
#define K_cT     20
#define K_cU     21
#define K_cV     22
#define K_cW     23
#define K_cX     24
#define K_cY     25
#define K_cZ     26

#define K_mA     xCODE(30)
#define K_mB     xCODE(48)
#define K_mC     xCODE(46)
#define K_mD     xCODE(32)
#define K_mE     xCODE(18)
#define K_mF     xCODE(33)
#define K_mG     xCODE(34)
#define K_mH     xCODE(35)
#define K_mI     xCODE(23)
#define K_mJ     xCODE(36)
#define K_mK     xCODE(37)
#define K_mL     xCODE(38)
#define K_mM     xCODE(50)
#define K_mN     xCODE(49)
#define K_mO     xCODE(24)
#define K_mP     xCODE(25)
#define K_mQ     xCODE(16)
#define K_mR     xCODE(19)
#define K_mS     xCODE(31)
#define K_mT     xCODE(20)
#define K_mU     xCODE(22)
#define K_mV     xCODE(47)
#define K_mW     xCODE(17)
#define K_mX     xCODE(45)
#define K_mY     xCODE(21)
#define K_mZ     xCODE(44)

#define K_BS     '\b'
#define K_CR     '\r'
#define K_sCR    xCODE(14)
#define K_ESC    27
#define K_SP     ' '

#define K_TAB    '\t'
#define K_sTAB   xCODE(15)
#define K_cTAB   xCODE(148)
#define K_mTAB   xCODE(165)

#define K_PAUSE  xCODE(70)
#define K_HOME   xCODE(71)
#define K_UP     xCODE(72)
#define K_PGUP   xCODE(73)
#define K_LEFT   xCODE(75)
#define K_MIDDLE xCODE(76)
#define K_RIGHT  xCODE(77)
#define K_END    xCODE(79)
#define K_DOWN   xCODE(80)
#define K_PGDN   xCODE(81)
#define K_INS    xCODE(82)
#define K_DEL    xCODE(83)

#define K_sHOME  xCODE(200)
#define K_sUP    xCODE(201)
#define K_sPGUP  xCODE(202)
#define K_sLEFT  xCODE(203)
#define K_sRIGHT xCODE(204)
#define K_sEND   xCODE(205)
#define K_sDOWN  xCODE(206)
#define K_sPGDN  xCODE(207)
#define K_sSP    xCODE(208)
#define K_sINS   xCODE(210)
#define K_sDEL   xCODE(211)

#define K_cHOME  xCODE(119)
#define K_cPGUP  xCODE(132)
#define K_cLEFT  xCODE(115)
#define K_cRIGHT xCODE(116)
#define K_cEND   xCODE(117)
#define K_cPGDN  xCODE(118)
#define K_cUP    xCODE(141)
#define K_cMIDDLE xCODE(143)
#define K_cDOWN  xCODE(145)
#define K_cINS   xCODE(146)
#define K_cDEL   xCODE(147)
#define K_cSP    xCODE(209)

#define K_mHOME  xCODE(151)
#define K_mPGUP  xCODE(153)
#define K_mLEFT  xCODE(155)
#define K_mRIGHT xCODE(157)
#define K_mEND   xCODE(159)
#define K_mPGDN  xCODE(161)
#define K_mUP    xCODE(152)
#define K_mDOWN  xCODE(160)
#define K_mINS   xCODE(162)
#define K_mDEL   xCODE(163)

#define K_F1     xCODE(59)
#define K_F2     xCODE(60)
#define K_F3     xCODE(61)
#define K_F4     xCODE(62)
#define K_F5     xCODE(63)
#define K_F6     xCODE(64)
#define K_F7     xCODE(65)
#define K_F8     xCODE(66)
#define K_F9     xCODE(67)
#define K_F10    xCODE(68)
#define K_F11    xCODE(133)
#define K_F12    xCODE(134)

#define K_sF1    xCODE(84)
#define K_sF2    xCODE(85)
#define K_sF3    xCODE(86)
#define K_sF4    xCODE(87)
#define K_sF5    xCODE(88)
#define K_sF6    xCODE(89)
#define K_sF7    xCODE(90)
#define K_sF8    xCODE(91)
#define K_sF9    xCODE(92)
#define K_sF10   xCODE(93)
#define K_sF11   xCODE(135)
#define K_sF12   xCODE(136)

#define K_cF1    xCODE(94)
#define K_cF2    xCODE(95)
#define K_cF3    xCODE(96)
#define K_cF4    xCODE(97)
#define K_cF5    xCODE(98)
#define K_cF6    xCODE(99)
#define K_cF7    xCODE(100)
#define K_cF8    xCODE(101)
#define K_cF9    xCODE(102)
#define K_cF10   xCODE(103)
#define K_cF11   xCODE(137)
#define K_cF12   xCODE(138)

#define K_mF1    xCODE(104)
#define K_mF2    xCODE(105)
#define K_mF3    xCODE(106)
#define K_mF4    xCODE(107)
#define K_mF5    xCODE(108)
#define K_mF6    xCODE(109)
#define K_mF7    xCODE(110)
#define K_mF8    xCODE(111)
#define K_mF9    xCODE(112)
#define K_mF10   xCODE(113)
#define K_mF11   xCODE(139)
#define K_mF12   xCODE(140)

#define K_m1     xCODE(120)
#define K_m2     xCODE(121)
#define K_m3     xCODE(122)
#define K_m4     xCODE(123)
#define K_m5     xCODE(124)
#define K_m6     xCODE(125)
#define K_m7     xCODE(126)
#define K_m8     xCODE(127)
#define K_m9     xCODE(128)
#define K_m0     xCODE(129)

#endif
