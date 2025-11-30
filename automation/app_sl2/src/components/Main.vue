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
          </v-card-text>
        </v-card>
        <br />
        <v-card>
          <v-card-title>Manual Control</v-card-title>
          <v-card-text>
            <v-number-input
              label="Red brightness"
              v-model="red"
              :min="0"
              :max="100"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="red"
              color="red"
              :min="0"
              :max="100"
              @end="update"
            ></v-slider>
            <v-number-input
              label="Green brightness"
              v-model="green"
              :min="0"
              :max="100"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="green"
              color="green"
              :min="0"
              :max="100"
              @end="update"
            ></v-slider>
            <v-number-input
              label="Blue brightness"
              v-model="blue"
              :min="0"
              :max="100"
              @update:modelValue="update"
            ></v-number-input>
            <v-slider
              v-model="blue"
              color="blue"
              :min="0"
              :max="100"
              @end="update"
            ></v-slider>
            <v-row class="mb-1">
              <v-col>
                <v-btn
                  color="red"
                  elevation="2"
                  x-large
                  @click="setScaling([1, 0, 0])">R</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="green"
                  elevation="2"
                  x-large
                  @click="setScaling([0, 1, 0])">G</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="blue"
                  elevation="2"
                  x-large
                  @click="setScaling([0, 0, 1])">B</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="grey-lighten-4"
                  elevation="2"
                  x-large
                  @click="setScaling([1, 1, 1])">⏽</v-btn>
              </v-col>
            </v-row>
            <v-row>
              <v-col>
                <v-btn
                  color="cyan"
                  elevation="2"
                  x-large
                  @click="setScaling([0, 1, 1])">C</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="pink"
                  elevation="2"
                  x-large
                  @click="setScaling([1, 0, 1])">M</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="yellow"
                  elevation="2"
                  x-large
                  @click="setScaling([1, 1, 0])">Y</v-btn>
              </v-col>
              <v-col>
                <v-btn
                  color="black"
                  elevation="2"
                  x-large
                  @click="setScaling([0, 0, 0])">◯</v-btn>
              </v-col>
            </v-row>
          </v-card-text>
        </v-card>

      </v-col>
      <v-col cols="6">
        <v-card>
          <v-card-title>Presets</v-card-title>
          <v-card-text>
            <v-select
              v-model="selectedPresetName"
              :items="presets.map(p => p.name)"
              label="Select Preset"
              dense
              outlined
              @update:modelValue="loadPreset"
            ></v-select>
            <v-btn class="mr-2" color="primary" @click="loadPreset" :disabled="!selectedPresetName">Load</v-btn>
            <v-btn class="mr-2" color="primary" @click="createPreset">Create</v-btn>
            <v-btn class="mr-2" color="primary" @click="renamePreset" :disabled="!selectedPresetName">Rename</v-btn>
            <v-btn  color="primary" @click="deletePreset" :disabled="!selectedPresetName">Delete</v-btn>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import serial from "../serial";

export default {
  name: "Main",
  data() {
    return {
      connected: false,
      red: 100,
      green: 100,
      blue: 100,
      scaling: [1, 1, 1],
      presets: [],
      selectedPresetName: null,
    };
  },
  mounted() {
    this.loadPresetsFromStorage();
  },
  methods: {
    async connect() {
      await serial.connect(115200);
      this.connected = true;
      this.red = 100;
      this.green = 100;
      this.blue = 100;
      this.scaling = [1, 1, 1];
      this.update();
    },
    setScaling(scaling) {
      this.scaling = scaling;
      this.update();
    },
    onChange(event) {
      console.log(event.target.value)
    },
    update() {
      const new_r = this.red * this.scaling[0] * 255 / 100;
      const new_g = this.green * this.scaling[1] * 255 / 100;
      const new_b = this.blue * this.scaling[2] * 255 / 100;
      serial.setColor(new_r, new_g, new_b);
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
    }
  },
  components: {
  },
};
</script>
