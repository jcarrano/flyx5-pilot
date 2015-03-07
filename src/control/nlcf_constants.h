/**
 * @file nlcf_constants.c
 *
 * @brief Constants for non linear complementary filter.
 *
 * Define NLCF_SAMPLERATE before including this header.
 *
 * @author	Juan I Carrano.
 */

#ifndef NLCF_CONSTANTS_H

/*esto hay que hacerlo a mano
 * q[n+1] = q[n] + d_q/fs = q[n] + 0.5 * q[n] * p(omega + kp*wmes) / fs
 * machine_omega = real_omega / GYRO_SCALE
 * nos olvidamos del wmes por ahora
 *  =>
 * q[n+1] = q[n] + 0.5 * q[n] * p[machine_omega] * GYRO_SCALE / fs
 * Comprimiendo las constantes:
 * q[n+1] = q[n] + q[n] * p(machine_omega) / D_Q_SCALE
 * 	donde D_Q_SCALE = 1/(0.5 * GYRO_SCALE / fs) = 2*fs / GYRO_SCALE
 */
#define GYRO_SCALE 34.90658504 /* rad / s @ fsd ; son 5.125 bits */

#if NLCF_SAMPLERATE == 1000
#define D_Q_SCALE 57 /* en realidad es 57.29578 */
#elif NLCF_SAMPLERATE == 50
#define D_Q_SCALE 3 /*en realidad es 2.864788976, o 1.518 bits*/
#else
#error "Sample rate not supported"
#endif

/* esto también va a mano
 *	machine_wmes = wmes / ACC_SCALE
 * wmes va sumado al gyro, por lo tanto:
 * 	v = omega + kp*wmes
 * 	machine_v = machine_omega + machine_wmes/WMES_DIV
 * 	machine_v = v / GYRO_SCALE = real_omega / GYRO_SCALE + kp*wmes / GYRO_SCALE
 *  =>
 * 	kp*wmes/GYRO_SCALE = kp*machine_wmes*ACC_SCALE/GYRO_SCALE = machine_wmes/ WMES_DIV
 * WMES_DIV = GYRO_SCALE / (kp * ACC_SCALE)
 */
#define ACC_SCALE (16*9.81) /* = 156.96 = 16g m*s^2 , son 7.29 bits */

#if NLCF_SAMPLERATE == 1000
#define WMES_DIV 2 /*para probar */
#elif NLCF_SAMPLERATE == 50
#define WMES_MUL 4 // deberia ser 1/WMES_DIV, o 4.497
#else
#error "Sample rate not supported"
#endif

/* Con el bias hay 2 grados de libertad
 *  (1) bias en sí, que depende de wmes.
 *  (2) bias con respecto a omega
 * La clave está en saber en que rango puede estar bias para aprovechar todo
 * el rango dinámico (1), y a partir de eso determinar (2)
 * MPU-6000 Zero rate output (ZRO): +- 20 °/s = 0.35 rad / s. Empirico: 0.1 max
 *
 * machine_bias = bias / BIAS_SCALE
 *
 * (1)
 * 	bias[n+1] = bias[n] + Ki * wmes / fs
 * 	machine_bias[n+1] = bias[n+1] / BIAS_SCALE
 * 			= bias[n]/BIAS_SCALE + Ki * wmes / (BIAS_SCALE * fs)
 * 				= machine_bias + machine_wmes / D_BIAS_SCALE
 * machine_wmes =
 * 	=>
 * 	Ki*wmes/(BIAS_SCALE*fs) = machine_wmes / D_BIAS_SCALE
 * 	Ki*(machine_wmes * ACC_SCALE)/(BIAS_SCALE*fs) = machine_wmes / D_BIAS_SCALE
 * 	D_BIAS_SCALE = BIAS_SCALE*fs / (ACC_SCALE * Ki)
 *
 * (2)
 *	omega_corrected = omega + bias
 * 	machine_omega_corrected = omega_corrected / GYRO_SCALE
 * 	omega/GYRO_SCALE + bias/GYRO_SCALE = machine_omega + machine_bias / BIAS_SCALE2
 * 	machine_bias * BIAS_SCALE / GYRO_SCALE = machine_bias / BIAS_SCALE2;
 * 		=> BIAS_SCALE2 = GYRO_SCALE / BIAS_SCALE
 *
 * Solución 1: BIAS_SCALE = ZRO (empirico) = 0.1
 *		BIAS_SCALE2 = 349
 * 		D_BIAS_SCALE = 5/(156*0.3) = 0.106
 * 	Problema: en una multiplico y en la otra divido
 * Solucion 2: BIAS_SCALE = 2
 * 		BIAS_SCALE2 = 17
 * 		D_BIAS_SCALE = 2
 *
 * Nota: El bias se suma para no tener que escribir una función de sustraccion
 * de vectores.
 */
/* para 1000Hz: ???*/
#define Ki 0.3

#if NLCF_SAMPLERATE == 1000

#define D_BIAS_SCALE 32
#define BIAS_SCALE2 2 /* GYRO_SCALE / BIAS_SCALE */

#elif NLCF_SAMPLERATE == 50

/* esto es demasiado */
#define D_BIAS_SCALE 2
#define BIAS_SCALE2 17

#else
#error "Sample rate not supported"
#endif

#endif /* NLCF_CONSTANTS_H */
