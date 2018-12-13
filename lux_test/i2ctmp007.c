#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include "TSL2591_def.h"

/* Driver Header files */
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>

/* Example/Board Header files */
#include "Board.h"
//#include <ti/drivers/i2c/i2CCC26XX.h>

#define TASKSTACKSIZE       640

static Display_Handle display;

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    uint32_t        temp1;
    uint8_t         txBuf[4];
    uint8_t         rxBuf[4];
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;

    /* Call driver init functions */
    Display_init();

    I2C_init();

    /* Open the HOST display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        while (1);
    }

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
        Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "I2C Initialized!\n");
    }

    i2cTransaction.slaveAddress = 0x29;
    i2cTransaction.readBuf = rxBuf;
    i2cTransaction.writeBuf = txBuf;

    i2cTransaction.writeCount = 2;
    txBuf[0] = 0xA1;
    txBuf[1] = 0x10;
    i2cTransaction.readCount = 0;
    if (I2C_transfer(i2c,&i2cTransaction)){
    }

    txBuf[0] = 0xA0; //configures the TSL2591 to have medium gain
    txBuf[1] = 0x8B; ////enables proper interrupts and power to work with TSL2591
    if (I2C_transfer(i2c,&i2cTransaction)){
    }

    i2cTransaction.writeCount = 2;
    txBuf[0] = 0xB4; //(TSL2591_COMMAND_BIT | TSL2591_C0DATAL)
    txBuf[1] = 0xB5; //(TSL2591_COMMAND_BIT | TSL2591_C0DATAH)
    i2cTransaction.readCount = 2;

int x;
    /* Take 20 samples and print them out onto the console */
for(x = 0; x < 10000; x++){
    if (I2C_transfer(i2c, &i2cTransaction)) {
        temp1 = rxBuf[0];
        temp1 <<= 16;
        temp1 |= rxBuf[1];
        temp1 /= 300000;
        Display_printf(display, 0, 0, "Lux value: %d\n", temp1);

    }
        else {
            Display_printf(display, 0, 0, "I2C Bus fault\n");
        }


}
    /* Deinitialized I2C */
    I2C_close(i2c);
    Display_printf(display, 0, 0, "I2C closed!\n");

    return (NULL);
}
