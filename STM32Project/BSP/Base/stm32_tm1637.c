#include "stm32f10x.h"

#include "stm32_tm1637.h"


void _tm1637Start(void);
void _tm1637Stop(void);
void _tm1637ReadResult(void);
void _tm1637WriteByte(unsigned char b);
void _tm1637DelayUsec(unsigned int i);
void _tm1637ClkHigh(void);
void _tm1637ClkLow(void);
void _tm1637DioHigh(void);
void _tm1637DioLow(void);

// Configuration.


#define CLK_GPIO_RCC           RCC_APB2Periph_GPIOB
#define CLK_GPIO_PORT          GPIOB
#define CLK_GPIO_PIN      			GPIO_Pin_8

#define DIO_GPIO_RCC           RCC_APB2Periph_GPIOB
#define DIO_GPIO_PORT          GPIOB
#define DIO_GPIO_PIN      			GPIO_Pin_9

const char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};


void tm1637Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(CLK_GPIO_RCC|DIO_GPIO_RCC,ENABLE);//Ê¹ÄÜGPIOµÄÍâÉèÊ±ÖÓ
    /*B8*/
    GPIO_InitStructure.GPIO_Pin =CLK_GPIO_PIN;//Ñ¡ÔñÒªÓÃµÄGPIOÒý½Å
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ÉèÖÃÒý½ÅÄ£Ê½ÎªÍÆÃâÊä³öÄ£Ê½						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ÉèÖÃÒý½ÅËÙ¶ÈÎª50MHZ         
    GPIO_Init(CLK_GPIO_PORT, &GPIO_InitStructure);//µ÷ÓÃ¿âº¯Êý£¬³õÊ¼»¯GPIO
    /*B9*/
    GPIO_InitStructure.GPIO_Pin =DIO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 			 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DIO_GPIO_PORT, &GPIO_InitStructure);

//CLK_PORT_CLK_ENABLE();
//	DIO_PORT_CLK_ENABLE();
//	GPIO_InitTypeDef g = {0};
//	g.Pull = GPIO_PULLUP;
//	g.Mode = GPIO_MODE_OUTPUT_OD; // OD = open drain
//	g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	g.Pin = CLK_PIN;
//	HAL_GPIO_Init(CLK_PORT, &g);
//	g.Pin = DIO_PIN;
//	HAL_GPIO_Init(DIO_PORT, &g);

    tm1637SetBrightness(2);
			tm1637clr();

}
void tm1637clr(){
	u8 i;
    _tm1637Start();
    _tm1637WriteByte(0x40);
    _tm1637ReadResult();
    _tm1637Stop();

    _tm1637Start();
    _tm1637WriteByte(0xc0);
    _tm1637ReadResult();

    for (i = 0; i < 4; ++i) {
        _tm1637WriteByte(0);
        _tm1637ReadResult();
    }

    _tm1637Stop();
}

void tm1637DisplayDecimal(int v, int displaySeparator)
	{	//Usage: (1234,1)  disp 123.4
		//Usage: (1234,3)  disp 1.234
		//Usage: (1234,0)  disp 1234.
		
    unsigned char digitArr[4],i;
    for (i = 0; i < 3; ++i) {
        digitArr[i] = segmentMap[v % 10];
        if (i == displaySeparator) {
            digitArr[i] |= 1 << 7;
        }
        v /= 10;
    }
		digitArr[3] = 0;

    _tm1637Start();
    _tm1637WriteByte(0x40);
    _tm1637ReadResult();
    _tm1637Stop();

    _tm1637Start();
    _tm1637WriteByte(0xc0);
    _tm1637ReadResult();

    for (i = 0; i < 4; ++i) {
        _tm1637WriteByte(digitArr[3 - i]);
        _tm1637ReadResult();
    }

    _tm1637Stop();
}

void tm1637DisplayTime(int v,int f)
	{	
    unsigned char digitArr[4],i;
    for (i = 0; i < 4; ++i) {
        digitArr[i] = segmentMap[v % 10];
//        if (i == displaySeparator) {
//            digitArr[i] |= 1 << 7;
//        }
        v /= 10;
    }

    _tm1637Start();
    _tm1637WriteByte(0x40);
    _tm1637ReadResult();
    _tm1637Stop();

    _tm1637Start();
    _tm1637WriteByte(0xc0);
    _tm1637ReadResult();

    for (i = 0; i < 4; ++i) {
        _tm1637WriteByte(digitArr[3 - i]);
        _tm1637ReadResult();
    }
		_tm1637WriteByte(f?0xff:0);
        _tm1637ReadResult();
    _tm1637Stop();
}
// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(char brightness)
{
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    _tm1637Start();
    _tm1637WriteByte(0x87 + brightness);
    _tm1637ReadResult();
    _tm1637Stop();
}

void _tm1637Start(void)
{
    _tm1637ClkHigh();
    _tm1637DioHigh();
    _tm1637DelayUsec(2);
    _tm1637DioLow();
}

void _tm1637Stop(void)
{
    _tm1637ClkLow();
    _tm1637DelayUsec(2);
    _tm1637DioLow();
    _tm1637DelayUsec(2);
    _tm1637ClkHigh();
    _tm1637DelayUsec(2);
    _tm1637DioHigh();
}

void _tm1637ReadResult(void)
{
    _tm1637ClkLow();
    _tm1637DelayUsec(5);
    // while (dio); // We're cheating here and not actually reading back the response.
    _tm1637ClkHigh();
    _tm1637DelayUsec(2);
    _tm1637ClkLow();
}

void _tm1637WriteByte(unsigned char b)
{
	int i;
    for ( i = 0; i < 8; ++i) {
        _tm1637ClkLow();
        if (b & 0x01) {
            _tm1637DioHigh();
        }
        else {
            _tm1637DioLow();
        }
        _tm1637DelayUsec(3);
        b >>= 1;
        _tm1637ClkHigh();
        _tm1637DelayUsec(3);
    }
}

void _tm1637DelayUsec(unsigned int i)
{
	int j;
    for (; i>0; i--) {
        for ( j = 0; j < 3; ++j) {
            //__asm__ __volatile__("nop\n\t":::"memory");
        }
    }
}

void _tm1637ClkHigh(void)
{
  //  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);
	GPIO_WriteBit(CLK_GPIO_PORT,CLK_GPIO_PIN,Bit_SET);
}

void _tm1637ClkLow(void)
{
//    HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
		GPIO_WriteBit(CLK_GPIO_PORT,CLK_GPIO_PIN,Bit_RESET);

}

void _tm1637DioHigh(void)
{
    //HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);
		GPIO_WriteBit(DIO_GPIO_PORT,DIO_GPIO_PIN,Bit_SET);

}

void _tm1637DioLow(void)
{
    //HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);
		GPIO_WriteBit(DIO_GPIO_PORT,DIO_GPIO_PIN,Bit_RESET);

}
