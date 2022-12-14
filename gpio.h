/* Wrapper around the xgpio driver.
 *
 * Sam Condon, 2022-10-31
 */
#ifndef GPIO_H
#define GPIO_H

#include "FreeRTOS.h"
#include "portmacro.h"
#include "xparameters.h"
#include "xgpio.h"
#include "adapter.h"

#define GPIO_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define GPIO_INTR_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define GPIO_INTR_MASK 0x01

#define GPIO_CHANNEL_1_DIR 0x03
#define GPIO_CHANNEL_2_DIR 0x00

class GpioAdapter;


using GpioAdapterBase = Adapter<GpioAdapter, XGpio, XGpio_Config>;


class GpioAdapter: public GpioAdapterBase::Adapter {
public:

	u32 read_state(u8 channel);
	void write_state(u8 channel, u32 state);

	u32 enable_interrupts(void);
	u32 disable_interrupts(void);
	u32 clear_interrupts(void);
	void (*post)(GpioAdapter *callback_ref);
	GpioAdapter(u16 hw_id, u32 intr_id, void (*post)(GpioAdapter *callback_ref));
};

#endif

