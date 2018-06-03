/*
 * ms_timer.h
 *
 *  Created on: Sep 21, 2015
 *      Author: mortenopprudjakobsen
 */

#ifndef MS_TIMER_H_
#define MS_TIMER_H_

void ms_timer_init(void);
void ms_timer_reset(void);
void ms_timer_stop(void);
void ms_timer_start(void);
unsigned int ms_timer_read(void);


#endif /* MS_TIMER_H_ */
