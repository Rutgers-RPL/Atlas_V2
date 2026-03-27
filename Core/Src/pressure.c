#include "pressure.h"
#include "main.h"

/* ===========================================================
 * CALIBRATION CONSTANTS
 * ===========================================================
 * Calculated via 8-point linear regression.
 * Equation: PSI = (m * Raw_ADC) + b
 */
static const float g_pt_m_reg = 0.402096f;   // Calculated Slope
static const float g_pt_b_reg = -328.4047f;  // Calculated Y-Intercept

/* ===========================================================
 * INTERNAL STATE
 * =========================================================== */

static ADC_HandleTypeDef *g_hadc;

volatile uint16_t g_pt_raw[PT_COUNT];
volatile float    g_pt_pressure[PT_COUNT];

/* ===========================================================
 * INTERNAL PROTOTYPES
 * =========================================================== */
static void PT_SetMux(uint8_t code);
static void PT_SettleDelay(void);
static uint16_t PT_ReadADC(void);

/* ===========================================================
 * PUBLIC FUNCTIONS
 * =========================================================== */

void PT_Init(ADC_HandleTypeDef *hadc)
{
    g_hadc = hadc;
}

void PT_ScanAll(void)
{
    // Binary codes to select channels 0 through 5 on the multiplexer
    const uint8_t codes[PT_COUNT] = { 0b000, 0b001, 0b010, 0b011, 0b100, 0b101 };

    for (int i = 0; i < PT_COUNT; i++)
    {
        // 1. Switch the multiplexer to the current sensor
        PT_SetMux(codes[i]);

        // 2. Wait for the ADC sample-and-hold capacitor to charge
        PT_SettleDelay();

        // 3. Read the 12-bit raw integer from the ADC
        uint16_t raw = PT_ReadADC();
        g_pt_raw[i] = raw;

        // 4. Apply linear regression mapping (Raw ADC -> PSI)
        g_pt_pressure[i] = (g_pt_m_reg * (float)raw) + g_pt_b_reg;
    }
}

float PT_GetPressure(int index)
{
    if (index < 0 || index >= PT_COUNT)
        return 0.0f;
    return g_pt_pressure[index];
}

/* ===========================================================
 * INTERNAL HELPER FUNCTIONS
 * =========================================================== */

//static void PT_SetMux(uint8_t code)
//{
//    HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin,
//                      (code & 0x1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//
//    HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin,
//                      (code & 0x2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//
//    HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin,
//                      (code & 0x4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//}

void PT_SetMux(uint8_t code)
{
    switch (code)
    {
        case 0: // Binary 000
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_RESET);
            break;

        case 1: // Binary 001
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_RESET);
            break;

        case 2: // Binary 010
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_RESET);
            break;

        case 3: // Binary 011
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_RESET);
            break;

        case 4: // Binary 100
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_SET);
            break;

        case 5: // Binary 101
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_SET);
            break;

        default:
            // Optional: Set all LOW or HIGH if code is out of range (0-5)
            HAL_GPIO_WritePin(MULT_S1_GPIO_Port, MULT_S1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S2_GPIO_Port, MULT_S2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MULT_S3_GPIO_Port, MULT_S3_Pin, GPIO_PIN_RESET);
            break;
    }
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
