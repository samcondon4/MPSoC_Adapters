/* C++ adapter wrapper around the uart driver.
 *
 */
#include "uart.h"


AdapterPacket::AdapterPacket(void){

	u8 i;
	// build the header and terminator ////////
	for (i = 0; i < HEADER_SIZE; i++) {
		this->header[i] = HEADER_VALUE;
	}
	for (i = 0; i < TERMINATOR_SIZE; i++) {
		this->terminator[i] = TERMINATOR_VALUE;
	}

	// set the packet head //
	this->pkt_head = this->pkt_data;

}

u32 AdapterPacket::reset_head(void) {
	this->pkt_head = this->pkt_data;
	return 0;
}

u32 AdapterPacket::write(u8 *buf, u32 buf_size){
	u32 i;
	for (i = 0; i < buf_size; i++) {
		*(this->pkt_head) = buf[i];
		this->pkt_head += 1;
	}

	return 0;
}

u8 *AdapterPacket::get_group_ptr(u8 group){
	return this->pkt_data + group*PACKET_GROUP_SIZE;
}


int uart_cfg_initialize(XUartPs *InstancePtr, XUartPs_Config * Config, unsigned int EffectiveAddr){
	return (u32)XUartPs_CfgInitialize(InstancePtr, Config, EffectiveAddr);
}


UartAdapter::UartAdapter(u16 hw_id, u32 intr_id, void (*post)(UartAdapter *callback_ref), AdapterPacket *packet): Adapter(hw_id, intr_id){

	this->child = this;
	this->post = post;
	this->lookup_config = XUartPs_LookupConfig;
	this->config_init = uart_cfg_initialize;

	this->init_hw();
	// - xuartps specific initialization - //
	XUartPs_SetBaudRate(&(this->connection), UART_BAUD);
	XUartPs_SetFifoThreshold(&(this->connection), HEADER_SIZE);
	XUartPs_SetRecvTimeout(&(this->connection), RX_TIMEOUT);

	this->configure_interrupts();
}

u32 UartAdapter::write_packet(void){

	u32 bytes;
	u32 pkt_size;
	u8 *pkt_head;
	u8 *terminator;
	ComStatus ret;

	bytes = 0xff;
	pkt_size = 0;
	pkt_head = this->packet->pkt_head;
	terminator = this->packet->terminator;

	// look for terminator while writing data to the packet. //
	while (pkt_size < PACKET_GROUPS*PACKET_GROUP_SIZE + TERMINATOR_SIZE) {
		bytes = XUartPs_Recv(&(this->connection), pkt_head, TERMINATOR_SIZE);
		if (this->strcmp(pkt_head, terminator, TERMINATOR_SIZE) == 0) {
			break;
		}
		else{
			pkt_head += bytes;
			pkt_size += bytes;
		}
	}

	// if the terminator was found, clear it from the packet and return a success. //
	if (this->strcmp(pkt_head, terminator, TERMINATOR_SIZE) == 0) {
		*pkt_head = 0x00;
		ret = PKT_SUCCESS;
	}
	// otherwise, return terminator not found error //
	else {
		ret = PKT_NO_TERMINATOR;
	}

	return (u32)ret;
}

u32 UartAdapter::clear_packet(void){
	u8 i;
	u8 *pkt_data;

	pkt_data = this->packet->pkt_data;
	for (i = 0; i < PACKET_GROUPS*PACKET_GROUP_SIZE; i++ ) {
		pkt_data[i] = 0x00;
	}
	this->packet->reset_head();

	return 0;
}

u32 UartAdapter::clear_rx_fifo(void){
	u32 bytes;
	u8 buf[1];
	bytes = 0xff;
	while (bytes != 0) {
		bytes = XUartPs_Recv(&(this->connection), buf, 1);
	}
	// halt any pending issued receive operation before returning. //
	XUartPs_Recv(&(this->connection), buf, 0);
	return 0;
}

u8 UartAdapter::strcmp(u8 *str0, u8 *str1, u32 cmp_size){
	u32 i;
	u8 ret = 0;
	for(i = 0; i < cmp_size; i++){
		if (str0[i] != str1[i]){
			ret = -1;
			break;
		}
	}

	return ret;
}

u32 UartAdapter::enable_interrupts(void){
	u32 intr_mask;
	intr_mask = XUartPs_GetInterruptMask(&(this->connection));
	intr_mask |= UART_INTR_MASK;
	XUartPs_SetInterruptMask(&(this->connection), intr_mask);

	return 0;
}

u32 UartAdapter::disable_interrupts(void){
	u32 intr_mask;
	intr_mask = XUartPs_GetInterruptMask(&(this->connection));
	intr_mask &= ~UART_INTR_MASK;
	XUartPs_SetInterruptMask(&(this->connection), intr_mask);

	return 0;
}

u32 UartAdapter::clear_interrupts(void){
	this->isr_status = XUartPs_ReadReg(this->connection.Config.BaseAddress, XUARTPS_ISR_OFFSET);
	XUartPs_WriteReg(this->connection.Config.BaseAddress, XUARTPS_ISR_OFFSET, this->isr_status);

	return this->isr_status;
}
