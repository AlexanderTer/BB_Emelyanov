#ifndef  __CONTROL_H__
#define __CONTROL_H__



typedef struct
{
	struct
	{
		float iL;
		float uout;
		float inj;
		float uin;
	} data, shift, scale;
	// data - истинное значение после пересчёта,
	// shift - смещение
	// scale - коэффициент масштабир

}Measure_Struct; // Структура с параметрами измерений

extern Measure_Struct BB_Control;

#endif
