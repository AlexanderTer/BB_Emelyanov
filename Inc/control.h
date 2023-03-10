#ifndef  __CONTROL_H__
#define __CONTROL_H__

#include "dsp.h"

#define SET_SHIFTS_MAX_COUNT ((unsigned int)(0.5 * 300.e3))

#define FSW (300e3f)			// Частота коммутации, Гц
#define F_HIRES (144e6f * 32.f) // Эквивалентная частота таймера высокого разрешения, Гц
#define PERIOD (F_HIRES / FSW)  // Период для записи в регистр периода

#define DUTY_MIN_BUCK (0.05f)
#define DUTY_MAX_BUCK (0.9f)
#define DUTY_MIN_BOOST  (0.1f)
#define DUTY_MAX_BOOST (0.95f)

#define U_MIN_BUCK (DUTY_MIN_BUCK)
#define U_MAX_BUCK (DUTY_MAX_BUCK)
#define U_CENTER (1.f)
#define U_MIN_BOOST (U_CENTER + DUTY_MIN_BOOST)
#define U_MAX_BOOST (U_CENTER + DUTY_MAX_BOOST)

#define U_CENTER_HYST_BUCK (U_CENTER - 0.05f)
#define U_CENTER_HYST_BOOST (U_CENTER + 0.05f)
#define U_MAX_BUCK_HYST (U_MAX_BUCK - 0.05f)
#define U_MIN_BOOST_HYST (U_MIN_BOOST + 0.05f)



// ---------------- Структура процесса регулирования ----------------
typedef struct {

	float duty; 					// Коэффициент заполнения, [0..2]
	float duty_Boost;
	float duty_Buck;
	float iL_ref;					// Уставка на ток рекатора
	float uout_ref;
	float error_current;
	float error_voltage;

	PID_Controller_Struct pid_current;// Структура регулятора тока реактора
	PID_Controller_Struct pid_voltage;// Структура регулятора напряжения

} Control_Struct; // Структура с параметрами системы кправления
extern Control_Struct BB_Control;

// ---------------- Структура измерений ----------------
typedef struct
{
	volatile unsigned int count; // 0 - режим измерения(не происходит автоопределения смещения)
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

	struct
	{
		float data;			// Значение переменной для вывода на ЦАП
		float shift;		// Смещение значения переменной
		float scale;		// Коэффициент масштабирования переменной

	} dac[2]; // Структура с параметрами ЦАП1 и ЦАП2

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
void set_shifts(void);
#endif
