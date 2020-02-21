#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#define BufferLength 2

uint16_t adc_value[BufferLength] = {0 , 0}; //her iki adc nin de baslangic degerleri 0 olarak belirledik.

uint16_t adc_value1;
uint16_t adc_value2;

void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void ADC_Config()
{
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_CommonStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonStruct.ADC_Prescaler = ADC_Prescaler_Div4; //adc max 36 MHz de calistigi icin bolduk
	ADC_CommonStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//Tek kanaldan dma okumasi yaptigimiz icin disable dedik.cok kanalli olsa idi diger modlardan birini sececektik
	ADC_CommonStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;//iki ornekleme arasindaki bekleme suresini sectik.datalar birbirine giememesi icin

	ADC_Init(ADC1,&ADC_CommonStruct);

	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//COKLU ADC OKUMALAR�NDA TARAMA YAPMAK icin kullanilir.
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//adc okumasinda harici tetikleme ile baslatma saglar.tetikleme yok dedik
	ADC_InitStruct.ADC_ExternalTrigConv = 0;//cevrim tetigi yok dedik, harici tetikleme olmas�n
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//data yazma islemi saga yasl�
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; //adc cevriminin surekli yapilmasini istiyoruz.disable yaparsak tek bir sefer cevrim yapar
	ADC_InitStruct.ADC_NbrOfConversion = BufferLength; // 2 adet adc cevrim okumasi yapacagimizi belirtik.ADC1 kullanildi sadece

	ADC_Init(ADC1,&ADC_InitStruct);
	ADC_Cmd(ADC1,ENABLE);


	ADC_RegularChannelConfig(ADC1,ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 2, ADC_SampleTime_3Cycles);

	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//her adc dma transferinden sonra yeni transferler icin istek yapiyoruz
	ADC_DMACmd(ADC1 , ENABLE);//adc yi dma ile bagladik
}

void DMA_Config()
{
	DMA_InitTypeDef DMA_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

	DMA_InitStruct.DMA_Channel = DMA_Channel_0;//syf 308
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh ; //oncelik durumu
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t ) & ADC1->DR;//DMA YA veriyi ADC1 �N DATE REG�STER(DR) den alacagini soyledik
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) & adc_value[1];//ADC den okudugumuz veriyi adc_value degisken adresine yaziyoruz.
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;//cevresel birimden hafizaya
	DMA_InitStruct.DMA_BufferSize = BufferLength;//kac farkl� cevrim yapacagimizi belirttik
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable; //ilk giren veri ilk c�ks�n dedik.
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//fifo verileri ne kadar doluyken iletsin
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; //hic durmadan veri aktarimina devam etsin
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16 bit sectik cunki 12 bitlik bir adc okumasi yap�yoruz
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//yukaridakiyle aym� olmak zorunda
	DMA_InitStruct.DMA_MemoryInc = ENABLE; // haf�zaya yazma i�leminde adresin surekli degismesini sagl�yoruz .cunku surekli ayni yere yazarsa eski degerler kaybolacak
	DMA_InitStruct.DMA_PeripheralInc = DISABLE;// ancek veri okudugumuz adresin surekli sabit kalmasini sagliyoruz.cunku okudugumuz adres hic degismiyor

	DMA_Init(DMA2_Stream0,&DMA_InitStruct);
	DMA_Cmd(DMA2_Stream0,ENABLE);
}


int main(void)
{
	GPIO_Config();
	ADC_Config();
	DMA_Config();

	ADC_SoftwareStartConv(ADC1);//ADC yi yazilimsal olarak baslatiyoruz.
	/* ADC_SoftwareStartConv islemini while icinde yapmad�k cunku while icinde yaparsak cevresel birim -> CPU -> RAM seklinde
	 * yazm�s oluruz. ama burada direk olarak cevresel birim -> RAM  seklinde bir yazma gerceklestirdik.  */
  while (1)
  {
	  adc_value1 = adc_value[0];
	  adc_value2 = adc_value[1];

  }
}
