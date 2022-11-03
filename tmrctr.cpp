/* C++ wrapper around the xtmrctr driver. Note that this wrapper currently
 * assumes that only one channel of the axi timer/counter peripheral is utilized.
 * The channel used is indicated with the TMR_CTR_NUM constant.
 *
 * Sam Condon, 2022-11-02
 */
#include "tmrctr.h"


int tmrctr_cfg_initialize(XTmrCtr *InstancePtr, XTmrCtr_Config *CfgPtr, unsigned int BaseAddr){
	XTmrCtr_CfgInitialize(InstancePtr, CfgPtr, BaseAddr);
	return 0;
}


TmrCtrAdapter::TmrCtrAdapter(u16 hw_id, u32 intr_id, void (*post)(TmrCtrAdapter *callback_ref)): Adapter(hw_id, intr_id){
	this->child = this;
	this->post = post;
	this->lookup_config = XTmrCtr_LookupConfig;
	this->config_init = tmrctr_cfg_initialize;
	this->init_hw();

	// - set the tmrctr options - //
	XTmrCtr_SetOptions(&(this->connection), TMR_CTR_NUM, TMR_CTR_OPTIONS);
	XTmrCtr_SetResetValue(&(this->connection), TMR_CTR_NUM, TMR_CTR_RESET);

	this->configure_interrupts();
}

void TmrCtrAdapter::reset(void){
	XTmrCtr_Reset(&(this->connection), TMR_CTR_NUM);
}

void TmrCtrAdapter::start(void){
	XTmrCtr_Start(&(this->connection), TMR_CTR_NUM);
}

void TmrCtrAdapter::stop(void){
	XTmrCtr_Stop(&(this->connection), TMR_CTR_NUM);
}

u32 TmrCtrAdapter::enable_interrupts(void){
	u32 options;

	options = XTmrCtr_GetOptions(&(this->connection), TMR_CTR_NUM);
	options |= XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(&(this->connection), TMR_CTR_NUM, options);

	return 0;
}

u32 TmrCtrAdapter::disable_interrupts(void){
	u32 options;

	options = XTmrCtr_GetOptions(&(this->connection), TMR_CTR_NUM);
	options &= ~XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(&(this->connection), TMR_CTR_NUM, options);

	return 0;
}

u32 TmrCtrAdapter::clear_interrupts(void){
	u32 csr_reg;

	csr_reg = XTmrCtr_ReadReg(this->connection.BaseAddress, TMR_CTR_NUM, XTC_TCSR_OFFSET);
	csr_reg &= ~XTC_CSR_ENABLE_TMR_MASK;
	XTmrCtr_WriteReg(this->connection.BaseAddress, TMR_CTR_NUM, XTC_TCSR_OFFSET, csr_reg | XTC_CSR_LOAD_MASK);
	XTmrCtr_WriteReg(this->connection.BaseAddress, TMR_CTR_NUM, XTC_TCSR_OFFSET, csr_reg);
	XTmrCtr_WriteReg(this->connection.BaseAddress, TMR_CTR_NUM, XTC_TCSR_OFFSET, csr_reg | XTC_CSR_INT_OCCURED_MASK);

	return 0;
}
