#include "hx711.h"
#include "hx711Config.h"
#if (_HX711_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define hx711_delay(x)    osDelay(x)
#else
#define hx711_delay(x)    HAL_Delay(x)
#endif

//#############################################################################################
void hx711_delay_us(void)
{
  uint32_t delay = _HX711_DELAY_US_LOOP;
  while (delay > 0)
  {
    delay--;
    __NOP(); __NOP(); __NOP(); __NOP();
  }
}
//#############################################################################################
void hx711_lock(hx711_t *hx711)
{
  while (hx711->lock)
    hx711_delay(1);
  hx711->lock = 1;
}
//#############################################################################################
void hx711_unlock(hx711_t *hx711)
{
  hx711->lock = 0;
}
//#############################################################################################
void hx711_init(hx711_t *hx711, GPIO_TypeDef *clk_gpio, uint16_t clk_pin, GPIO_TypeDef *dat_gpio, uint16_t dat_pin)
{
  hx711_lock(hx711);
  hx711->clk_gpio = clk_gpio;
  hx711->clk_pin = clk_pin;
  hx711->dat_gpio = dat_gpio;
  hx711->dat_pin = dat_pin;

  GPIO_InitTypeDef  gpio = {0};
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = clk_pin;
  HAL_GPIO_Init(clk_gpio, &gpio);
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = dat_pin;
  HAL_GPIO_Init(dat_gpio, &gpio);
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
  hx711_delay(10);
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
  hx711_delay(10);
  hx711_value(hx711);
  hx711_value(hx711);
  hx711_unlock(hx711);
}
//#############################################################################################
int32_t hx711_value(hx711_t *hx711)
{
  uint32_t data = 0;
  uint32_t  startTime = HAL_GetTick();
  while(HAL_GPIO_ReadPin(hx711->dat_gpio, hx711->dat_pin) == GPIO_PIN_SET)
  {
    hx711_delay(1);
    if(HAL_GetTick() - startTime > 150)
      return 0;
  }
  for(int8_t i=0; i<24 ; i++)
  {
    HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
    hx711_delay_us();
    HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
    hx711_delay_us();
    data = data << 1;
    if(HAL_GPIO_ReadPin(hx711->dat_gpio, hx711->dat_pin) == GPIO_PIN_SET)
      data ++;
  }
  data = data ^ 0x800000;
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
  hx711_delay_us();
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
  hx711_delay_us();
  return data;
}
//#############################################################################################
int32_t hx711_value_ave(hx711_t *hx711, uint16_t sample)
{
  hx711_lock(hx711);
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711_value(hx711);
    hx711_delay(5);
  }
  int32_t answer = (int32_t)(ave / sample);
  hx711_unlock(hx711);
  return answer;
}
//#############################################################################################
void hx711_tare(hx711_t *hx711, uint16_t sample)
{
  hx711_lock(hx711);
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711_value(hx711);
    hx711_delay(5);
  }
  hx711->offset = (int32_t)(ave / sample);
  hx711_unlock(hx711);
}
//#############################################################################################
void hx711_calibration(hx711_t *hx711, int32_t noload_raw, int32_t load_raw, float scale)
{
  hx711_lock(hx711);
  hx711->offset = noload_raw;
  hx711->coef = (load_raw - noload_raw) / scale;
  hx711_unlock(hx711);
}
//#############################################################################################
float hx711_weight(hx711_t *hx711, uint16_t sample)
{
  hx711_lock(hx711);
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711_value(hx711);
    hx711_delay(5);
  }
  int32_t data = (int32_t)(ave / sample);
  float answer =  (data - hx711->offset) / hx711->coef;
  hx711_unlock(hx711);
  return answer;
}
//#############################################################################################
void hx711_coef_set(hx711_t *hx711, float coef)
{
  hx711->coef = coef;
}
//#############################################################################################
float hx711_coef_get(hx711_t *hx711)
{
  return hx711->coef;
}
//#############################################################################################
void hx711_power_down(hx711_t *hx711)
{
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
  hx711_delay(1);
}
//#############################################################################################
void hx711_power_up(hx711_t *hx711)
{
  HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
}
//#############################################################################################
//#############################################################################################
// Debug-only calibration helper — stores results in globals for debugger inspection
//#############################################################################################
long hx711_zero_reading = 0;
long hx711_loaded_reading = 0;
long hx711_delta = 0;
float hx711_scale_factor = 0.0f;
float hx711_known_weight = 0.0f;

void hx711_calibrate_debug(hx711_t *hx711, float known_weight)
{
  hx711_known_weight = known_weight;

  // Step 1: Tare (zero load)
  hx711_tare(hx711, 10);
  hx711_zero_reading = hx711_value(hx711);

  // Step 2: Delay to allow placing known weight
  HAL_Delay(5000);  // 5 seconds to put the calibration mass

  // Step 3: Read raw data with known weight
  hx711_loaded_reading = hx711_value(hx711);

  // Step 4: Compute scale factor
  hx711_delta = hx711_loaded_reading - hx711_zero_reading;
  hx711_scale_factor = (float)hx711_delta / known_weight;

  // Step 5: Apply new calibration coefficient
  hx711_coef_set(hx711, hx711_scale_factor);
}

//chat wrote this ngl LMAO
//#############################################################################################
// Multi-point calibration using least-squares regression
// raw = coef * weight + offset
//#############################################################################################
void hx711_calibration_multipoint(
    hx711_t *hx711,
    const float *weights,
    const int32_t *raws,
    uint16_t n,
    float *out_coef,
    int32_t *out_offset
)
{
    if (n < 2)
        return;

    float sum_w  = 0.0f;
    float sum_r  = 0.0f;
    float sum_wr = 0.0f;
    float sum_w2 = 0.0f;

    for (uint16_t i = 0; i < n; i++)
    {
        sum_w  += weights[i];
        sum_r  += raws[i];
        sum_wr += weights[i] * raws[i];
        sum_w2 += weights[i] * weights[i];
    }

    float denom = n * sum_w2 - sum_w * sum_w;
    if (denom == 0.0f)
        return;

    float coef   = (n * sum_wr - sum_w * sum_r) / denom;
    float offset = (sum_r - coef * sum_w) / n;

    hx711_lock(hx711);
    hx711->coef   = coef;
    hx711->offset = (int32_t)offset;
    hx711_unlock(hx711);

    // Copy results back to main-owned variables
    if (out_coef)
        *out_coef = coef;
    if (out_offset)
        *out_offset = (int32_t)offset;
}


