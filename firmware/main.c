#include <atmel_start.h>

enum 
{
	kModeSequential=0,
	kModeBounce=1,
	kModeInverted=2,
	
	kModeLast
};

enum
{
	kCounterModeLinear=0,
	kCounterModeExponential,
	kCounterModeFibonacci,
	
	kCounterModeLast
};

typedef enum
{
	kForwards,
	kBackwards
	
} BounceDirection;
	
const int kLongHoldTicks = 20000;
const int kSaveSettingsDelay = 500000;
const int kDebounceTicks = 1000;
const int kDebounceSignalTicks = 1000;
#define kExponentialSize  10
const int32_t kExponential[kExponentialSize] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
#define kFibonacciSize  18
const int32_t kFibonacci[kFibonacciSize] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597};

volatile int64_t gTicks=0;
volatile int8_t gChannel1=false;
volatile int8_t gChannel2=false;
volatile int8_t gChannel3=false;
volatile int8_t gChannel4=false;
volatile int8_t gLinearChannelIndex=0;
volatile BounceDirection gBounceDirection=kForwards;
volatile bool gSettingsDirty=false;
volatile bool gInvertChannels=false;
volatile int32_t gCounter=0;


volatile int8_t gSwitcheeMode=kModeSequential;
volatile int8_t gCounterMode=kCounterModeLinear;

static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
	++gTicks;
}

static struct timer_task TIMER_0_task1;
void setupTimer(void)
{
	TIMER_0_task1.interval = 2;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_start(&TIMER_0);
}


void setCounterModeIndicators()
{
	bool D10=false;
	bool D11=false;
	
	switch (gCounterMode)
	{
		case kCounterModeLinear:
			D10 = true;
			break;
		case kCounterModeExponential:
			D11 = true;
			break;
		case kModeInverted:
			D10 = true;
			D11 = true;
			break;
	}
	
	gpio_set_pin_level(LED10, D10);
	gpio_set_pin_level(LED11, D11);
}


void incrementCounterMode()
{
	gCounterMode = (1+gCounterMode) % kCounterModeLast;
	setCounterModeIndicators();
	
	gSettingsDirty = true;
	gCounter = 0;
}

void checkCounterModeButton()
{
	static volatile bool sLastCounterMode=false;
	static volatile int64_t sLastCounterModeTicks=0;
	
	bool input = !gpio_get_pin_level(SW_OUT);
	
	if (input != sLastCounterMode)
	{
		if ((gTicks-sLastCounterModeTicks) > kDebounceTicks)
		{
			if (input)
			{
				incrementCounterMode();
			}
			
			sLastCounterModeTicks = gTicks;
		}
		sLastCounterMode = input;
	}
}

void incrementMode()
{
	gSwitcheeMode = (1+gSwitcheeMode) % kModeLast;
	gInvertChannels = (gSwitcheeMode == kModeInverted);
	setModeIndicators();
	
	gSettingsDirty = true;
	gLinearChannelIndex = 0;
	gCounter = 0;
	clearChannels();
}

void setModeIndicators()
{
	bool D8=false;
	bool D9=false;
	
	switch (gSwitcheeMode)
	{
		case kModeSequential:
			D8 = true;
			break;
		case kModeBounce:
			D9 = true;
			break;
		case kCounterModeFibonacci:
			D8 = true;
			D9 = true;
			break;
	}
	
	gpio_set_pin_level(LED8, D8);
	gpio_set_pin_level(LED9, D9);
}

void flashModeIndicators()
{
	gpio_set_pin_level(LED8, true);
	gpio_set_pin_level(LED9, true);	
	delay_ms(100);
	gpio_set_pin_level(LED8, false);
	gpio_set_pin_level(LED9, false);
	delay_ms(100);
	gpio_set_pin_level(LED8, true);
	gpio_set_pin_level(LED9, true);
	delay_ms(100);
	
	setModeIndicators();	// Restore the proper state
}

void flashCounterModeIndicators()
{
	gpio_set_pin_level(LED10, true);
	gpio_set_pin_level(LED11, true);
	delay_ms(100);
	gpio_set_pin_level(LED10, false);
	gpio_set_pin_level(LED11, false);
	delay_ms(100);
	gpio_set_pin_level(LED10, true);
	gpio_set_pin_level(LED11, true);
	delay_ms(100);
	
	setCounterModeIndicators();	// Restore the proper state
}



void checkModeButton()
{
	static volatile bool sLastSWModePress=false;
	static volatile int64_t sLastSWModePressTicks=0;
	static volatile int64_t sLastSWModeLongholdTicks=0;
	
	bool swModePress = !gpio_get_pin_level(SW_MODE);
	
	if (swModePress != sLastSWModePress)
	{
		if ((gTicks-sLastSWModePressTicks) > kDebounceTicks)
		{
			if (!swModePress)	// Do this on button release
			{
				if ((gTicks-sLastSWModeLongholdTicks) > kLongHoldTicks)
				{
					flashModeIndicators();
					reset();
				}
				else
				{
					incrementMode();
				}
				sLastSWModeLongholdTicks = 0;
			}
			else
			{
				sLastSWModeLongholdTicks = gTicks;
			}
		
			sLastSWModePressTicks = gTicks;
		}
		sLastSWModePress = swModePress;
	}
}

void clearChannels()
{
	gChannel1 = gChannel2 = gChannel3 = gChannel4 = false;
	setChannels();
}

void setChannels()
{
	gpio_set_pin_level(OUT1, gInvertChannels ? !gChannel1 : gChannel1);
	gpio_set_pin_level(OUT2, gInvertChannels ? !gChannel2 : gChannel2);
	gpio_set_pin_level(OUT3, gInvertChannels ? !gChannel3 : gChannel3);
	gpio_set_pin_level(OUT4, gInvertChannels ? !gChannel4 : gChannel4);
}

void increment()
{
	++gCounter;
	
	switch (gCounterMode)
	{
		case kCounterModeLinear:
			gLinearChannelIndex = (1+gLinearChannelIndex) % 5;
			break;
		case kCounterModeExponential:
			for (auto i=0 ; i < kExponentialSize ; ++i)
			{
				if (gCounter == kExponential[i])
				{
					gLinearChannelIndex = (1+gLinearChannelIndex) % 5;
					
					if (i == kExponentialSize-1)
					{
						gCounter=0;
					}
					break;
				}
			}
			break;
		case kCounterModeFibonacci:
			for (auto i=0 ; i < kFibonacciSize ; ++i)
			{
				if (gCounter == kFibonacci[i])
				{
					gLinearChannelIndex = (1+gLinearChannelIndex) % 5;
					return;
				}
			}
			
			gCounter=0;
			break;
	}
}


void decrement()
{
	--gCounter;
	
	switch (gCounterMode)
	{
		case kCounterModeLinear:
			--gLinearChannelIndex;
			break;
		case kCounterModeExponential:
			for (auto i=0 ; i < kExponentialSize ; ++i)
			{
				if (gCounter == kExponential[i])
				{
					--gLinearChannelIndex;
					if (i == kExponentialSize-1)
					{
						gCounter=0;
					}
					break;
				}
			}
			break;
		case kCounterModeFibonacci:
			for (auto i=0 ; i < kFibonacciSize ; ++i)
			{
				if (gCounter == kFibonacci[i])
				{
					--gLinearChannelIndex;
					return;
				}
			}
		
			gCounter=0;
			break;
	}
}


void incrementLinear()
{
	increment();
	
	
	gChannel1 = gChannel2 = gChannel3 = gChannel4 = false;
	
	switch (gLinearChannelIndex)
	{
		case 0:
			// All are turned off.
			break;
		case 1:
			gChannel1 = true;
			break;
		case 2:
			gChannel2 = true;
			break;
		case 3:
			gChannel3 = true;
			break;
		case 4:
			gChannel4 = true;
			break;
	}
}

void incrementBounce()
{
	if (kForwards == gBounceDirection && gLinearChannelIndex==4)
		gBounceDirection = kBackwards;
	else if (kBackwards == gBounceDirection && gLinearChannelIndex==0)
		gBounceDirection = kForwards;
	
	if (kForwards == gBounceDirection)
	{
		increment();
	}
	else if (kBackwards == gBounceDirection)
	{
		decrement();
	}
	
	
	gChannel1 = gChannel2 = gChannel3 = gChannel4 = false;
	
	switch (gLinearChannelIndex)
	{
		case 0:
		// All are turned off.
		break;
		case 1:
		gChannel1 = true;
		break;
		case 2:
		gChannel2 = true;
		break;
		case 3:
		gChannel3 = true;
		break;
		case 4:
		gChannel4 = true;
		break;
	}
}

void trigger()
{
	switch (gSwitcheeMode)
	{
		case kModeSequential:
		case kModeInverted:
			incrementLinear();
			break;
		case kModeBounce:
			incrementBounce();
			break;
	}
	
	setChannels();
}


void reset()
{
	gBounceDirection = kForwards;
	gLinearChannelIndex = 0;
	gChannel1 = gChannel2 = gChannel3 = gChannel4 = false;
	gCounter = 0;
	
	setChannels();
}

void checkTrigger()
{
	static volatile bool sLastTrigger=false;
	static volatile int64_t sLastTriggerTicks=0;
	
	bool input = !gpio_get_pin_level(IN_TRIGGER);
	
	if (input != sLastTrigger)
	{
		if ((gTicks-sLastTriggerTicks) > kDebounceSignalTicks)
		{
			if (input)
			{
				trigger();
			}
			
			sLastTriggerTicks = gTicks;
		}
		sLastTrigger = input;
	}
}



void checkReset()
{
	static volatile bool sLastReset=false;
	static volatile int64_t sLastResetTicks=0;
	
	
	bool input = !gpio_get_pin_level(IN_RESET);
	
	if (input != sLastReset)
	{
		if ((gTicks-sLastResetTicks) > kDebounceSignalTicks)
		{
			if (input)
			{
				reset();
			}
			
			sLastResetTicks = gTicks;
		}
		sLastReset = input;
	}
}

void saveSettings()
{
	static volatile int64_t sLastSaveTicks=0;

	if ((gTicks - sLastSaveTicks) > kSaveSettingsDelay)
	{
		// page size 3 is 64 bytes.
		//volatile uint8_t page_size = NVMCTRL->PARAM.bit.PSZ;
		
		uint8_t page_data[64];
		
		flash_read(&FLASH_0, 0x3200, page_data, 64);
		
		page_data[0] = gSwitcheeMode;
		page_data[1] = gCounterMode;
		
		flash_write(&FLASH_0, 0x3200, page_data, 64);
		
		sLastSaveTicks = gTicks;
	}
}

void readSettings()
{
	uint8_t page_data[64];
	flash_read(&FLASH_0, 0x3200, page_data, 64);
	
	gSwitcheeMode = page_data[0];
	if (gSwitcheeMode != kModeSequential && gSwitcheeMode != kModeBounce && gSwitcheeMode != kModeInverted)
		gSwitcheeMode = kModeSequential;
		
	gInvertChannels = (gSwitcheeMode == kModeInverted);
	
	gCounterMode = page_data[1];
	if (gCounterMode != kCounterModeLinear && gCounterMode != kCounterModeExponential && gCounterMode != kCounterModeFibonacci)
		gCounterMode = kCounterModeLinear;
}


int main(void)
{
	atmel_start_init();

	dac_sync_enable_channel(&DAC_0, 0);

	setupTimer();
	
	readSettings();
	
	flashModeIndicators();
	flashCounterModeIndicators();
	
	setModeIndicators();
	setCounterModeIndicators();

	while (1)
	{
		checkModeButton();
		checkCounterModeButton();
		checkTrigger();
		checkReset();
		
		if (gSettingsDirty)
		{
			saveSettings();	
		}
	}
}
