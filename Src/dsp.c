/**
 * \brief Файл - реализация цифровой обработки сигналов
 */

#include "dsp.h"
#include "stm32f3xx.h"


/**
 * \brief Функция интегратора методом трапеций
 * \param integrator: структура с параметрами интегратора
 * \param x: вход интегратора
 * \return y: выход интегратора
 */
float Trapezoidal_Integrator(Integrator_Struct *integrator, float x) {


	// y[n] = s[n-1] + x[n]*k
	float out = LIMIT(integrator->sum + integrator->k * x, integrator->sat.min,
			integrator->sat.max);

	// s[n] = y[n] * k
	integrator->sum = out + x * integrator->k;
	return out;
}

/**
 * \brief Функция дифференциатора методом Эйлера
 * \param diff: структура с параметрами дифференциатора
 * \param x: вход дифференциатора
 * \return y: выход дифференциатора
 */
float BackwardEuler_Diff(Diff_Struct *diff, float x) {

	// y[n] = (x[n] - x[n-1]) * k
	float out = (x - diff->xz) * diff->k;

	// Сохнаняем текущее значение х
	diff->xz = x;
	return out;
}

/**
 * \brief Функция ПИД-регулятора
 * \param pid : струтура ПИД-регулятора
 * \param x: вход регулятора
 * \param у: выход регулятора
 */
float PID_Controller(PID_Controller_Struct *pid, float x) {

	// Расчёт пропорциональной части
	float out_p = x * pid->kp;

	// Расчёт интегральной части
	float out_i = Trapezoidal_Integrator(&pid->integrator, x);

	// Расчёт дифференциальной части
	float out_d = BackwardEuler_Diff(&pid->diff, x);
	return LIMIT(out_p + out_i + out_d, pid->sat.min, pid->sat.max);
}



