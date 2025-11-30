<template>
  <v-container>
    <v-row>
      <v-col cols="6">
        <v-card>
          <v-card-title>Scanlight</v-card-title>
          <v-card-text>
            <v-btn @click="connect" :disabled="connected" color="primary">
              Connect
            </v-btn>
            <v-alert v-if="connectionError" class="mt-4" title="Connection Failed" text="Disconnect and reconnect the USB cable and try again" type="error" variant="tonal"></v-alert>
            <v-alert v-if="connected && controlsDisabled" class="mt-4" title="Connect Power Cable" text="Connect the other USB port to a power supply" type="warning" variant="tonal"></v-alert>
          </v-card-text>
        </v-card>
        <br />
        <v-card :disabled="controlsDisabled">
          <v-card-title>Manual Control</v-card-title>
          <v-card-text>
            <v-number-input
              label="Red brightness"
              v-model="red"
              :min="0"
              :max="255"
              variant="outlined"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="red"
              color="red"
              :min="0"
              :max="255"
              @end="update"
            ></v-slider>
            <v-number-input
              label="Green brightness"
              v-model="green"
              :min="0"
              :max="255"
              variant="outlined"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="green"
              color="green"
              :min="0"
              :max="255"
              @end="update"
            ></v-slider>
            <v-number-input
              label="Blue brightness"
              v-model="blue"
              :min="0"
              :max="255"
              variant="outlined"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="blue"
              color="blue"
              :min="0"
              :max="255"
              @end="update"
            ></v-slider>
            <v-row justify="space-between">
              <v-col>
                <v-btn
                  block
                  color="cyan-lighten-3"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([1, 1, 1, 0, 0])">RGB</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  block
                  color="amber-lighten-3"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([0, 0, 0, 1, 0])">WHITE</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  block
                  color="black"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([0, 0, 0, 0, 0])">OFF</v-btn>
              </v-col>
            </v-row>
            <v-row justify="space-between">
              <v-col>
                <v-btn
                  block
                  color="red"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([1, 0, 0, 0, 0])">R</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  block
                  color="green"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([0, 1, 0, 0, 0])">G</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  block
                  color="blue"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([0, 0, 1, 0, 0])">B</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  block
                  color="pink-darken-4"
                  elevation="2"
                  x-large
                  @click="setEnabledChannels([0, 0, 0, 0, 1])">IR</v-btn>
              </v-col>
            </v-row>
            <v-alert v-if="showReducedPowerWarning" class="mt-4" text="Operating at reduced brightness. Use a USB-C power source with 9V 15W or higher output for full brightness." type="info" variant="tonal"></v-alert>
          </v-card-text>
        </v-card>
      </v-col>
      <v-col cols="6">
        <v-card>
          <v-card-title>RGB Presets</v-card-title>
          <v-card-text class="pb-2">
            <v-select
              v-model="selectedPresetName"
              :items="presets.map(p => p.name)"
              label="Select Preset"
              variant="outlined"
              @update:modelValue="loadPreset"
            ></v-select>
            <v-btn class="mr-2 mb-2" color="primary" @click="loadPreset" :disabled="!selectedPresetName">Load</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="createPreset">Create</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="renamePreset" :disabled="!selectedPresetName">Rename</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="deletePreset" :disabled="!selectedPresetName">Delete</v-btn>
            <v-dialog v-model="dialog" max-width="400" persistent>
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn :disabled="controlsDisabled" class="mr-2 mb-2" color="primary" v-bind="activatorProps">Set as default</v-btn>
              </template>
              <v-card
                title="Overwrite default?"
                text="The current RGB setting will be stored to the light's memory and will be used at startup, or whenever the light is not connected to a computer."
              >
                <template v-slot:actions>
                  <v-spacer></v-spacer>
                  <v-btn @click="dialog = false">
                    Cancel
                  </v-btn>
                  <v-btn class="text-accent" @click="dialog = false">
                    OK
                  </v-btn>
                </template>
              </v-card>
            </v-dialog>
            <v-btn :disabled="controlsDisabled" class="mr-2 mb-2" color="primary" @click="loadDefault">Load default</v-btn>
          </v-card-text>
        </v-card>
        <br />
        <v-card :disabled="controlsDisabled">
          <v-card-title>Automation</v-card-title>
          <v-card-text class="pb-2">
            <v-number-input
              label="Shutter Pulse Length (s)"
              v-model="shutterPulseLength"
              :min="0.05"
              :max="0.2"
              :step="0.01"
              :precision="2"
              variant="outlined"
            ></v-number-input>
            <v-number-input
              label="Post-shutter Delay (s)"
              v-model="postShutterDelay"
              :min="0.1"
              :max="2.0"
              :step="0.01"
              :precision="2"
              variant="outlined"
            ></v-number-input>
            <v-btn class="mr-2 mb-2" color="primary" @click="">Auto R,G,B</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="">Auto R,G,B,IR</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="">Auto RGB,IR</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="">Auto W,IR</v-btn>
            <v-btn class="mr-2 mb-2" color="primary" @click="">Shutter</v-btn>
          </v-card-text>
        </v-card>
        <br />
        <v-card>
          <v-card-title>Info</v-card-title>
          <v-card-text>
            <div class="text-grey-darken-1 text-caption">
              <span class="mr-4">Input voltage: {{ inputVoltageV }}V</span><span v-if="false" class="mr-4">LED temperature: {{ ledTemperatureC }}C</span><span class="mr-4">Current firmware version: {{ fwVersionString }}</span>
              <v-alert v-if="fwUpdateAvailable" class="mt-4" title="Firmware Update Available" text="" type="info" variant="tonal">View details and download <a class="text-accent" href='https://github.com/jackw01/scanlight/releases'>here</a></v-alert>
              <br />
              <a class="text-accent" href="https://jackw01.github.io/">big scanlight user manual</a>
              <br />
              big scanlight control app v1.0 by jackw01
            </div>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import protocol from "../protocol";
import config from "../config";

export default {
  name: "Main",
  data() {
    return {
      connected: false,
      connectionError: false,
      red: 255,
      green: 255,
      blue: 255,
      enabledChannels: [0, 0, 0, 0, 0],
      presets: [],
      selectedPresetName: null,
      shutterPulseLength: 0.1,
      postShutterDelay: 0.3,
      inputVoltageMv: 5000,
      ledTemperatureMdegc: 0,
      fwUpdateAvailable: false,
      fwVersionString: "",
      dialog: false,
    };
  },
  computed: {
    controlsDisabled: function() {
      return !(this.connected && this.inputVoltageMv > config.USBVBUSThreshold5V);
    },
    inputVoltageV: function() {
      return (this.inputVoltageMv / 1000).toFixed(2);
    },
    ledTemperatureC: function() {
      return (this.ledTemperatureMdegc / 1000).toFixed(2);
    },
    showReducedPowerWarning: function() {
      return (this.inputVoltageMv > config.USBVBUSThreshold5V && this.inputVoltageMv < config.USBVBUSThreshold9V);
    },
  },
  mounted() {
    this.loadPresetsFromStorage();
    console.log(this.controlsEnabled);
  },
  methods: {
    async connect() {
      try {
        await protocol.connect();
        this.connected = true;
        this.connectionError = false;
        protocol.addCallback(protocol.PKT_D2H_FW_VERSION, this.checkFWVersion);
        protocol.addCallback(protocol.PKT_D2H_LED_TEMP, this.updateLEDTemp);
        protocol.addCallback(protocol.PKT_D2H_VBUS, this.updateVbus);
        protocol.addCallback(protocol.PKT_D2H_DEFAULT_RGB, this.updateRGB);
        protocol.sendPacket(protocol.PKT_H2D_GET_FW_VERSION, []);
        protocol.sendPacket(protocol.PKT_H2D_GET_DEFAULT_RGB, []);
        this.update();
      } catch (e) {
        this.connectionError = true;
      }
    },
    setEnabledChannels(ch) {
      this.enabledChannels = ch;
      this.update();
    },
    onChange(event) {
      console.log(event.target.value);
    },
    checkFWVersion(header, rawData, dataView) {
      const versionId = dataView.getInt32(0);
      this.fwVersionString = config.FWVersionStrings[versionId];
      if (versionId > config.LatestFWVersionID) this.fwUpdateAvailable = true;
    },
    updateLEDTemp(header, rawData, dataView) {
      this.ledTemperatureMdegc = dataView.getInt32(0);
    },
    updateVbus(header, rawData, dataView) {
      this.inputVoltageMv = dataView.getInt32(0);
    },
    updateRGB(header, rawData, dataView) {
      this.red = dataView.getUint8(0);
      this.green = dataView.getUint8(1);
      this.blue = dataView.getUint8(2);
    },
    update() {
      if (!this.connected) return;
      const new_color = [
        this.red * this.enabledChannels[0],
        this.green * this.enabledChannels[1],
        this.blue * this.enabledChannels[2],
        255 * this.enabledChannels[3],
        255 * this.enabledChannels[4],
        0, // save preset flag
      ];
      protocol.sendPacket(protocol.PKT_H2D_SET_COLOR, new_color.map(x => parseInt(x) & 0xff));
    },
    loadPresetsFromStorage() {
      const stored = localStorage.getItem("rgb_presets");
      this.presets = stored ? JSON.parse(stored) : [];
    },
    savePresetsToStorage() {
      localStorage.setItem("rgb_presets", JSON.stringify(this.presets));
    },
    createPreset() {
      const name = prompt("Enter a name for the new preset:");
      if (!name) return;
      this.presets.push({ name, red: this.red, green: this.green, blue: this.blue });
      this.savePresetsToStorage();
      this.selectedPresetName = name;
    },
    renamePreset() {
      const idx = this.presets.findIndex(p => p.name === this.selectedPresetName);
      if (idx === -1) return;
      const newName = prompt("Enter a new name:", this.presets[idx].name);
      if (!newName) return;
      this.presets[idx].name = newName;
      this.savePresetsToStorage();
      this.selectedPresetName = newName;
    },
    deletePreset() {
      const idx = this.presets.findIndex(p => p.name === this.selectedPresetName);
      if (idx === -1) return;
      if (!confirm(`Delete preset "${this.selectedPresetName}"?`)) return;
      this.presets.splice(idx, 1);
      this.savePresetsToStorage();
      this.selectedPresetName = null;
    },
    loadPreset() {
      const preset = this.presets.find(p => p.name === this.selectedPresetName);
      if (!preset) return;
      this.red = preset.red;
      this.green = preset.green;
      this.blue = preset.blue;
      this.update();
    },
    loadDefault() {
      protocol.sendPacket(protocol.PKT_H2D_GET_DEFAULT_RGB, []);
    }
  },
  components: {
  },
};
</script>
