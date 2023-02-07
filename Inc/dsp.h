/**
 * \brief Файл - описание цифровой обработки сигналов
 */
#include "stm32f3xx.h"

#ifndef  __DSP_H__
#define __DSP_H__

#define LIMIT(x,min,max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max):(x)))

#define SUM_Q(a, b)     ( (a) + (b) )
#define SUB_Q(a, b)     ( (a) - (b) )
#define MUL_Q(a, b, q)  ( (int32_t)(((int64_t)(a)*(int64_t)(b)) >> (q)) )
#define DIV_Q(a, b, q)  ( (int32_t)(((int64_t)(a) << (q)) / (int64_t)(b)) )

#define F2Q(f, q)   ( (int32_t)((1 << (q)) * (f)) )
#define Q20(i, q)   ( (int32_t)(((i) + (1 << ((q) - 1))) >> (q)) )
#define Q2F(i, q)   ( (float)Q20((i), (q)) )

#define Q_USE       (16)



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

typedef struct
{
    int32_t k;        // Коэффициент интегратора (включает период дискретизации).
    int32_t sum;      // Сумма интегратора (выход).
    int32_t c;        // Переменая для накопления погрешности интегрирования в алгоритме Кэхэна.

    struct
    {
        int32_t min;  // Мин. значение суммы.
        int32_t max;  // Макс. значение суммы.

    } sat;  // Структура с параметрами ограничителя суммы.

} Integrator_Q_Struct;  // Структура параметров интегратора.


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


typedef struct
{
    int32_t kp;       // Коэффициент пропорциональной части.
    int32_t kb;       // Коэффициент обратной связи в алгоритме Back-calculation.

    Integrator_Q_Struct integrator;   // Интегральная часть.

    int32_t bc;        // Переменная обратной связи в алгоритме Back-calculation.

    struct
    {
        int32_t min;  // Мин. значение выхода.
        int32_t max;  // Макс. значение выхода.

    } sat;  // Структура с параметрами ограничителя выхода.

} PID_Controller_Q_Struct;    // Структура параметров ПИД-регулятора.

int32_t PID_Controller_Q(PID_Controller_Q_Struct *pid, int32_t x);
float PID_Controller(PID_Controller_Struct *pid, float x);
float BackwardEuler_Diff(Diff_Struct *diff, float x);
int32_t Trapezoidal_Integrator_Q(Integrator_Q_Struct * integrator, int32_t x);
float Trapezoidal_Integrator(Integrator_Struct *integrator, float x);





#endif
