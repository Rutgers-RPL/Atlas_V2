#ifndef PRESSURE_H
#define PRESSURE_H

#include "stm32f4xx_hal.h"

// Number of transducers connected to the multiplexer
#define PT_COUNT 6

// Public global data (if needed)
extern volatile uint16_t g_pt_raw[PT_COUNT];
extern volatile float g_pt_pressure[PT_COUNT];

// --- Public API ---
void PT_Init(ADC_HandleTypeDef *hadc);
void PT_ScanAll(void);
void PT_CalibrateZero(void);
float PT_GetPressure(int index);
float PT_GetVoltage(int index);

#endif // PRESSURE_H
