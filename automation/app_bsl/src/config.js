export default {
  USBVBUSThreshold5V: 4000,
  USBVBUSThreshold9V: 8000,
  OverTemperatureThresholdMdegc: 77000,
  FWVersionStrings: {
    0: "v1.0.0",
    1: "v1.1.0",
    2: "v1.2.0",
  },
  LatestFWVersionID: 0,
  HWVersionStrings: {
    0: "big scanlight v1",
    1: "scanlight v4a",
    2: "scanlight v2/v3",
    3: "scanlight v4b",
  },
  SequenceRGB: [
    [1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0],
    [0, 0, 1, 0, 0],
  ],
  SequenceRGBIR: [
    [1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0],
    [0, 0, 1, 0, 0],
    [0, 0, 0, 0, 1],
  ],
  SequenceNWIR: [
    [1, 1, 1, 0, 0],
    [0, 0, 0, 0, 1],
  ],
  SequenceBWIR: [
    [0, 0, 0, 1, 0],
    [0, 0, 0, 0, 1],
  ],
};
