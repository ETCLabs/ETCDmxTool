// Script to turn a DMX channel on and off
// until the string "ERROR" is seen on the serial port

// Tom Steer, April 2022

// Enable DMX output
dmx.enable();

var foundError = false;
var outputOn = false;

while (!foundError) {
    // Flip channel
    outputOn = !outputOn;
    dmx.setLevel(0, outputOn ? 255 : 0);

    // Wait 5s for reading to stabilize
    utils.sleep(5000);

    // Check for error
    if (serial.readText().includes("ERROR")) {
        console.log("Error reported - stopping test");
        foundError = true;
    }
}