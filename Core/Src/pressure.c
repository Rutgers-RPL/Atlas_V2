#include "pressure.h"
#include "main.h"

/* ===========================================================
 * CALIBRATION CONSTANTS
 * ===========================================================
 * Derived from Kulite Calibration Certificate (8864-11-799)
 * Formula: Pressure (psiA) = (Output Voltage (mV) - 224.94 mV) / 2.376 (mV/psiA)
 */
static const float g_pt_sensitivity = 2.376f;  // mV/psiA
static const float g_pt_zero_offset = 224.94f; // mV

/* ADC Conversion Constants */
// Adjust ADC_VREF_MV if using a 5V reference (5000.0f) or if a voltage divider is present
static const float ADC_VREF_MV = 3300.0f;      // Standard STM32 3.3V reference in millivolts
static const float ADC_MAX_RAW = 4095.0f;      // 12-bit ADC maximum raw value

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

        // 4. Convert the raw ADC value to millivolts
        float voltage_mv = ((float)raw / ADC_MAX_RAW) * ADC_VREF_MV;

        // 5. Apply Kulite calibration mapping (mV -> psiA)
        g_pt_pressure[i] = (voltage_mv - g_pt_zero_offset) / g_pt_sensitivity;
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
