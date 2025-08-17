/*
 * control.c
 *
 *  Created on: Jun 20, 2025
 *      Author: aravs
 */

#include "control.h"

#include <math.h>

void PID_init(PID* pid) {
	pid->pt = HAL_GetTick();
	pid->pe = 0.0f;
	pid->i = 0.0f;
	pid->d = 0.0f;
	pid->cfe = 0.0f;
	pid->pfe = 0.0f;
	pid->output = 0.0f;
}

void PID_compute(PID* pid, float target, float current) {
	uint32_t t = HAL_GetTick();

	float dt = ((float)(t - pid->pt)) / 1.0e3f;
	pid->pt = t;

	if (dt <= 0.0f) return;

	float error = target - current;

	pid->cfe = (pid->Ka * pid->pfe) + (1 - pid->Ka) * (error - pid->pe);
	pid->pfe = pid->cfe;

	pid->pe = error;

	pid->p = error;
	pid->i += error * dt;

	if (pid->Ki != 0.0f) {
	    float i_max = pid->OUT_MAX / pid->Ki;
	    float i_min = pid->OUT_MIN / pid->Ki;

		if (pid->i > i_max) pid->i = i_max;
	    if (pid->i < i_min) pid->i = i_min;
	}

	pid->d = pid->cfe / dt;

	float signal = pid->Kp * pid->p + pid->Ki * pid->i + pid->Kd * pid->d;

	if (signal > pid->OUT_MAX) signal = pid->OUT_MAX;
	if (signal < pid->OUT_MIN) signal = pid->OUT_MIN;

	pid->output = signal;
}

void update_EMA_filter(float sample, float* mean, float* var, float alpha, float min_var) {
	*mean = alpha * sample + (1.0f - alpha) * (*mean);

	float dev = sample - *mean;

	*var = alpha * (dev * dev) + (1.0f - alpha) * (*var);
	*var = fmaxf(*var, min_var);
}

void Kalman_2D_update_measurement_variance(Kalman_2D* kalman, float sample, float alpha, float min_var) {
	update_EMA_filter(sample, &kalman->measurement_mean, &kalman->measurement_var, alpha, min_var);

	kalman->R = kalman->measurement_var;
}

void Kalman_2D_update_process_variance(Kalman_2D* kalman, float process1_input, float process2_input, float alpha, float min_var) {
	update_EMA_filter(process1_input, &kalman->process1_mean, &kalman->process1_var, alpha, min_var);
	update_EMA_filter(process2_input, &kalman->process2_mean, &kalman->process2_var, alpha, min_var);

	kalman->Q[0][0] = kalman->process1_var;
	kalman->Q[1][1] = kalman->process2_var;
}

void Kalman_2D_altitude_initialize(Kalman_2D* kalman, float Q[2][2], float R) {
	kalman->time = HAL_GetTick();

	kalman->x[0] = 0.0f;
	kalman->x[1] = 0.0f;

	kalman->P[0][0] = 1.0f;
	kalman->P[0][1] = 0.0f;
	kalman->P[1][0] = 0.0f;
	kalman->P[1][1] = 1.0f;

	kalman->Q[0][0] = Q[0][0];
	kalman->Q[0][1] = Q[0][1];
	kalman->Q[1][0] = Q[1][0];
	kalman->Q[1][1] = Q[1][1];

	kalman->R = R;

	kalman->measurement_mean = 0.0f;
	kalman->measurement_var  = R;

	kalman->process1_mean = 0.0f;
	kalman->process1_var  = Q[0][0];

	kalman->process2_mean = 0.0f;
	kalman->process2_var  = Q[1][1];
}

void Kalman_2D_altitude_predict(Kalman_2D* kalman, float acceleration, float process_alpha, float process_min_var) {
	uint32_t time = HAL_GetTick();
	float dt = ((float)(time - kalman->time)) / 1000.0f;
	kalman->time = time;

	// predict state
	kalman->x[0] += 0.5f * acceleration * dt * dt + kalman->x[1] * dt;
	kalman->x[1] += acceleration * dt;

	// update process variance
	float process1_input = 0.5f * acceleration * dt * dt;
	float process2_input = acceleration * dt;
	Kalman_2D_update_process_variance(kalman, process1_input, process2_input, process_alpha, process_min_var);

	// state transition matrix
	float F[2][2] = {
		{1.0f, dt},
		{0.0f, 1.0f}
	};

	// P = F * P * F^T + Q

	// F * P
	float FP00 = F[0][0] * kalman->P[0][0] + F[0][1] * kalman->P[1][0];
	float FP01 = F[0][0] * kalman->P[0][1] + F[0][1] * kalman->P[1][1];
	float FP10 = F[1][0] * kalman->P[0][0] + F[1][1] * kalman->P[1][0];
	float FP11 = F[1][0] * kalman->P[0][1] + F[1][1] * kalman->P[1][1];

	// FP * F^T
	float P00 = FP00 * F[0][0] + FP01 * F[0][1];
	float P01 = FP00 * F[1][0] + FP01 * F[1][1];
	float P10 = FP10 * F[0][0] + FP11 * F[0][1];
	float P11 = FP10 * F[1][0] + FP11 * F[1][1];

	// FPF^T + Q
	P00 += kalman->Q[0][0];
	P01 += kalman->Q[0][1];
	P10 += kalman->Q[1][0];
	P11 += kalman->Q[1][1];

	kalman->P[0][0] = P00;
	kalman->P[0][1] = P01;
	kalman->P[1][0] = P10;
	kalman->P[1][1] = P11;
}

void Kalman_2D_altitude_update(Kalman_2D* kalman, float altitude, float measurement_alpha, float measurement_min_var) {
	// update measurement variance
	Kalman_2D_update_measurement_variance(kalman, altitude, measurement_alpha, measurement_min_var);

	// measurement matrix
	float H[2] = {1.0f, 0.0f};

	// innovation
	float y = altitude - (H[0] * kalman->x[0] + H[1] * kalman->x[1]);

	// S = H * P * H^T + R
	float S =	H[0] * kalman->P[0][0] * H[0] +
            	H[0] * kalman->P[0][1] * H[1] +
				H[1] * kalman->P[1][0] * H[0] +
				H[1] * kalman->P[1][1] * H[1] +
				kalman->R;

	// kalman gain
	// K = P * H^T * S^-1
	float K[2];
	K[0] = (kalman->P[0][0] * H[0] + kalman->P[0][1] * H[1]) / S;
	K[1] = (kalman->P[1][0] * H[0] + kalman->P[1][1] * H[1]) / S;

	// update state
	kalman->x[0] += K[0] * y;
	kalman->x[1] += K[1] * y;

	// update covariance
	// P = (I - K * H) * P
	float KH[2][2] = {
		{K[0] * H[0], K[0] * H[1]},
		{K[1] * H[0], K[1] * H[1]}
	};

	float I_KH[2][2] = {
		{1.0f - KH[0][0], 0.0f - KH[0][1]},
		{0.0f - KH[1][0], 1.0f - KH[1][1]}
	};

	float P00 = I_KH[0][0] * kalman->P[0][0] + I_KH[0][1] * kalman->P[1][0];
	float P01 = I_KH[0][0] * kalman->P[0][1] + I_KH[0][1] * kalman->P[1][1];
	float P10 = I_KH[1][0] * kalman->P[0][0] + I_KH[1][1] * kalman->P[1][0];
	float P11 = I_KH[1][0] * kalman->P[0][1] + I_KH[1][1] * kalman->P[1][1];

	kalman->P[0][0] = P00;
	kalman->P[0][1] = P01;
	kalman->P[1][0] = P10;
	kalman->P[1][1] = P11;
}
