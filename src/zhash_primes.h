/*  =========================================================================
    zhash_primes.h - 5 largest primes less than 2^n for n = 4...63

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZHASH_PRIMES_H_INCLUDED__
#define __ZHASH_PRIMES_H_INCLUDED__

#ifdef _MSC_VER
#	define PORTABLE_LLU(number) number##ULL
#else
#	define PORTABLE_LLU(number) number##LLU
#endif

static size_t primes [] = {
    PORTABLE_LLU(3),
    PORTABLE_LLU(5),
    PORTABLE_LLU(7),
    PORTABLE_LLU(11),
    PORTABLE_LLU(13), // 2^4
    PORTABLE_LLU(17),
    PORTABLE_LLU(19),
    PORTABLE_LLU(23),
    PORTABLE_LLU(29),
    PORTABLE_LLU(31), // 2^5
    PORTABLE_LLU(43),
    PORTABLE_LLU(47),
    PORTABLE_LLU(53),
    PORTABLE_LLU(59),
    PORTABLE_LLU(61), // 2^6
    PORTABLE_LLU(103),
    PORTABLE_LLU(107),
    PORTABLE_LLU(109),
    PORTABLE_LLU(113),
    PORTABLE_LLU(127), // 2^7
    PORTABLE_LLU(229),
    PORTABLE_LLU(233),
    PORTABLE_LLU(239),
    PORTABLE_LLU(241),
    PORTABLE_LLU(251), // 2^8
    PORTABLE_LLU(487),
    PORTABLE_LLU(491),
    PORTABLE_LLU(499),
    PORTABLE_LLU(503),
    PORTABLE_LLU(509), // 2^9
    PORTABLE_LLU(997),
    PORTABLE_LLU(1009),
    PORTABLE_LLU(1013),
    PORTABLE_LLU(1019),
    PORTABLE_LLU(1021), // 2^10
    PORTABLE_LLU(2011),
    PORTABLE_LLU(2017),
    PORTABLE_LLU(2027),
    PORTABLE_LLU(2029),
    PORTABLE_LLU(2039), // 2^11
    PORTABLE_LLU(4057),
    PORTABLE_LLU(4073),
    PORTABLE_LLU(4079),
    PORTABLE_LLU(4091),
    PORTABLE_LLU(4093), // 2^12
    PORTABLE_LLU(8161),
    PORTABLE_LLU(8167),
    PORTABLE_LLU(8171),
    PORTABLE_LLU(8179),
    PORTABLE_LLU(8191), // 2^13
    PORTABLE_LLU(16349),
    PORTABLE_LLU(16361),
    PORTABLE_LLU(16363),
    PORTABLE_LLU(16369),
    PORTABLE_LLU(16381), // 2^14
    PORTABLE_LLU(32707),
    PORTABLE_LLU(32713),
    PORTABLE_LLU(32717),
    PORTABLE_LLU(32719),
    PORTABLE_LLU(32749), // 2^15
    PORTABLE_LLU(65449),
    PORTABLE_LLU(65479),
    PORTABLE_LLU(65497),
    PORTABLE_LLU(65519),
    PORTABLE_LLU(65521), // 2^16
    PORTABLE_LLU(131023),
    PORTABLE_LLU(131041),
    PORTABLE_LLU(131059),
    PORTABLE_LLU(131063),
    PORTABLE_LLU(131071), // 2^17
    PORTABLE_LLU(262111),
    PORTABLE_LLU(262121),
    PORTABLE_LLU(262127),
    PORTABLE_LLU(262133),
    PORTABLE_LLU(262139), // 2^18
    PORTABLE_LLU(524243),
    PORTABLE_LLU(524257),
    PORTABLE_LLU(524261),
    PORTABLE_LLU(524269),
    PORTABLE_LLU(524287), // 2^19
    PORTABLE_LLU(1048517),
    PORTABLE_LLU(1048549),
    PORTABLE_LLU(1048559),
    PORTABLE_LLU(1048571),
    PORTABLE_LLU(1048573), // 2^20
    PORTABLE_LLU(2097091),
    PORTABLE_LLU(2097097),
    PORTABLE_LLU(2097131),
    PORTABLE_LLU(2097133),
    PORTABLE_LLU(2097143), // 2^21
    PORTABLE_LLU(4194247),
    PORTABLE_LLU(4194271),
    PORTABLE_LLU(4194277),
    PORTABLE_LLU(4194287),
    PORTABLE_LLU(4194301), // 2^22
    PORTABLE_LLU(8388547),
    PORTABLE_LLU(8388571),
    PORTABLE_LLU(8388581),
    PORTABLE_LLU(8388587),
    PORTABLE_LLU(8388593), // 2^23
    PORTABLE_LLU(16777141),
    PORTABLE_LLU(16777153),
    PORTABLE_LLU(16777183),
    PORTABLE_LLU(16777199),
    PORTABLE_LLU(16777213), // 2^24
    PORTABLE_LLU(33554341),
    PORTABLE_LLU(33554347),
    PORTABLE_LLU(33554371),
    PORTABLE_LLU(33554383),
    PORTABLE_LLU(33554393), // 2^25
    PORTABLE_LLU(67108763),
    PORTABLE_LLU(67108777),
    PORTABLE_LLU(67108819),
    PORTABLE_LLU(67108837),
    PORTABLE_LLU(67108859), // 2^26
    PORTABLE_LLU(134217593),
    PORTABLE_LLU(134217613),
    PORTABLE_LLU(134217617),
    PORTABLE_LLU(134217649),
    PORTABLE_LLU(134217689), // 2^27
    PORTABLE_LLU(268435331),
    PORTABLE_LLU(268435337),
    PORTABLE_LLU(268435361),
    PORTABLE_LLU(268435367),
    PORTABLE_LLU(268435399), // 2^28
    PORTABLE_LLU(536870839),
    PORTABLE_LLU(536870849),
    PORTABLE_LLU(536870869),
    PORTABLE_LLU(536870879),
    PORTABLE_LLU(536870909), // 2^29
    PORTABLE_LLU(1073741719),
    PORTABLE_LLU(1073741723),
    PORTABLE_LLU(1073741741),
    PORTABLE_LLU(1073741783),
    PORTABLE_LLU(1073741789), // 2^30
    PORTABLE_LLU(2147483563),
    PORTABLE_LLU(2147483579),
    PORTABLE_LLU(2147483587),
    PORTABLE_LLU(2147483629),
    PORTABLE_LLU(2147483647), // 2^31
    PORTABLE_LLU(4294967197),
    PORTABLE_LLU(4294967231),
    PORTABLE_LLU(4294967279),
    PORTABLE_LLU(4294967291),
    PORTABLE_LLU(4294967295), // 2^32
#if __WORDSIZE == 64
    PORTABLE_LLU(8589934581),
    PORTABLE_LLU(8589934585),
    PORTABLE_LLU(8589934587),
    PORTABLE_LLU(8589934589),
    PORTABLE_LLU(8589934591), // 2^33
    PORTABLE_LLU(17179869175),
    PORTABLE_LLU(17179869177),
    PORTABLE_LLU(17179869179),
    PORTABLE_LLU(17179869181),
    PORTABLE_LLU(17179869183), // 2^34
    PORTABLE_LLU(34359738359),
    PORTABLE_LLU(34359738361),
    PORTABLE_LLU(34359738363),
    PORTABLE_LLU(34359738365),
    PORTABLE_LLU(34359738367), // 2^35
    PORTABLE_LLU(68719476725),
    PORTABLE_LLU(68719476727),
    PORTABLE_LLU(68719476729),
    PORTABLE_LLU(68719476733),
    PORTABLE_LLU(68719476735), // 2^36
    PORTABLE_LLU(137438953463),
    PORTABLE_LLU(137438953465),
    PORTABLE_LLU(137438953467),
    PORTABLE_LLU(137438953469),
    PORTABLE_LLU(137438953471), // 2^37
    PORTABLE_LLU(274877906935),
    PORTABLE_LLU(274877906937),
    PORTABLE_LLU(274877906939),
    PORTABLE_LLU(274877906941),
    PORTABLE_LLU(274877906943), // 2^38
    PORTABLE_LLU(549755813877),
    PORTABLE_LLU(549755813879),
    PORTABLE_LLU(549755813883),
    PORTABLE_LLU(549755813885),
    PORTABLE_LLU(549755813887), // 2^39
    PORTABLE_LLU(1099511627767),
    PORTABLE_LLU(1099511627769),
    PORTABLE_LLU(1099511627771),
    PORTABLE_LLU(1099511627773),
    PORTABLE_LLU(1099511627775), // 2^40
    PORTABLE_LLU(2199023255543),
    PORTABLE_LLU(2199023255545),
    PORTABLE_LLU(2199023255547),
    PORTABLE_LLU(2199023255549),
    PORTABLE_LLU(2199023255551), // 2^41
    PORTABLE_LLU(4398046511095),
    PORTABLE_LLU(4398046511097),
    PORTABLE_LLU(4398046511099),
    PORTABLE_LLU(4398046511101),
    PORTABLE_LLU(4398046511103), // 2^42
    PORTABLE_LLU(8796093022199),
    PORTABLE_LLU(8796093022201),
    PORTABLE_LLU(8796093022203),
    PORTABLE_LLU(8796093022205),
    PORTABLE_LLU(8796093022207), // 2^43
    PORTABLE_LLU(17592186044407),
    PORTABLE_LLU(17592186044409),
    PORTABLE_LLU(17592186044411),
    PORTABLE_LLU(17592186044413),
    PORTABLE_LLU(17592186044415), // 2^44
    PORTABLE_LLU(35184372088823),
    PORTABLE_LLU(35184372088825),
    PORTABLE_LLU(35184372088827),
    PORTABLE_LLU(35184372088829),
    PORTABLE_LLU(35184372088831), // 2^45
    PORTABLE_LLU(70368744177655),
    PORTABLE_LLU(70368744177657),
    PORTABLE_LLU(70368744177659),
    PORTABLE_LLU(70368744177661),
    PORTABLE_LLU(70368744177663), // 2^46
    PORTABLE_LLU(140737488355319),
    PORTABLE_LLU(140737488355321),
    PORTABLE_LLU(140737488355323),
    PORTABLE_LLU(140737488355325),
    PORTABLE_LLU(140737488355327), // 2^47
    PORTABLE_LLU(281474976710647),
    PORTABLE_LLU(281474976710649),
    PORTABLE_LLU(281474976710651),
    PORTABLE_LLU(281474976710653),
    PORTABLE_LLU(281474976710655), // 2^48
    PORTABLE_LLU(562949953421303),
    PORTABLE_LLU(562949953421305),
    PORTABLE_LLU(562949953421307),
    PORTABLE_LLU(562949953421309),
    PORTABLE_LLU(562949953421311), // 2^49
    PORTABLE_LLU(1125899906842615),
    PORTABLE_LLU(1125899906842617),
    PORTABLE_LLU(1125899906842619),
    PORTABLE_LLU(1125899906842621),
    PORTABLE_LLU(1125899906842623), // 2^50
    PORTABLE_LLU(2251799813685239),
    PORTABLE_LLU(2251799813685241),
    PORTABLE_LLU(2251799813685243),
    PORTABLE_LLU(2251799813685245),
    PORTABLE_LLU(2251799813685247), // 2^51
    PORTABLE_LLU(4503599627370487),
    PORTABLE_LLU(4503599627370489),
    PORTABLE_LLU(4503599627370491),
    PORTABLE_LLU(4503599627370493),
    PORTABLE_LLU(4503599627370495), // 2^52
    PORTABLE_LLU(9007199254740983),
    PORTABLE_LLU(9007199254740985),
    PORTABLE_LLU(9007199254740987),
    PORTABLE_LLU(9007199254740989),
    PORTABLE_LLU(9007199254740991), // 2^53
    PORTABLE_LLU(18014398509481975),
    PORTABLE_LLU(18014398509481977),
    PORTABLE_LLU(18014398509481979),
    PORTABLE_LLU(18014398509481981),
    PORTABLE_LLU(18014398509481983), // 2^54
    PORTABLE_LLU(36028797018963959),
    PORTABLE_LLU(36028797018963961),
    PORTABLE_LLU(36028797018963963),
    PORTABLE_LLU(36028797018963965),
    PORTABLE_LLU(36028797018963967), // 2^55
    PORTABLE_LLU(72057594037927925),
    PORTABLE_LLU(72057594037927927),
    PORTABLE_LLU(72057594037927929),
    PORTABLE_LLU(72057594037927933),
    PORTABLE_LLU(72057594037927935), // 2^56
    PORTABLE_LLU(144115188075855863),
    PORTABLE_LLU(144115188075855865),
    PORTABLE_LLU(144115188075855867),
    PORTABLE_LLU(144115188075855869),
    PORTABLE_LLU(144115188075855871), // 2^57
    PORTABLE_LLU(288230376151711735),
    PORTABLE_LLU(288230376151711737),
    PORTABLE_LLU(288230376151711739),
    PORTABLE_LLU(288230376151711741),
    PORTABLE_LLU(288230376151711743), // 2^58
    PORTABLE_LLU(576460752303423479),
    PORTABLE_LLU(576460752303423481),
    PORTABLE_LLU(576460752303423483),
    PORTABLE_LLU(576460752303423485),
    PORTABLE_LLU(576460752303423487), // 2^59
    PORTABLE_LLU(1152921504606846967),
    PORTABLE_LLU(1152921504606846969),
    PORTABLE_LLU(1152921504606846971),
    PORTABLE_LLU(1152921504606846973),
    PORTABLE_LLU(1152921504606846975), // 2^60
    PORTABLE_LLU(2305843009213693941),
    PORTABLE_LLU(2305843009213693943),
    PORTABLE_LLU(2305843009213693945),
    PORTABLE_LLU(2305843009213693947),
    PORTABLE_LLU(2305843009213693949), // 2^61
    PORTABLE_LLU(4611686018427387895),
    PORTABLE_LLU(4611686018427387897),
    PORTABLE_LLU(4611686018427387899),
    PORTABLE_LLU(4611686018427387901),
    PORTABLE_LLU(4611686018427387903), // 2^62
    PORTABLE_LLU(9223372036854775799),
    PORTABLE_LLU(9223372036854775801),
    PORTABLE_LLU(9223372036854775803),
    PORTABLE_LLU(9223372036854775805),
    PORTABLE_LLU(9223372036854775807) // 2^63
#endif
};

#endif