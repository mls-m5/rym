/*
 * common.h
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

export module common;

export constexpr double pi = 3.1415926535897932384626433832795028841971693;
export constexpr double pi2 = pi * 2;
export constexpr float pif = static_cast<float>(pi);
export constexpr float pi2f = static_cast<float>(pi2);

export constexpr bool UseRoamingBroadphase = true;
