/**
 * \file
 *
 * \brief Display demo process control application
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef APP_TANK_H
#define APP_TANK_H

#include <workqueue.h>

/**
 * \ingroup apps_group
 * \defgroup apps_tank_group Water tank application
 *
 * \brief The water tank application is a process control-inspired demonstration
 * of the slider, progress bar and command button widgets in the \ref gfx_wtk
 * "Widget toolkit"; bitmaps of the \ref gfx "Graphics display system";
 * and basic use of the \ref gfx_win "Window toolkit".
 *
 * The application consists of a slider for controlling the supply (flow in) of
 * water to a tank, and two progress bars for indicating the level of the water
 * tank and the time-varying demand (flow out) for water, respectively.
 * An alarm light changes color from green to red whenever the tank overflows,
 * and the indicator for demand changes color if the demand exceeds the supply
 * and available water in the tank.
 *
 * This application makes use of the \ref timer_group "Timer driver" for
 * maintaining a constant update rate regardless of the workqueue load.
 * The \ref appsutil_fileloader_group "Fileloader utility" is used to load the
 * alarm light bitmaps to hugemem, allowing for synchronous drawing of them.
 *
 * \par Simulated process
 *
 * The process in this application was created simply to get a time-varying
 * system, and is not meant to represent a real-world one. This section explains
 * how the process is computed.
 *
 * At iteration \a n, the process parameters are:
 * - \a r(n) : random value
 * - \a d(n) : demand
 * - \a l(n) : level of tank
 * - \a f(n) : total flow of the system
 * - \a s(n) : supply
 *
 * First, the random value is computed by use of the logistic map with scaling:
 * \verbatim r(n) = q * (r(n-1) * (k - r(n-1))) / k \endverbatim
 * If \a r(0) is in the range [1 , \a k - 1], this function will return a value
 * in the same range. The factor \a q affects how chaotic the logistic function
 * behaves: For values close to the upper bound of 4, the function will seem
 * quite random. In the implementation, \a q has been set to 3.95 and is
 * represented by the ratio 79 / 20 since integer multiplications must be used.
 * The maximum value \a k is set to the same as the maximum level of the tank:
 * 127.
 *
 * The demand is then computed from a weighted mean:
 * \verbatim d(n) = (2 * d(n-1) + l(n-1) + r(n)) / 4 \endverbatim
 * Disregarding the random component, this will essentially give a low-pass
 * filtered response to changes in demand and tank level.
 *
 * The flow of the system is then computed as:
 * \verbatim f(n) = (s(n) - d(n)) / t \endverbatim
 * The factor \a t can be used to "time-stretch" the simulated process, and has
 * been set to 4 in the implementation. The maximum value of supply is the same
 * as the maximum level of the tank: 127.
 *
 * Finally, the level of the tank is then computed as:
 * \verbatim l(n) = l(n-1) + f(n) \endverbatim
 * However, the level of the tank is bounded by 0 and its maximum value.
 * If the tank's level reaches 0, the demand indicator will change color to
 * indicate that the supply is insufficient. If it reaches the maximum, the
 * overflow alarm light will change color to indicate that the supply is too
 * great. As stated above, the maximum level of the tank is set to 127.
 */

void app_tank_launch(struct workqueue_task *task);

#endif /* APP_TANK_H */
