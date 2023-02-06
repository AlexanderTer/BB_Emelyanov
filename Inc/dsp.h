/**
 * \brief Файл - описание цифровой обработки сигналов
 */

#ifndef  __DSP_H__
#define __DSP_H__

#define LIMIT(x,min,max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max):(x)))

// --------------------------- Интегратор --------------------------
typedef struct {

	float k;   	// Коэффициент игтегратора (включает период дискретизации)
	float sum; 	// Сумма интегратора (выход)
	float c;	// Переменная для накомпления погрешности интегрирования в алгоритме Кэхена

	struct {
		float min; // Минимальное значение суммы
		float max; // Максимальное значение суммы
	} sat; // Струкура с параметрами ограничителя суммы

} Integrator_Struct;// Структура параметров интегратора (метод прямоугольников)


// --------------------------- Дифференциатор --------------------------
typedef struct {

	float k; 	// Коэффициент диффиринциатора (включает период дискретизации)
	float xz;	// Переменная хранения предыдущего значения входа

} Diff_Struct; // Структура параметров дифференциатора


// --------------------------- PID регулятор ------------------------
typedef struct {

	float kp;						// Пропорциональная часть
	float kb;						// Коэффициент обратной связи в алгоритме Back-calculation

	Integrator_Struct integrator;	// Интегральная часть
	Diff_Struct diff;				// Дифференциальная часть

	float bc;						// Переменная обратной связи алгоритма Back - calculation

	struct {
		float min; // Минимальное значение суммы
		float max; // Максимальное значение суммы
	} sat; // Струкура с параметрами ограничителя суммы
} PID_Controller_Struct;



float PID_Controller(PID_Controller_Struct *pid, float x);
float BackwardEuler_Diff(Diff_Struct *diff, float x);
float Trapezoidal_Integrator(Integrator_Struct *integrator, float x);





#endif
