/* C++ adapter wrapper around the uart driver.
 *
 */
#include "xparameters.h"
#include "xuartps.h"
#include "adapter.h"

#define UART_ID XPAR_XUARTPS_0_DEVICE_ID
#define UART_INTR_ID XPAR_XUARTPS_0_INTR
#define UART_INTR_MASK XUARTPS_IXR_RXOVR

#define UART_BAUD 115200
#define RX_FIFO_SIZE 64
#define RX_TIMEOUT 2

#define HEADER_SIZE 0x02
#define HEADER_VALUE 0x55
#define TERMINATOR_SIZE 0x01
#define TERMINATOR_VALUE 0x0D
#define PACKET_GROUPS 0x03
#define PACKET_GROUP_SIZE 0x02

typedef u8 HEADER_TYPE;
typedef u16 GROUP_TYPE;
typedef u8 TERMINATOR_TYPE;
typedef enum ComStatus { PKT_SUCCESS=0, PKT_NO_TERMINATOR=1 } ComStatus;


class UartAdapter;


using UartAdapterBase = Adapter<UartAdapter, XUartPs, XUartPs_Config>;


class AdapterPacket {
/* The AdapterPacket class implements the data structure that separates the header, terminator, and data values.
 *
 */
public:
	HEADER_TYPE header[HEADER_SIZE];
	TERMINATOR_TYPE terminator[TERMINATOR_SIZE];

	u32 pkt_size;
	u8 *pkt_head;
	u8 pkt_data[PACKET_GROUPS*PACKET_GROUP_SIZE + TERMINATOR_SIZE];

	AdapterPacket(void);
	u32 reset_head(void);
	u32 write(u8 *buf, u32 buf_size);
	u8 *get_group_ptr(u8 group);
};


class UartAdapter: public UartAdapterBase::Adapter {
public:

	u32 isr_status;
	AdapterPacket *packet;
	u32 write_packet(void);
	u32 clear_packet(void);
	u32 clear_rx_fifo(void);
	u8 strcmp(u8 *str0, u8 *str1, u32 cmp_size);
	u32 enable_interrupts(void);
	u32 disable_interrupts(void);
	u32 clear_interrupts(void);
	void (*post)(UartAdapter *callback_ref);
	UartAdapter(u16 hw_id, u32 intr_id, void (*post)(UartAdapter *callback_ref), AdapterPacket *packet);
};
