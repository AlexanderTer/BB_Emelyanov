#ifndef  __CONTROL_H__
#define __CONTROL_H__

#include "dsp.h"

#define SET_SHIFTS_MAX_COUNT ((unsigned int)(0.5 * 300.e3))

#define Fsw (100e3)
#define PERIOD (144e6 * 32.f / Fsw)



#define DUTY_MIN_BUCK (0.05)
#define DUTY_MAX_BUCK (0.9)
#define DUTY_MIN_BOOST  (0.1)
#define DUTY_MAX_BOOST (0.95)

#define U_MIN_BUCK DUTY_MIN_BUCK
#define U_MAX_BUCK DUTY_MAX_BUCK
#define U_MIN_BOOST 1 + DUTY_MIN_BOOST
#define U_MAX_BOOST 1 + DUTY_MAX_BOOST


// ---------------- Структура процесса регулирования ----------------
typedef struct {


	int32_t duty; 					// Коэффициент заполнения, [0..2]
	int32_t duty_Boost;
	int32_t duty_Buck;
	int32_t iL_ref;					// Уставка на ток рекатора

	PID_Controller_Struct pid_current;// Структура регулятора тока реактора
	PID_Controller_Struct pid_voltage;// Структура регулятора напряжения

} Control_Struct; // Структура с параметрами системы кправления
extern Control_Struct BB_Control;

// ---------------- Структура измерений ----------------
typedef struct
{
	unsigned int count; // 0 - режим измерения(не происходит автоопределения смещения)
						// >0 - режим автоопределения смещения

	struct
	{
		int32_t iL;
		int32_t uout;
		int32_t inj;
		int32_t uin;
		int32_t power;
		int32_t temperature;

	}data, shift, scale,sum;
	// data - истинное значение после пересчёта,
	// shift - смещение
	// scale - коэффициент масштабирования
	// sum - переменная для накопления при автоопределении смещения

}Measure_Struct; // Структура с параметрами измерений
extern Measure_Struct BB_Measure;



// ---------------- Структура программных защит ----------------
typedef struct
{
	int32_t iL_max;
	int32_t uin_max;
	int32_t uin_min;
	int32_t uout_max;
	int32_t power_max;

}Protect_Struct;
extern Protect_Struct BB_Protect;

// ---------------- Структура режмов работы----------------
typedef enum
{
	BUCK,
	BUCK_BOOST,
	BOOST,
	FAULT,
}State;
extern State BB_State;

void ADC_Data_Hanler (void);
void software_protection_monitor(void);
void timer_PWM_off(void);
void set_Duty(void);
#endif
