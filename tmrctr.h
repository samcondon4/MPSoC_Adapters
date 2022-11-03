/* C++ wrapper around the xtmrctr driver. Note that this wrapper currently
 * assumes that only one channel of the axi timer/counter peripheral is utilized.
 * The channel used is indicated with the TMR_CTR_NUM constant.
 *
 * Sam Condon, 2022-11-02
 */
#ifndef TMR_CTR_H
#define TMR_CTR_H

#include "FreeRTOS.h"
#include "portmacro.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "adapter.h"

#define TMR_CTR_ID XPAR_TMRCTR_0_DEVICE_ID
#define TMR_CTR_INTR_ID XPAR_FABRIC_TMRCTR_0_VEC_ID

#define TMR_CTR_NUM 0
#define TMR_CTR_OPTIONS XTC_DOWN_COUNT_OPTION
#define TMR_CTR_RESET 0x0bebc200


class TmrCtrAdapter;


using TmrCtrAdapterBase = Adapter<TmrCtrAdapter, XTmrCtr, XTmrCtr_Config>;


class TmrCtrAdapter: public TmrCtrAdapterBase::Adapter {
public:

	void reset(void);
	void start(void);
	void stop(void);

	u32 enable_interrupts(void);
	u32 disable_interrupts(void);
	u32 clear_interrupts(void);
	void (*post)(TmrCtrAdapter *callback_ref);
	TmrCtrAdapter(u16 hw_id, u32 intr_id, void (*post)(TmrCtrAdapter *callback_ref));
};

#endif
