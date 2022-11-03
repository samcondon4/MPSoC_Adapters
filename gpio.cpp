/* C++ wrapper around the xgpio driver.
 *
 *Sam Condon, 2022-10-31
 */
#include "gpio.h"

GpioAdapter::GpioAdapter(u16 hw_id, u32 intr_id, void (*post)(GpioAdapter *callback_ref)): Adapter(hw_id, intr_id){
	// - initialize the hardware - //
	this->child = this;
	this->post = post;
	this->lookup_config = XGpio_LookupConfig;
	this->config_init = XGpio_CfgInitialize;
	this->init_hw();

	// - configure data directions - //
	XGpio_SetDataDirection(&(this->connection), 1, GPIO_CHANNEL_1_DIR);
	XGpio_SetDataDirection(&(this->connection), 2, GPIO_CHANNEL_2_DIR);

	this->configure_interrupts();

}

u32 GpioAdapter::read_state(u8 channel){
	u32 state;
	state = XGpio_DiscreteRead(&(this->connection), channel);
	return state;
}

void GpioAdapter::write_state(u8 channel, u32 state){
	XGpio_DiscreteWrite(&(this->connection), channel, state);
}

u32 GpioAdapter::enable_interrupts(void){
	XGpio_InterruptEnable(&(this->connection), GPIO_INTR_MASK);
	XGpio_InterruptGlobalEnable(&(this->connection));

	return 0;
}

u32 GpioAdapter::disable_interrupts(void){
	XGpio_InterruptDisable(&(this->connection), GPIO_INTR_MASK);
	XGpio_InterruptGlobalDisable(&(this->connection));

	return 0;
}

u32 GpioAdapter::clear_interrupts(void){
	XGpio_InterruptClear(&(this->connection), GPIO_INTR_MASK);

	return 0;
}
