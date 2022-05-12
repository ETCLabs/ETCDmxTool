// Script to read voltage measurements
// From a Fluke 45 Compatible voltage meter
// based on DMX level

// Tom Steer, 3/15/2022

// Enable DMX output
dmx.enable();


for (var i = 0; i <= 255; i++) {
	// Ramp channel 1 from 0 to 255
	dmx.setLevel(0, i);
	// Wait 0.5s for reading to stabilize
	utils.sleep(500);
	// Issue measure command
	serial.writeText("MEAS?\r\n");
	utils.sleep(200);
	console.log("," + i + "," + serial.readText().split(/\r?\n/)[0]);
}