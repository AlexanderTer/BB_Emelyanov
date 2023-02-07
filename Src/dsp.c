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

//inline int32_t Trapezoidal_Integrator_Q(Integrator_Q_Struct * integrator, int32_t x)
//{
//    // out[n] = s[n-1] + x[n] * k.
//    int32_t out = LIMIT(SUM_Q(integrator->sum, MUL_Q(x, integrator->k, Q_USE)), integrator->sat.min, integrator->sat.max);
//
//    // s[n] = out[n] + x[n] * k.
//    integrator->sum = SUM_Q(out, MUL_Q(x, integrator->k, Q_USE));
//
//    return out;
//}


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

//inline int32_t PID_Controller_Q(PID_Controller_Q_Struct * pid, int32_t x)
//{
//    // Расчёт пропорциональной части.
//    int32_t out_p = MUL_Q(x, pid->kp, Q_USE);
//
//    // Расчёт интегральной части.
//    int32_t out_i = Trapezoidal_Integrator_Q(&pid->integrator, x);
//
//    return LIMIT(SUM_Q(out_p, out_i), pid->sat.min, pid->sat.max);
//}

