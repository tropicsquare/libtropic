# Functional Mock Tests
Functional Mock Tests are a special type of test which are run against mock HAL (not against any "real" target including the model). As such, all communication can (and has to be) mocked.

To write this type of test, you need to understand TROPIC01's communication protocol at the lowest level. At first, remainder of units and layers of communication:

- One unit of communication on L1 = SPI transfer, represented by `lt_port_spi_transfer`. One more more bytes can be transferred during one transfer.
- One unit of communication on L2 = frame. Initiated by negedge on CSN and terminated by posedge on CSN. Frame can be transferred during single or multiple L1 transfer. As long as the CSN is low and any part of the frame is remaining, host can do SPI transfers. As a consequence, frame can be read even byte-by-byte.
- One unit of communication on L3 = packet. Encapsulated in L2 frame's REQ_DATA/RSP_DATA (similar to ISO/OSI encapsulation). L2 frame size is limited and as such it can happen that L3 packet has to be split into multiple frames. This is not really relevant from the point of the HAL -- it is still a part of the frame.

Additional key points:

- We mock incoming data -- the data that would be on the MISO line.
- There's no difference between "writing" and "reading". Everything is a SPI transaction, where the contents of buffers are swapped. We distinguish "reads" from "writes" only by a different `REQ_ID`. "Reads" have `REQ_ID == 0xAA`. Everything else is a "write". As such, you need to mock incoming data also for "writes", which is currently only a single `CHIP_STATUS` byte. Other bytes can be undefined/invalid/zero.
  - Actually, length of outcoming and incoming data should always match, due to the nature of the SPI protocol. However, to simplify things, HAL does not require to fill the rest of the space after CHIP_STATUS with
    something. If Libtropic tries to read incoming data (MISO) beyond data queued by user, it returns zeroes (up to the LT_L1_MAX_LENGTH). There's a logging macro which can be enabled to simplify test design.
- Data sent to the mocked HAL (outgoing data -- "MOSI") are ignored in current implementation to keep test complexity bearable.
- Data are mocked by "frames", not by bytes. After CSN posedge, next mocked transaction is prepared in the internal mock queue.
  - By "frames", we also mean the single CHIP_STATUS that is sent when `REQ_ID == Get_Response`.
- If no mocked transaction is available in the queue, attempt at starting the transaction by setting CSN low (negedge) throws an error.
- Any attempt at SPI transfer while the CSN is high throws an error, as this is an error in implementation. Transfers can happen only during CSN low (after CSN negedge).

Quirks:
- When determining size of the mocked L2 Request, you cannot use `sizeof()`, as some structures are universal for multiple types of arguments. For example, `lt_l2_get_info_rsp_t()` is actually not always `sizeof(lt_l2_get_info_rsp_t)` bytes long. As a consequence, you have to either determine size manually, or use `calc_mocked_resp_len()`, which will calculate length for you (including CRC).
  - Another side effect is that CRC is not always in the `crc` field, it can be in the `object` field if data are shorter.