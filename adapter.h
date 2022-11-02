/* ADAPTER: Class template for C++ wrappers of Xilinx standalone peripheral drivers
 *
 * Sam Condon, 2022-11-01
 */
#ifndef ADAPTER
#define ADAPTER

#include "FreeRTOS.h"
#include "portmacro.h"
#include "xil_types.h"


template <typename child_type, typename connection_type, typename connection_cfg_type>
class Adapter {
public:
	u16 hw_id;
	u32 intr_id;
	child_type *child;
	connection_type connection;
	connection_cfg_type *connection_cfg;

	// - function pointers set by derived classes - //
	connection_cfg_type *(*lookup_config)(u16 hw_id);
	int (*config_init)(connection_type *connection, connection_cfg_type *cfg, unsigned int base_addr);

	u32 init_hw(void);
	u32 configure_interrupts(void);
	Adapter(u16 hw_id, u32 intr_id);

private:
	static void connection_isr(void *child);

};

template <typename child_type, typename connection_type, typename connection_cfg_type>
Adapter<child_type, connection_type, connection_cfg_type>::Adapter(u16 hw_id, u32 intr_id){
	this->hw_id = hw_id;
	this->intr_id = intr_id;
}

template <typename child_type, typename connection_type, typename connection_cfg_type>
u32 Adapter<child_type, connection_type, connection_cfg_type>::init_hw(void){
	u32 status;
	u32 base_addr;
	this->connection_cfg = this->lookup_config(this->hw_id);
	base_addr = this->connection_cfg->BaseAddress;
	status = this->config_init(&(this->connection), this->connection_cfg, base_addr);

	return status;
}

template <typename child_type, typename connection_type, typename connection_cfg_type>
void Adapter<child_type, connection_type, connection_cfg_type>::connection_isr(void *child){
	child_type *child_ref = (child_type *)child;
	child_ref->disable_interrupts();
	child_ref->clear_interrupts();
	child_ref->post(child_ref);
	child_ref->enable_interrupts();
}

template <typename child_type, typename connection_type, typename connection_cfg_type>
u32 Adapter<child_type, connection_type, connection_cfg_type>::configure_interrupts(void){
	this->child->enable_interrupts();

#ifdef INC_FREERTOS_H
	u32 ret;

	BaseType_t status;
	ret = 0;
	status = xPortInstallInterruptHandler(this->intr_id, this->connection_isr, (void *)this->child);
	if (status != pdPASS){
		ret = -1;
	}
	vPortEnableInterrupt(this->intr_id);
#else

#endif

	return ret;
}

#endif // - ifndef ADAPTER - //
