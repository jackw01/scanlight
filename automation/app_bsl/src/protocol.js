class Protocol {
  constructor() {
    this.UART_BAUD_RATE = 115200;
    this.PACKET_START = 254;

    // Host-to-device packets
    this.PKT_H2D_SET_COLOR = 0;
    this.PKT_H2D_GET_DEFAULT_RGB = 1;
    this.PKT_H2D_GET_FW_VERSION = 2;
    this.PKT_H2D_SHUTTER_PULSE = 3;
    this.PKT_H2D_DFU_MODE = 4;
    this.PKT_H2D_SET_TRIM = 5;
    this.PKT_H2D_GET_TRIM = 6;

    // Device-to-host packets
    this.PKT_D2H_ACK = 0;
    this.PKT_D2H_LED_TEMP = 1;
    this.PKT_D2H_VBUS = 2;
    this.PKT_D2H_FW_VERSION = 3;
    this.PKT_D2H_DEFAULT_RGB = 4;
    this.PKT_D2H_TRIM = 5;

    this.PACKET_MAX_LENGTH = 128;

    this.incomingPacketHeader = 0;
    this.incomingPacketBuffer = new ArrayBuffer(this.PACKET_MAX_LENGTH);
    this.incomingPacket = new Uint8Array(this.incomingPacketBuffer);
    this.packetIndex = 0;
    this.packetEnd = 0;

    this.callbacks = {};

    navigator.serial.addEventListener("connect", (event) => {
      console.log(event);
    });

    navigator.serial.addEventListener("disconnect", (event) => {
      console.log(event);
    });
  }

  async readUntilClosed() {
    while (this.port.readable) {
      this.reader = this.port.readable.getReader();
      try {
        while (true) {
          const { value, done } = await this.reader.read();
          if (done) {
            // reader.cancel() has been called.
            break;
          }
          //console.log(String.fromCharCode(...value));
          for (let i = 0; i < value.length; i++) {
            if (this.packetIndex == 0) {
              if (value[i] == this.PACKET_START) this.packetIndex++;
              else continue;
            } else if (this.packetIndex == 1) {
              this.incomingPacketHeader = value[i];
              this.packetIndex++;
            } else if (this.packetIndex == 2) {
              this.packetEnd = value[i] + 2;
              this.packetIndex++;
            } else if (this.packetIndex >= 3 && this.packetIndex < this.packetEnd && this.packetIndex - 3 < this.PACKET_MAX_LENGTH - 1) {
              this.incomingPacket[this.packetIndex - 3] = value[i];
              this.packetIndex++;
            } else if (this.packetIndex == this.packetEnd) {
              this.incomingPacket[this.packetIndex - 3] = value[i];
              this.incomingPacket[this.packetIndex - 2] = 0;
              this.handlePacket();
              this.packetIndex = 0;
            } else {
              this.packetIndex = 0;
            }
          }
        }
      } catch (error) {
        // Handle error...
        console.log(error);
      } finally {
        // Allow the serial port to be closed later.
        this.reader.releaseLock();
      }
    }
    await this.port.close();
  }

  handlePacket() {
    //console.log("received bytes: ", this.incomingPacketHeader, this.incomingPacket);
    if (this.incomingPacketHeader in this.callbacks) {
      this.callbacks[this.incomingPacketHeader](
        this.incomingPacketHeader,
        this.incomingPacket,
        new DataView(this.incomingPacket.buffer)
      );
    }
  }

  async sendPacket(header, packetData) {
    const data = new Uint8Array(packetData.length + 3);
    data[0] = this.PACKET_START;
    data[1] = header;
    data[2] = packetData.length;
    for (let i = 3; i < packetData.length + 3; i++) {
      data[i] = packetData[i - 3];
    }
    console.log("writing bytes: ", data);
    await this.writer.write(data);
  }

  async connect() {
    this.port = await navigator.serial.requestPort();
    await this.port.open({ baudRate: this.UART_BAUD_RATE });
    console.log(this.port.getInfo());
    this.writer = this.port.writable.getWriter();
    this.readUntilClosed();
  }

  addCallback(header, callback) {
    this.callbacks[header] = callback;
  }
}

export default new Protocol();
