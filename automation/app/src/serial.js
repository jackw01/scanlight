class SerialInterface {
  constructor() {
    this.connected = false;

    this.packetType = 0;
    this.incomingPacket = [];
    this.packetIndex = 0;

    this.PacketStartByte = 254;
    this.PacketEndByte = 255;

    this.commands = {
      set: 0,
      reset: 1,
    };

    this.messageTypes = {
    };

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
              if (value[i] == this.PacketStartByte) this.packetIndex++;
              else continue;
            } else if (this.packetIndex == 1) {
              this.packetType = value[i];
              this.packetIndex++;
            } else if (this.packetIndex >= 2 && this.packetIndex <= 5) {
              this.incomingPacket.push(value[i]);
              this.packetIndex++;
            } else if (this.packetIndex == 6) {
              if (value[i] == this.PacketEndByte) {
                await this.handlePacket();
                this.incomingPacket = [];
                this.packetIndex = 0;
              } else continue;
            }
          }
        }
      } catch (error) {
        // Handle error...
      } finally {
        // Allow the serial port to be closed later.
        this.reader.releaseLock();
      }
    }
    await this.port.close();
  }

  async handlePacket() {
    let value = this.incomingPacket;
    if (this.packetType in this.callbacks) {
      this.callbacks[this.packetType](value);
    }
  }

  async sendCommand(command) {
    console.log("command: ", command);
    const data = new Uint8Array([
      this.PacketStartByte,
      this.commands[command], // CmdType
      0, // 0 bytes long
      this.PacketEndByte,
    ]);
    console.log("writing bytes: ", data);
    await this.writer.write(data);
  }

  async setColor(r, g, b) {
    const data = new Uint8Array([
      this.PacketStartByte,
      this.commands.set, // CmdType
      3, // 9 bytes long
      parseInt(r) & 0xff,
      parseInt(g) & 0xff,
      parseInt(b) & 0xff,
      this.PacketEndByte,
    ]);
    console.log("writing bytes: ", data);
    await this.writer.write(data);
  }

  async connect(baudRate) {
    this.port = await navigator.serial.requestPort();
    await this.port.open({ baudRate: baudRate });
    console.log(this.port.getInfo());
    this.writer = this.port.writable.getWriter();
    this.readUntilClosed();
  }

  addCallback(messageType, callback) {
    this.callbacks[messageType] = callback;
  }
}

export default new SerialInterface();
