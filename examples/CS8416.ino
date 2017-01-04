
#include <CS8416.h>
#include <Wire.h>

 CS8416 SPDIF; 

void setup()
{
   SPDIF.begin();
   delay(50);
   SPDIF.initiate();
   SPDIF.clockSwitch(true);
   SPDIF.changeInput(1);
}

void loop()
{
	delay(1000);
	SPDIF.muteOutput(false);
	delay(1000);
	SPDIF.muteOutput(true);   
}
