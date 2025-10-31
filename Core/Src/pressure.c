#include "pressure.h"

// --- MUX select pins (adjust if you ever move them) ---
#define MUX_S1_GPIO  GPIOC
#define MUX_S1_PIN   GPIO_PIN_0
#define MUX_S2_GPIO  GPIOC
#define MUX_S2_PIN   GPIO_PIN_1
#define MUX_S3_GPIO  GPIOC
#define MUX_S3_PIN   GPIO_PIN_2

// --- ADC channel is PC3 (CH13) ---
#define PT_ADC_CHANNEL  ADC_CHANNEL_13

// --- Calibration constants ---
static float g_pt_m_common = 633.713f;  // psi per volt (initial slope)
static float g_pt_b[PT_COUNT];          // per-channel zero offset

// --- ADC handle (passed in from main) ---
static ADC_HandleTypeDef *g_hadc;

// --- Global arrays ---
volatile uint16_t g_pt_raw[PT_COUNT];
volatile float g_pt_pressure[PT_COUNT];

// --- Internal helper prototypes ---
static void PT_SetMux(uint8_t code);
static void PT_SettleDelay(void);
static uint16_t PT_ReadADC(void);
static float PT_AdcToVoltage(uint16_t adc_code);
static float PT_VoltageToPressure_Ch(float v, int ch);

/* ===========================================================
 *                 PUBLIC FUNCTIONS
 * =========================================================== */

void PT_Init(ADC_HandleTypeDef *hadc)
{
    g_hadc = hadc;
    PT_CalibrateZero();
}

void PT_ScanAll(void)
{
    const uint8_t codes[PT_COUNT] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b101};

    for (int i = 0; i < PT_COUNT; i++)
    {
        PT_SetMux(codes[i]);
        PT_SettleDelay();

        uint16_t raw = PT_ReadADC();
        g_pt_raw[i] = raw;

        float v = PT_AdcToVoltage(raw);
        float psi = PT_VoltageToPressure_Ch(v, i);

        g_pt_pressure[i] = psi;
    }
}

void PT_CalibrateZero(void)
{
    const uint8_t codes[PT_COUNT] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b101};

    for (int i = 0; i < PT_COUNT; i++)
    {
        float acc = 0.0f;
        int samples = 16;

        PT_SetMux(codes[i]);
        PT_SettleDelay();

        for (int k = 0; k < samples; k++)
        {
            uint16_t raw = PT_ReadADC();
            acc += PT_AdcToVoltage(raw);
        }

        float v0 = acc / (float)samples;
        g_pt_b[i] = -g_pt_m_common * v0;   // so P=0 at v0
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
    if (code & 0x1)
        HAL_GPIO_WritePin(MUX_S1_GPIO, MUX_S1_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(MUX_S1_GPIO, MUX_S1_PIN, GPIO_PIN_RESET);

    if (code & 0x2)
        HAL_GPIO_WritePin(MUX_S2_GPIO, MUX_S2_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(MUX_S2_GPIO, MUX_S2_PIN, GPIO_PIN_RESET);

    if (code & 0x4)
        HAL_GPIO_WritePin(MUX_S3_GPIO, MUX_S3_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(MUX_S3_GPIO, MUX_S3_PIN, GPIO_PIN_RESET);
}

static void PT_SettleDelay(void)
{
    for (volatile int i = 0; i < 400; ++i)
        __NOP(); // a few µs
}

static uint16_t PT_ReadADC(void)
{
    HAL_ADC_Start(g_hadc);
    HAL_ADC_PollForConversion(g_hadc, 10);
    uint16_t val = (uint16_t)HAL_ADC_GetValue(g_hadc);
    HAL_ADC_Stop(g_hadc);
    return val;
}

static float PT_AdcToVoltage(uint16_t adc_code)
{
    return ((float)adc_code / 4095.0f) * 3.3f;
}

static float PT_VoltageToPressure_Ch(float v, int ch)
{
    return g_pt_m_common * v + g_pt_b[ch];
}
