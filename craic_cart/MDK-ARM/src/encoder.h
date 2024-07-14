#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f4xx_hal.h"

extern int16_t FL_encoder,FR_encoder,RL_encoder,RR_encoder;
extern int32_t distance;
//extern int32_t lastdistance;

void encoder_init(void);
//short get_encoder(void);

#endif
