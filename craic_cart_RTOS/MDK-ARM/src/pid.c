#include "pid.h"

void Pos_pid(pid *pid,int16_t target,int16_t real)
{
	pid->error = target-real;
	pid->output = (int16_t)(pid->p*pid->error + pid->d*(pid->error-pid->last_error));
	pid->output += pid->i*pid->error;
	pid->last_error = pid->error;
}

void Incre_pid(pid *pid,int16_t target,int16_t real)
{
	pid->error = target-real;
	pid->output += (int16_t)(pid->p*(pid->error-pid->last_error) + pid->i*(pid->error) + pid->d*(pid->error-2*pid->last_error+pid->last_last_error));
	pid->last_last_error = pid->last_error;
	pid->last_error = pid->error;
}
