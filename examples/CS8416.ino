#include <SoftReset.h>
#include <Timer.h>
#include <Event.h>
#include <si5351.h>
#include <TimerOne.h>
#include <ClickButton.h>
#include <PinChangeInterrupt.h>
#include <SPI_VFD.h>
#include <CS8416.h>
#include <Wire.h>

enum PinAssignments {
	encoderPinA = 3,
	encoderPinB = 4,
	clearButton = 8,
	int0Pin     = 3,
	int1Pin     = 2,
	rst         = 9
};

int InState = 0;

volatile unsigned int encoderPos = 0;
unsigned int lastReportedPos = 1;
unsigned int controlPos = 1;
static boolean rotating = false;

uint8_t brightness = 0;

boolean A_set = false;
boolean B_set = false;
boolean lock  = false;
boolean os    = false;
boolean mute  = false;
boolean tick = false;
boolean clksw = false;
boolean alwon = false;

unsigned int freq = 0;
unsigned int Menu  = 0;

unsigned long long Eight     =   819200000ULL;
unsigned long long Eleven    =  1128960000ULL;
unsigned long long Twelve    =  1228800000ULL;
unsigned long long pll0_freq = 61440000000ULL;
unsigned long long pll1_freq = 67737600000ULL;

static unsigned int MenuSelect;
static unsigned int InputSelect;
static unsigned int FilterSelect;
static unsigned int ClockSelect;
static unsigned int BrightSelect;
static unsigned int ResetDefaults;

const unsigned long PERIOD = 10000;
int DisplayEvent;

ClickButton btn(clearButton, LOW, CLICKBTN_PULLUP);
Si5351 CLKGEN;
SPI_VFD vfd(SCK, MOSI, SS, VFD_BRIGHTNESS25);
CS8416 SPDIF;
Timer t;

void doEncoderA(){
	if ( rotating ) delay (1);
	if( digitalRead(encoderPinA) != A_set ) {
		A_set = !A_set;
		if ( A_set && !B_set )
		encoderPos += 1;
		rotating = false;
	}
}

void doEncoderB(){
	if ( rotating ) delay (1);
	if( digitalRead(encoderPinB) != B_set ) {
		B_set = !B_set;
		if( B_set && !A_set )
		encoderPos -= 1;
		rotating = false;
	}
}

void setup() {
	
	CLKGEN.init(SI5351_CRYSTAL_LOAD_10PF, 0, 0);
	CLKGEN.set_clock_disable(SI5351_CLK0, SI5351_CLK_DISABLE_HI_Z);
	CLKGEN.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
	CLKGEN.pll_reset(SI5351_PLLA);
	CLKGEN.set_freq_manual(pll0_freq, Twelve, SI5351_CLK0);
	CLKGEN.output_enable(SI5351_CLK0, 1);
	
	vfd.begin(20, 2);
	vfd.setBrightness(2);
	vfd.setCursor(0, 0);
	
	pinMode(encoderPinA, INPUT_PULLUP);
	pinMode(encoderPinB, INPUT_PULLUP);

	btn.debounceTime   = 20;   // Debounce timer in ms
	btn.multiclickTime = 250;  // Time limit for multi clicks
	btn.longClickTime  = 1000; // time until "held-down clicks" register
	
	attachPinChangeInterrupt(digitalPinToPCINT(encoderPinA), doEncoderA, CHANGE);
	attachPinChangeInterrupt(digitalPinToPCINT(encoderPinB), doEncoderB, CHANGE);
	
	attachInterrupt(int0Pin, readErrors, CHANGE);
	
	digitalWrite(rst, LOW);
	Timer1.initialize(1500000);
	Timer1.attachInterrupt(activate);
	
	DisplayEvent = t.every(PERIOD, displayOff);
	
	SPDIF.begin();

	delay(50);
	vfd.clear();
	vfd.print("@ preheat");
	
	Serial.begin(19200);
	Serial.print("go");
}

void setHighZ(){
	CLKGEN.output_enable(SI5351_CLK0, 0);
}

void set819200(){
	CLKGEN.output_enable(SI5351_CLK0, 0);
	CLKGEN.pll_reset(SI5351_PLLA);
	CLKGEN.set_freq_manual(pll0_freq, Eight, SI5351_CLK0);
	CLKGEN.output_enable(SI5351_CLK0, 1);
	freq = 1;
}

void set112896(){
	CLKGEN.output_enable(SI5351_CLK0, 0);
	CLKGEN.pll_reset(SI5351_PLLA);
	CLKGEN.set_freq_manual(pll1_freq, Eleven, SI5351_CLK0);
	CLKGEN.output_enable(SI5351_CLK0, 1);
	freq = 2;
}
void set122880(){
	CLKGEN.output_enable(SI5351_CLK0, 0);
	CLKGEN.pll_reset(SI5351_PLLA);
	CLKGEN.set_freq_manual(pll0_freq, Twelve, SI5351_CLK0);
	CLKGEN.output_enable(SI5351_CLK0, 1);
	freq = 0;
}

void activate(){
	displayClearR();
	digitalWrite(rst, HIGH);
	delay(50);
	SPDIF.initiate();
}

void getInformation(){
	if (Menu == 0){
		lock = digitalRead(int0Pin);
		os   = digitalRead(int1Pin);
		vfd.setCursor(0, 0);

		clksw = (SPDIF.readRegister(CS8416_CTRL1) >> 7);

		if (!clksw){
			int val =  SPDIF.readRegister(CS8416_OMCLK_TO_RMCLK);
			if (val == 0x40) vfd.print("48 kHz");
			if (val == 0x60) vfd.print("32 kHz");
			if (val > 0x40 && val < 0x60) vfd.print("44.1 kHz");
		}
		else{
			if (freq == 0) vfd.print("48 kHz");
			if (freq == 1) vfd.print("32 kHz");
			if (freq == 2) vfd.print("44.1 kHz");
		}
		vfd.setCursor(0, 10);
		if (!os){
			vfd.print("1x");
		}
		else{
			vfd.print("2x");
		}
		vfd.setCursor(0, 14);
		if(!lock){
			vfd.print(" UNLOCK");
		}
		else{
			vfd.print(" LOCKED");
		}
		vfd.setCursor(1, 0);
	}

}

void readErrors(){

}

void displayClearR(){
	for (int positionCounter = 0; positionCounter < 30; positionCounter++) {
		// scroll one position left:
		vfd.scrollDisplayRight();
		// wait a bit:
		delay(10);
	}
}

void displayClearL(){
	for (int positionCounter = 0; positionCounter < 30; positionCounter++) {
		// scroll one position left:
		vfd.scrollDisplayLeft();
		// wait a bit:
		delay(10);
	}
}

void displayOff(){
	if (controlPos != encoderPos) {
		controlPos = encoderPos;
	}
	else {
		if(!alwon){
			vfd.noDisplay();
		}
		else{
			vfd.display();
		}
	}
}

void loop()
{
	wdt_enable(WDTO_4S);

	btn.Update();
	t.update();
	vfd.setBrightness(brightness);
	if (btn.clicks != 0) InState = btn.clicks;
	rotating = true;
	if (lastReportedPos != encoderPos) {
		Serial.print("Index:");
		Serial.println(encoderPos, DEC);
		lastReportedPos = encoderPos;
		vfd.display();

	}
	// if(btn.clicks == 1) Serial.println(Menu, DEC);
	if (Menu == 0){
		getInformation();
		if(btn.clicks == 1){
			Menu = 1;
			displayClearR();
			encoderPos = 0;
			controlPos = 1;
			Menu = 0;
		}
	}
	if (Menu == 1){

		vfd.setCursor(0, 0);
		vfd.println("Menu Select"); Serial.println("Menu Select");
		vfd.setCursor(1, 0);
		encoderPos = 1;

		switch (MenuSelect)
		{

			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				InputSelect = 1;
			}
			break;case 1:
			vfd.print("Input Selection");Serial.println("Input Selection");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 5 && encoderPos <=9){
				InputSelect = 2;
			}
			break;
			case 2:
			vfd.print("Filter Selection");Serial.println("Filter Selection");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				SPDIF.changeInput(1);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 10 && encoderPos <=14){
				InputSelect = 3;
			}
			break;
			case 3:
			vfd.print("Reclocking");Serial.println("Reclocking");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				SPDIF.changeInput(2);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 15 && encoderPos <=19){
				InputSelect = 4;
			}
			break;
			case 4:
			vfd.print("Optical 2");Serial.println("Optical 2");
			if(btn.clicks == 1){
				SPDIF.changeInput(3);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 20 ){
				encoderPos = 0;
				controlPos = 1;
				InputSelect = 1;
			}
			break;
		}
	}

	if (Menu == 2){

		vfd.setCursor(0, 0);
		vfd.println("Input Select"); Serial.println("Input Select");
		vfd.setCursor(1, 0);

		switch (InputSelect)
		{

			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				InputSelect = 1;
			}case 1:
			vfd.print("Coaxial 1");Serial.println("Coaxial 1");
			if(btn.clicks == 1){
				SPDIF.changeInput(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 5 && encoderPos <=9){
				InputSelect = 2;
			}
			break;
			case 2:
			vfd.print("Coaxial 2");Serial.println("Coaxial 2");
			if(btn.clicks == 1){
				SPDIF.changeInput(1);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 10 && encoderPos <=14){
				InputSelect = 3;
			}
			break;
			case 3:
			vfd.print("Optical 1");Serial.println("Optical 1");
			if(btn.clicks == 1){
				SPDIF.changeInput(2);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 15 && encoderPos <=19){
				InputSelect = 4;
			}
			break;
			case 4:
			vfd.print("Optical 2");Serial.println("Optical 2");
			if(btn.clicks == 1){
				SPDIF.changeInput(3);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 20 ){
				encoderPos = 0;
				controlPos = 1;
				InputSelect = 1;
			}
			break;
		}
	}

	if (Menu == 3){

		vfd.setCursor(0, 0);
		vfd.println("Filter Select");
		vfd.setCursor(1, 0);

		switch (FilterSelect)
		{
			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				FilterSelect = 1;
			}case 1:
			vfd.print("Off direct");
			if(btn.clicks == 1){
				SPDIF.emphFilter(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 5 && encoderPos <=9){
				FilterSelect = 2;
			}
			break;
			case 2:
			vfd.print("32 kHz");
			if(btn.clicks == 1){
				SPDIF.emphFilter(1);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 10 && encoderPos <=14){
				FilterSelect = 3;
			}
			break;
			case 3:

			vfd.print("44.1 kHz");
			if(btn.clicks == 1){
				SPDIF.emphFilter(2);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 15 && encoderPos <=19){
				FilterSelect = 4;
			}
			break;
			case 4:
			vfd.print("48 kHz");
			if(btn.clicks == 1){
				SPDIF.emphFilter(3);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 20 && encoderPos <=24){
				FilterSelect = 5;
			}
			break;
			case 5:
			vfd.print("AUTO 50us/15us");
			if(btn.clicks == 1){
				SPDIF.emphFilter(4);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 29 ){
				encoderPos = 0;
				controlPos = 1;
				FilterSelect = 1;
			}
			break;
		}
	}

	if (Menu == 4){

		vfd.setCursor(0, 0);
		vfd.println("Reclocking");
		vfd.setCursor(1, 0);

		switch (ClockSelect)
		{
			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				ClockSelect = 1;
			}
			case 1:
			vfd.print("Off");
			if(btn.clicks == 1){
				//setHighZ();
				set122880();
				SPDIF.clockSwitch(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 5 && encoderPos <=9){
				ClockSelect = 2;
			}
			break;
			case 2:
			vfd.print("8.192 MHz");
			if(btn.clicks == 1){
				SPDIF.muteOutput(1);
				set819200();
				SPDIF.clockSwitch(1);
				set819200();
				SPDIF.muteOutput(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 10 && encoderPos <=14){
				ClockSelect = 3;
			}
			break;
			case 3:
			vfd.print("11.2896 kHz");
			if(btn.clicks == 1){
				SPDIF.muteOutput(1);
				SPDIF.clockSwitch(1);
				set112896();
				SPDIF.muteOutput(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 15 && encoderPos <=19){
				ClockSelect = 4;
			}
			break;
			case 4:
			vfd.print("12.288 kHz");
			if(btn.clicks == 1){
				SPDIF.muteOutput(1);
				SPDIF.clockSwitch(1);
				set122880();
				SPDIF.muteOutput(0);
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 20){
				encoderPos = 0;
				controlPos = 1;
				ClockSelect = 1;
			}
			break;
		}
	}

	if (Menu == 5){
		vfd.setCursor(0, 0);
		vfd.println("Set Brightness");
		vfd.setCursor(1, 0);

		switch (BrightSelect)
		{
			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				BrightSelect = 1;
			}
			break;

			case 1:
			vfd.print("100%");
			if(btn.clicks == 1){
				brightness = 0;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 5 && encoderPos <=9){
				BrightSelect = 2;
			}
			break;

			case 2:
			vfd.print("75%");
			if(btn.clicks == 1){
				brightness = 1;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 10 && encoderPos <=14){
				BrightSelect = 3;
			}
			break;

			case 3:
			vfd.print("50%");
			if(btn.clicks == 1){
				brightness = 2;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 15 && encoderPos <=19){
				BrightSelect = 4;
			}
			break;

			case 4:
			vfd.print("25%");
			if(btn.clicks == 1){
				brightness = 3;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 20 && encoderPos <=24){
				BrightSelect = 5;
			}
			break;

			case 5:
			vfd.print("Always On");
			if(btn.clicks == 1){
				alwon = true;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos >= 25 && encoderPos <=29){
				encoderPos = 0;
				controlPos = 1;
				BrightSelect = 1;
			}
			break;

			case 6:
			vfd.print("Auto Off");
			if(btn.clicks == 1){
				alwon = false;
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 30){
				encoderPos = 0;
				controlPos = 1;
				BrightSelect = 1;
			}
			break;
		}
	}
	if (Menu == 6){

		vfd.setCursor(0, 0);
		vfd.println("Reset to Defaults");
		vfd.setCursor(1, 0);

		switch (ResetDefaults)
		{
			case 0:
			vfd.print("Back");Serial.println("IBack");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				encoderPos = 0;
				controlPos = 1;
				Menu = 0;
			}
			if (encoderPos > 0 ){
				ResetDefaults = 1;
			}
			break;

			case 1:
			vfd.print("Reset to defaults!!");
			//todo readout and display current value on row 1
			if(btn.clicks == 1){
				displayClearR();
				soft_restart();
			}
			if (encoderPos > 10 ){
				ResetDefaults = 0;
				encoderPos = 0;
				controlPos = 1;
			}
			break;
		}
	}

	if(InState == 2) {

	}
	if(InState == -1) {
		mute = !mute;
		SPDIF.muteOutput(mute);
	}
}
