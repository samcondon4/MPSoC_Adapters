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
	this->configure_interrupts();
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
