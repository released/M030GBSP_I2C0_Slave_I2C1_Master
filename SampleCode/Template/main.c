/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "i2c_conf.h"
#include "misc_config.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_SPECIFIC       			(flag_PROJ_CTL.bit1)
#define FLAG_PROJ_UART_CMD                 				(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_ADC_LOG                               (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_ADC_FINISH                            (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

enum
{
	ADC0_CH0 = 0 ,
	ADC0_CH1 , 	
	ADC0_CH2 , 
	ADC0_CH3 , 
	ADC0_CH4 , 
	
	ADC0_CH5 , 
	ADC0_CH6 , 
	ADC0_CH7 , 
	ADC0_CH8 , 
	ADC0_CH9 , 
	
	ADC0_CH10 , 
	ADC0_CH11 , 
	ADC0_CH12 ,
	ADC0_CH13 , 
	ADC0_CH14 , 
	ADC0_CH15 , 
	
	ADC_CH_DEFAULT 	
}ADC_CH_TypeDef;


#define ADC_RESOLUTION							        (4096ul)
#define ADC_MAX_TARGET							        (4095ul)	//(float)(2.612f)
#define ADC_MIN_TARGET							        (0ul)	//(float)(0.423f)

#define ADC_SAMPLE_COUNT                                (8)
#define ADC_SAMPLE_POWER	 				            (3)
#define ADC_SAMPLE_DROP 						        (4ul)
#define ADCTotalLength                                  (ADC_SAMPLE_COUNT+ADC_SAMPLE_DROP)   // drop first 4 ADC result 
#define ADC_DIGITAL_SCALE(void) 		                (0xFFFU >> ((0) >> (3U - 1U)))		//0: 12 BIT 

uint32_t AVdd = 0;
volatile uint16_t ADC_ch_value = 0;
volatile int16_t g_iConversionData[ADCTotalLength] = {0};

/*
    use https://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml

    target point : 256
    amplitude : 12 bit / 4096 - 1 , range : 0 ~ 4095 
*/
const uint16_t g_au16Sine[] = 
{

    /*64 bytes , BSP sample code*/
    // 2047, 2251, 2453, 2651, 2844, 3028, 3202, 3365, 3515, 3650, 3769, 3871, 3954,
    // 4019, 4064, 4088, 4095, 4076, 4040, 3984, 3908, 3813, 3701, 3573, 3429, 3272,
    // 3102, 2921, 2732, 2536, 2335, 2132, 1927, 1724, 1523, 1328, 1141,  962,  794,
    // 639,  497,  371,  262,  171,   99,   45,   12,    0,    7,   35,   84,  151,
    // 238,  343,  465,  602,  754,  919, 1095, 1281, 1475, 1674, 1876,
    
    /*64 bytes*/
    // 2048, 2248, 2447, 2642, 2831, 3013, 3185, 3346,
    // 3495, 3630, 3750, 3853, 3939, 4007, 4056, 4085,
    // 4095, 4085, 4056, 4007, 3939, 3853, 3750, 3630,
    // 3495, 3346, 3185, 3013, 2831, 2642, 2447, 2248,
    // 2048, 1847, 1648, 1453, 1264, 1082, 910, 749,
    // 600, 465, 345, 242, 156, 88, 39, 10,
    // 0, 10, 39, 88, 156, 242, 345, 465,
    // 600, 749, 910, 1082, 1264, 1453, 1648, 1847,

    /*256 bytes*/
    0x800, 0x832, 0x864, 0x896, 0x8c8, 0x8fa, 0x92c, 0x95e,
    0x98f, 0x9c0, 0x9f1, 0xa22, 0xa52, 0xa82, 0xab1, 0xae0,
    0xb0f, 0xb3d, 0xb6b, 0xb98, 0xbc5, 0xbf1, 0xc1c, 0xc47,
    0xc71, 0xc9a, 0xcc3, 0xceb, 0xd12, 0xd39, 0xd5f, 0xd83,
    0xda7, 0xdca, 0xded, 0xe0e, 0xe2e, 0xe4e, 0xe6c, 0xe8a,
    0xea6, 0xec1, 0xedc, 0xef5, 0xf0d, 0xf24, 0xf3a, 0xf4f,
    0xf63, 0xf76, 0xf87, 0xf98, 0xfa7, 0xfb5, 0xfc2, 0xfcd,
    0xfd8, 0xfe1, 0xfe9, 0xff0, 0xff5, 0xff9, 0xffd, 0xffe,
    0xfff, 0xffe, 0xffd, 0xff9, 0xff5, 0xff0, 0xfe9, 0xfe1,
    0xfd8, 0xfcd, 0xfc2, 0xfb5, 0xfa7, 0xf98, 0xf87, 0xf76,
    0xf63, 0xf4f, 0xf3a, 0xf24, 0xf0d, 0xef5, 0xedc, 0xec1,
    0xea6, 0xe8a, 0xe6c, 0xe4e, 0xe2e, 0xe0e, 0xded, 0xdca,
    0xda7, 0xd83, 0xd5f, 0xd39, 0xd12, 0xceb, 0xcc3, 0xc9a,
    0xc71, 0xc47, 0xc1c, 0xbf1, 0xbc5, 0xb98, 0xb6b, 0xb3d,
    0xb0f, 0xae0, 0xab1, 0xa82, 0xa52, 0xa22, 0x9f1, 0x9c0,
    0x98f, 0x95e, 0x92c, 0x8fa, 0x8c8, 0x896, 0x864, 0x832,
    0x800, 0x7cd, 0x79b, 0x769, 0x737, 0x705, 0x6d3, 0x6a1,
    0x670, 0x63f, 0x60e, 0x5dd, 0x5ad, 0x57d, 0x54e, 0x51f,
    0x4f0, 0x4c2, 0x494, 0x467, 0x43a, 0x40e, 0x3e3, 0x3b8,
    0x38e, 0x365, 0x33c, 0x314, 0x2ed, 0x2c6, 0x2a0, 0x27c,
    0x258, 0x235, 0x212, 0x1f1, 0x1d1, 0x1b1, 0x193, 0x175,
    0x159, 0x13e, 0x123, 0x10a, 0xf2, 0xdb, 0xc5, 0xb0,
    0x9c, 0x89, 0x78, 0x67, 0x58, 0x4a, 0x3d, 0x32,
    0x27, 0x1e, 0x16, 0xf, 0xa, 0x6, 0x2, 0x1,
    0x0, 0x1, 0x2, 0x6, 0xa, 0xf, 0x16, 0x1e,
    0x27, 0x32, 0x3d, 0x4a, 0x58, 0x67, 0x78, 0x89,
    0x9c, 0xb0, 0xc5, 0xdb, 0xf2, 0x10a, 0x123, 0x13e,
    0x159, 0x175, 0x193, 0x1b1, 0x1d1, 0x1f1, 0x212, 0x235,
    0x258, 0x27c, 0x2a0, 0x2c6, 0x2ed, 0x314, 0x33c, 0x365,
    0x38e, 0x3b8, 0x3e3, 0x40e, 0x43a, 0x467, 0x494, 0x4c2,
    0x4f0, 0x51f, 0x54e, 0x57d, 0x5ad, 0x5dd, 0x60e, 0x63f,
    0x670, 0x6a1, 0x6d3, 0x705, 0x737, 0x769, 0x79b, 0x7cd,
};

static uint32_t g_u32Index = 0;
const uint32_t g_u32ArraySize = sizeof(g_au16Sine) / sizeof(uint16_t);

uint8_t uart_data = 0;
/*_____ M A C R O S ________________________________________________________*/

// #define ENABLE_ADC_IRQ

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

void delay_ms(uint16_t ms)
{
	#if 1
    uint32_t tickstart = get_tick();
    uint32_t wait = ms;
	uint32_t tmp = 0;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000 -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
	
	#else
	TIMER_Delay(TIMER0, 1000*ms);
	#endif
}


void DAC01_IRQHandler(void)
{
    if (DAC_GET_INT_FLAG(DAC0, 0))
    {

        if (g_u32Index == g_u32ArraySize)
            g_u32Index = 0;
        else
        {
            DAC_WRITE_DATA(DAC0, 0, g_au16Sine[g_u32Index++]);
            DAC_START_CONV(DAC0);
            /* Clear the DAC conversion complete finish flag */
            DAC_CLR_INT_FLAG(DAC0, 0);
        }
    }

    return;
}


void DAC_Init(void)
{
    printf("%s:%d\r\n",__FUNCTION__,g_u32ArraySize);

    /* Set the software trigger DAC and enable D/A converter */
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);

    /* The DAC conversion settling time is 1us */
    DAC_SetDelayTime(DAC0, 1);

    /* Set DAC 12-bit holding data */
    // DAC_WRITE_DATA(DAC0, 0, 0x400);
    DAC_WRITE_DATA(DAC0, 0, g_au16Sine[g_u32Index]);

    /* Clear the DAC conversion complete finish flag for safe */
    DAC_CLR_INT_FLAG(DAC0, 0);

    /* Enable the DAC interrupt */
    DAC_ENABLE_INT(DAC0, 0);
    NVIC_EnableIRQ(DAC01_IRQn);

    /* Start A/D conversion */
    DAC_START_CONV(DAC0);  
}

void Sort_tab(uint16_t tab[], uint8_t length)
{
	uint8_t l = 0x00, exchange = 0x01; 
	uint16_t tmp = 0x00;

	/* Sort tab */
	while(exchange==1) 
	{ 
		exchange=0; 
		for(l=0; l<length-1; l++) 
		{
			if( tab[l] > tab[l+1] ) 
			{ 
				tmp = tab[l]; 
				tab[l] = tab[l+1]; 
				tab[l+1] = tmp; 
				exchange=1; 
			}
		}
	} 
}

void ADC_IRQHandler(void)
{
    FLAG_PROJ_ADC_FINISH = 1;
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* Clear the A/D interrupt flag */
}

uint16_t ADC_InitChannel(uint8_t ch)
{
    uint16_t tmp = 0;
  	volatile uint32_t sum = 0;
    uint16_t u16adc_convert_target = 0;
	volatile uint16_t adc_value = 0;

	u16adc_convert_target = (ADC_MIN_TARGET*ADC_RESOLUTION/AVdd);

    // polling
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 0x1 << ch);

    for ( tmp = 0 ; tmp < (ADCTotalLength) ; tmp++)
    {  		
        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        #if defined (ENABLE_ADC_IRQ)
        ADC_ENABLE_INT(ADC, ADC_ADF_INT);  /* Enable sample module A/D interrupt. */
        NVIC_EnableIRQ(ADC_IRQn);
        FLAG_PROJ_ADC_FINISH = 0;

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        while(FLAG_PROJ_ADC_FINISH == 0);
        ADC_DISABLE_INT(ADC, ADC_ADF_INT);

        #else
        /* Start A/D conversion */
        ADC_START_CONV(ADC);
        while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
        #endif

        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);        		
        g_iConversionData[tmp] = ADC_GET_CONVERSION_DATA(ADC, ch);		
        // printf("ch[%2d]%2d:0x%04X\r\n",ch,tmp,g_iConversionData[tmp]);					
    }

    Sort_tab( (uint16_t*) g_iConversionData,ADCTotalLength);
    for (tmp = ADC_SAMPLE_DROP/2; tmp < ADCTotalLength - ADC_SAMPLE_DROP/2; tmp++)
    {
        sum += g_iConversionData[tmp];
    }

    sum = sum >> ADC_SAMPLE_POWER;
    adc_value = sum;

	adc_value = (adc_value <= u16adc_convert_target) ? (u16adc_convert_target) : (adc_value); 
	adc_value = (adc_value >= ADC_RESOLUTION) ? (ADC_RESOLUTION) : (adc_value); 

    return (uint16_t)(adc_value);
}

uint32_t GetAVDDCodeByADC(void)
{

	volatile uint32_t sum = 0;
	uint16_t tmp = 0;
	uint16_t i = 0;
	uint16_t res = 0;

    // /* Power on ADC */
    SYS_ResetModule(ADC_RST);
    ADC_POWER_ON(ADC);
    // CLK_SysTickDelay(10000);

    /* Set input mode as single-end, Single mode, and select channel 29 (band-gap voltage) */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, BIT29);


    /*
        ADC clock source -> PCLK1  = 48 MHz                                
        ADC clock divider          = 2                                     
        ADC clock                  = 48 MHz / 2 = 24 MHz                   
        ADC extended sampling time = 71                                    
        ADC conversion time = 17 + ADC extended sampling time = 88         
        ADC conversion rate = 24 MHz / 88 = 272.7 ksps                     
    */
    /* To sample band-gap precisely, the ADC capacitor must be charged at least 3 us for charging the ADC capacitor ( Cin )*/
    /* Sampling time = extended sampling time + 1 */
    /* 1/24000000 * (Sampling time) = 3 us */
    ADC_SetExtendSampleTime(ADC, 0, 71);

    /* Clear conversion finish flag */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    for ( i = 0 ; i < ADC_SAMPLE_COUNT ; i++)
    {					
        /* Delay for band-gap voltage stability */
        CLK_SysTickDelay(100);

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        tmp = 0;

        /* Wait conversion done */
        while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        tmp = ADC_GET_CONVERSION_DATA(ADC, 29);
        sum += tmp;
    }
    res = (uint16_t) (sum >> ADC_SAMPLE_POWER);	

    return ( (uint16_t) res);

}

__STATIC_INLINE uint32_t FMC_ReadBandGap(void)
{
    FMC->ISPCMD = FMC_ISPCMD_READ_UID;            /* Set ISP Command Code */
    FMC->ISPADDR = 0x70u;                         /* Must keep 0x70 when read Band-Gap */
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;           /* Trigger to start ISP procedure */
#if ISBEN
    __ISB();
#endif                                            /* To make sure ISP/CPU be Synchronized */
    while(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) {}  /* Waiting for ISP Done */

    return FMC->ISPDAT & 0xFFF;
}

int32_t getBuiltInBandGap(void)
{
    int32_t res = 0;
    SYS_UnlockReg();
    FMC_Open();
    res = FMC_ReadBandGap();
    FMC_Close();
    SYS_LockReg();

    return res;
}

uint32_t GetAVDDVoltage(void)
{
    uint32_t  u32ConversionResult;
    uint32_t u32MvAVDD;
    uint32_t BuiltInData = 0;

    u32ConversionResult = GetAVDDCodeByADC();
    BuiltInData = getBuiltInBandGap();

    u32MvAVDD = (3072*BuiltInData) / u32ConversionResult;

    // printf("Conversion result: 0x%X\n", u32ConversionResult);

    return (uint32_t)u32MvAVDD;
}

uint16_t ADC_To_Voltage(uint16_t adc_value)
{
	uint16_t volt = 0;

	// volt = (uint16_t) (AVdd*adc_value)/ADC_DIGITAL_SCALE();
	volt = (float) (AVdd*adc_value)/ADC_DIGITAL_SCALE();
	
	#if 1   //debug
    if (FLAG_PROJ_ADC_LOG)
    {
        printf("%s[0x%4X,%4d]%4dmv,AVdd = %4dmv\r\n",__FUNCTION__ ,adc_value ,adc_value, volt, AVdd);
    }
	#endif

	return volt;	
}

void ADC_Process(void)
{
    ADC_ch_value = ADC_InitChannel(ADC0_CH11);
    ADC_To_Voltage(ADC_ch_value);

    // for test
    // ADC_ch_value = ADC_InitChannel(ADC0_CH6);
    // ADC_To_Voltage(ADC_ch_value);
}

void ADC_CH_Init(void)
{
    // /* Power on ADC */
    ADC_POWER_DOWN(ADC);
    SYS_ResetModule(ADC_RST);
    ADC_POWER_ON(ADC);
    // CLK_SysTickDelay(10000);

    // ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, NULL);

    ADC_SetExtendSampleTime(ADC, 0, 0xFF);
}

void ADC_VREF_Init(void)
{
    AVdd = GetAVDDVoltage();
    printf("AVDD Voltage: %dmV\r\n", AVdd);    
}


//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    printf("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        printf("0)POR Reset Flag\r\n");       
    }
    if (src & BIT1)
    {
        printf("1)NRESET Pin Reset Flag\r\n");       
    }
    if (src & BIT2)
    {
        printf("2)WDT Reset Flag\r\n");       
    }
    if (src & BIT3)
    {
        printf("3)LVR Reset Flag\r\n");       
    }
    if (src & BIT4)
    {
        printf("4)BOD Reset Flag\r\n");       
    }
    if (src & BIT5)
    {
        printf("5)System Reset Flag \r\n");       
    }
    if (src & BIT6)
    {
        printf("6)Reserved.\r\n");       
    }
    if (src & BIT7)
    {
        printf("7)CPU Reset Flag\r\n");       
    }
    if (src & BIT8)
    {
        printf("8)CPU Lockup Reset Flag\r\n");       
    }
    #endif

    
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        printf("power on from POR\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_CPURF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);

        printf("power on from CPU reset\r\n");
        return FALSE;         
    }    
    else if (src & SYS_RSTSTS_PINRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        
        printf("power on from nRESET pin\r\n");
        return FALSE;
    }
    
    printf("power on from unhandle reset source\r\n");
    return FALSE;
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_1000MS = 1;//set_flag(flag_timer_period_1000ms ,ENABLE);
		}

		if ((get_tick() % 100) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 1;
		}	

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

        // printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
        PC1 ^= 1;        
    }

    if (FLAG_PROJ_TIMER_PERIOD_SPECIFIC)
    {
        FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 0;
        ADC_Process();
    }

	if (FLAG_PROJ_UART_CMD)
	{
		FLAG_PROJ_UART_CMD = DISABLE;
		I2Cx_Master_example(uart_data);

	}	    
}

void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(UART0);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		printf("press : %c\r\n" , res);
		switch(res)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
                FLAG_PROJ_UART_CMD = ENABLE;
                uart_data = res - 0x30;
				break;

			case 'A':
			case 'a':
                FLAG_PROJ_ADC_LOG ^= 1;
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void UART0_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
			UARTx_Process();
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
    UART_EnableInt(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    NVIC_EnableIRQ(UART0_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
//	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
//	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	

    #if 0
    printf("FLAG_PROJ_TIMER_PERIOD_1000MS : 0x%2X\r\n",FLAG_PROJ_TIMER_PERIOD_1000MS);
    printf("FLAG_PROJ_REVERSE1 : 0x%2X\r\n",FLAG_PROJ_REVERSE1);
    printf("FLAG_PROJ_REVERSE2 : 0x%2X\r\n",FLAG_PROJ_REVERSE2);
    printf("FLAG_PROJ_REVERSE3 : 0x%2X\r\n",FLAG_PROJ_REVERSE3);
    printf("FLAG_PROJ_REVERSE4 : 0x%2X\r\n",FLAG_PROJ_REVERSE4);
    printf("FLAG_PROJ_REVERSE5 : 0x%2X\r\n",FLAG_PROJ_REVERSE5);
    printf("FLAG_PROJ_REVERSE6 : 0x%2X\r\n",FLAG_PROJ_REVERSE6);
    printf("FLAG_PROJ_REVERSE7 : 0x%2X\r\n",FLAG_PROJ_REVERSE7);
    #endif

}

void GPIO_Init(void)
{
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO);
    GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT);
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
//    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);


//    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

//    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /***********************************/
    // CLK_EnableModuleClock(TMR0_MODULE);
  	// CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    /***********************************/
	CLK_EnableModuleClock(CRC_MODULE);

    /***********************************/
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    // SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
    //                 (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD);
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA1MFP_Msk)) |
                    (SYS_GPA_MFPL_PA1MFP_UART0_TXD);

    /***********************************/
    CLK_EnableModuleClock(ADC_MODULE);
    /* Switch ADC clock source to HIRC, set divider to 2, ADC clock is 48/2 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(2));

    GPIO_SetMode(PB, BIT11, GPIO_MODE_INPUT);
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB11MFP_Msk)) |
                    (SYS_GPB_MFPH_PB11MFP_ADC0_CH11);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT11);

    // for test
    // GPIO_SetMode(PB, BIT6, GPIO_MODE_INPUT);
    // SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB6MFP_Msk)) |
    //                 (SYS_GPB_MFPL_PB6MFP_ADC0_CH6);
    // GPIO_DISABLE_DIGITAL_PATH(PB, BIT6);

    /***********************************/
    CLK_EnableModuleClock(DAC01_MODULE);

    /* Set PA multi-function pins for DAC voltage output */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA0MFP_Msk) | SYS_GPA_MFPL_PA0MFP_DAC0_OUT;

    /* Set PA.0 to input mode */
    PA->MODE &= ~(GPIO_MODE_MODE0_Msk) ;

    /* Disable digital input path of analog pin DAC01_OUT to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PA, (1ul << 0));

    /***********************************/
    
    /*I2C SLAVE*/
    CLK_EnableModuleClock(I2C0_MODULE);

    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF3MFP_Msk | SYS_GPF_MFPL_PF2MFP_Msk)) |
                    (SYS_GPF_MFPL_PF3MFP_I2C0_SCL | SYS_GPF_MFPL_PF2MFP_I2C0_SDA);

    /***********************************/
    
    /*I2C MASTER*/
    CLK_EnableModuleClock(I2C1_MODULE);

    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk)) |
                    (SYS_GPA_MFPL_PA3MFP_I2C1_SCL | SYS_GPA_MFPL_PA2MFP_I2C1_SDA);

    /***********************************/
   /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    SYS_Init();

    // for EVB I/O toggle test
	GPIO_Init();

    /*PA1 : TX , PB12 : RX*/
	UART0_Init();
	TIMER1_Init();
    check_reset_source();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif

    /*PB.11 : ADC0_CH11*/
    ADC_VREF_Init();
    ADC_CH_Init();

    /*PA.0 : DAC0_OUT*/
    DAC_Init();

    /*PF3 : SCL , PF2 : SDA*/
	I2Cx_Slave_Init();

    #if 1   // to verify I2C slave
    /*PA3 : SCL , PA2 : SDA*/
	I2Cx_Master_Init();
    #endif

    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
