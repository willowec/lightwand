#include "pico/stdlib.h"
#include "hardware/i2c.h"

/*
ADCL343 defines are taken from the Adafruit library
https://github.com/adafruit/Adafruit_ADXL343/blob/master/Adafruit_ADXL343.h
*/

#define ADXL343_DEFAULT_ADDRESS     (0x53)  /**< Assumes ALT address pin low */

// Registers
#define ADXL3XX_REG_DEVID           (0x00)  /**< Device ID */
#define ADXL3XX_REG_THRESH_TAP      (0x1D)  /**< Tap threshold */
#define ADXL3XX_REG_OFSX            (0x1E)  /**< X-axis offset */
#define ADXL3XX_REG_OFSY            (0x1F)  /**< Y-axis offset */
#define ADXL3XX_REG_OFSZ            (0x20)  /**< Z-axis offset */
#define ADXL3XX_REG_DUR             (0x21)  /**< Tap duration */
#define ADXL3XX_REG_LATENT          (0x22)  /**< Tap latency */
#define ADXL3XX_REG_WINDOW          (0x23)  /**< Tap window */
#define ADXL3XX_REG_THRESH_ACT      (0x24)  /**< Activity threshold */
#define ADXL3XX_REG_THRESH_INACT    (0x25)  /**< Inactivity threshold */
#define ADXL3XX_REG_TIME_INACT      (0x26)  /**< Inactivity time */
#define ADXL3XX_REG_ACT_INACT_CTL   (0x27)  /**< Axis enable control for activity and inactivity detection */
#define ADXL3XX_REG_THRESH_FF       (0x28)  /**< Free-fall threshold */
#define ADXL3XX_REG_TIME_FF         (0x29)  /**< Free-fall time */
#define ADXL3XX_REG_TAP_AXES        (0x2A)  /**< Axis control for single/double tap */
#define ADXL3XX_REG_ACT_TAP_STATUS  (0x2B)  /**< Source for single/double tap */
#define ADXL3XX_REG_BW_RATE         (0x2C)  /**< Data rate and power mode control */
#define ADXL3XX_REG_POWER_CTL       (0x2D)  /**< Power-saving features control */
#define ADXL3XX_REG_INT_ENABLE      (0x2E)  /**< Interrupt enable control */
#define ADXL3XX_REG_INT_MAP         (0x2F)  /**< Interrupt mapping control */
#define ADXL3XX_REG_INT_SOURCE      (0x30)  /**< Source of interrupts */
#define ADXL3XX_REG_DATA_FORMAT     (0x31)  /**< Data format control */
#define ADXL3XX_REG_DATAX0          (0x32)  /**< X-axis data 0 */
#define ADXL3XX_REG_DATAX1          (0x33)  /**< X-axis data 1 */
#define ADXL3XX_REG_DATAY0          (0x34)  /**< Y-axis data 0 */
#define ADXL3XX_REG_DATAY1          (0x35)  /**< Y-axis data 1 */
#define ADXL3XX_REG_DATAZ0          (0x36)  /**< Z-axis data 0 */
#define ADXL3XX_REG_DATAZ1          (0x37)  /**< Z-axis data 1 */
#define ADXL3XX_REG_FIFO_CTL        (0x38)  /**< FIFO control */
#define ADXL3XX_REG_FIFO_STATUS     (0x39)  /**< FIFO status */

#define ADXL3XX_RANGE_2G            (0x00)
#define ADXL3XX_RANGE_4G            (0x01)
#define ADXL3XX_RANGE_8G            (0x02)
#define ADXL3XX_RANGE_16G           (0x03)

#define ADXL343_I2C_TIMEOUT_US  5 * 1000 * 1000 // 5 second timeout      

// adxl343 struct
typedef struct adxl343_struct {
    uint8_t address;
    i2c_inst_t *i2c;
} adxl343;


/*
Sets up the i2c used to communicate to the adxl343, and configures
the device for usage

Returns:
    PICO_ERROR_NONE: success
    PICO_ERROR_TIMEOUT: i2c communications timed out
    PICO_ERROR_GENERIC: adxl343 did not respond with the expected device ID of 0xe5

*/
int adxl343_setup(adxl343 *accelerometer, i2c_inst_t *i2c, uint8_t SDA_pin, uint8_t SCL_pin, uint8_t address);

/*
Writes a register on the adxl343. Returns i2c errors if encountered
*/
int adxl343_write_register(adxl343 *accelerometer, uint8_t reg, uint8_t value);

/*
Reads 8 bits from a register on the adxl343. Returns i2c errors if encountered
*/
int adxl343_read_register_8(adxl343 *accelerometer, uint8_t reg, uint8_t *out_val);

/*
Reads 16 bits from a register on the adxl343. Returns i2c errors if encountered
*/
int adxl343_read_register_16(adxl343 *accelerometer, uint8_t reg, int16_t *out_val);

/*
Gets the most recent x axis value
*/
int adxl343_getx(adxl343 *accelerometer, int16_t *out_val);

/*
Gets the most recent y axis value
*/
int adxl343_gety(adxl343 *accelerometer, int16_t *out_val);

/*
Gets the most recent z axis value
*/
int adxl343_getz(adxl343 *accelerometer, int16_t *out_val);