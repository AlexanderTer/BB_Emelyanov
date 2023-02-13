#ifndef  __CONTROL_H__
#define __CONTROL_H__

#include "dsp.h"

#define SET_SHIFTS_MAX_COUNT ((unsigned int)(0.5 * 300.e3))

#define Fsw (100e3f)			// Частота коммутации, Гц
#define F_HIRES (144e6f * 32.f) // Частота таймера высокого разрешения, Гц
#define PERIOD (F_HIRES / Fsw)



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


	float duty; 					// Коэффициент заполнения, [0..2]
	float duty_Boost;
	float duty_Buck;
	float iL_ref;					// Уставка на ток рекатора

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
		float iL;
		float uout;
		float inj;
		float uin;
		float power;
		float temperature;

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
	float iL_max;
	float uin_max;
	float uin_min;
	float uout_max;
	float power_max;

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
