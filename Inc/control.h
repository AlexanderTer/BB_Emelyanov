#ifndef  __CONTROL_H__
#define __CONTROL_H__


// ---------------- Структура измерений ----------------
typedef struct
{
	struct
	{
		float iL;
		float uout;
		float inj;
		float uin;
		float power

	} data, shift, scale;
	// data - истинное значение после пересчёта,
	// shift - смещение
	// scale - коэффициент масштабирования

}Measure_Struct; // Структура с параметрами измерений
extern Measure_Struct BB_Control;



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





void ADC_Data_Hanler (void);
void software_protection_monitor(void);
void timer_PWM_off(void);
#endif
