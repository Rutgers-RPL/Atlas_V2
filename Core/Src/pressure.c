#include "pressure.h"

/* ===========================================================
 *            SET THIS CONSTANT TO YOUR SENSOR RANGE
 * ===========================================================
 *
 * Valid datasheet full-scale values for ETM-375:
 *   35, 70, 170, 350, 700, 1000, 2000  (psi)
 */
#define PT_FULL_SCALE_PSI   2000.0f     // <<< CHANGE THIS >>>

// Slope = FS / 5V (sensor is 0–5 V full scale)
static const float g_pt_m = PT_FULL_SCALE_PSI / 5.0f;

/* ===========================================================
 *                    HARDWARE PIN DEFINITIONS
 * =========================================================== */

#define MUX_S1_GPIO  GPIOC
#define MUX_S1_PIN   GPIO_PIN_0
#define MUX_S2_GPIO  GPIOC
#define MUX_S2_PIN   GPIO_PIN_1
#define MUX_S3_GPIO  GPIOC
#define MUX_S3_PIN   GPIO_PIN_2

#define PT_ADC_CHANNEL  ADC_CHANNEL_13

/* ===========================================================
 *                    INTERNAL STATE
 * =========================================================== */

static ADC_HandleTypeDef *g_hadc;

volatile uint16_t g_pt_raw[PT_COUNT];
volatile float    g_pt_pressure[PT_COUNT];

/* ===========================================================
 *                   INTERNAL PROTOTYPES
 * =========================================================== */
static void PT_SetMux(uint8_t code);
static void PT_SettleDelay(void);
static uint16_t PT_ReadADC(void);
static float PT_AdcToVoltage(uint16_t adc_code);

/* ===========================================================
 *                    PUBLIC FUNCTIONS
 * =========================================================== */

void PT_Init(ADC_HandleTypeDef *hadc)
{
    g_hadc = hadc;
}

void PT_ScanAll(void)
{
    const uint8_t codes[PT_COUNT] =
    { 0b000, 0b001, 0b010, 0b011, 0b100, 0b101 };

    for (int i = 0; i < PT_COUNT; i++)
    {
        PT_SetMux(codes[i]);
        PT_SettleDelay();

        uint16_t raw = PT_ReadADC();
        g_pt_raw[i] = raw;

        float voltage = PT_AdcToVoltage(raw);
        g_pt_pressure[i] = g_pt_m * voltage;   // DIRECT: P = m * V
    }
}

float PT_GetPressure(int index)
{
    if (index < 0 || index >= PT_COUNT)
        return 0.0f;
    return g_pt_pressure[index];
}

float PT_GetVoltage(int index)
{
    if (index < 0 || index >= PT_COUNT)
        return 0.0f;
    return PT_AdcToVoltage(g_pt_raw[index]);
}

/* ===========================================================
 *                 INTERNAL HELPER FUNCTIONS
 * =========================================================== */

static void PT_SetMux(uint8_t code)
{
    HAL_GPIO_WritePin(MUX_S1_GPIO, MUX_S1_PIN,
                      (code & 0x1) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MUX_S2_GPIO, MUX_S2_PIN,
                      (code & 0x2) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MUX_S3_GPIO, MUX_S3_PIN,
                      (code & 0x4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void PT_SettleDelay(void)
{
    for (volatile int i = 0; i < 400; i++)
        __NOP();
}

static uint16_t PT_ReadADC(void)
{
    HAL_ADC_Start(g_hadc);
    HAL_ADC_PollForConversion(g_hadc, 10);
    uint16_t val = HAL_ADC_GetValue(g_hadc);
    HAL_ADC_Stop(g_hadc);
    return val;
}

static float PT_AdcToVoltage(uint16_t adc_code)
{
    return ((float)adc_code / 4095.0f) * 3.3f;
}
