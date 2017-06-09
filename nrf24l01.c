#include "stm8s.h"
#include "timer.h"

#define uint unsigned int 
#define uchar unsigned char

#define CE_1    GPIO_WriteHigh(GPIOC, GPIO_PIN_3) 
#define CE_0    GPIO_WriteLow(GPIOC, GPIO_PIN_3) 

//SPIʱ�Ӷ�
#define SCK_1   GPIO_WriteHigh(GPIOC, GPIO_PIN_5)
#define SCK_0   GPIO_WriteLow(GPIOC, GPIO_PIN_5)

//SPI��������ӻ������
#define MISO    GPIO_ReadInputPin(GPIOC,GPIO_PIN_7)
//SPI��������ӻ������ 
#define MOSI_1  GPIO_WriteHigh(GPIOC, GPIO_PIN_6)
#define MOSI_0  GPIO_WriteLow(GPIOC, GPIO_PIN_6)
//SPIƬѡ��//����SS
#define CSN_1   GPIO_WriteHigh(GPIOC, GPIO_PIN_4)
#define CSN_0   GPIO_WriteLow(GPIOC, GPIO_PIN_4) 

#define TX_ADDR_WITDH 5//���͵�ַ�������Ϊ5���ֽ�
#define RX_ADDR_WITDH 5
#define TX_DATA_WITDH 8
#define RX_DATA_WITDH 8

// nRF24L01ָ���ʽ��
#define R_REGISTER      0x00    // ���Ĵ���
#define W_REGISTER      0x20    // д�Ĵ���
#define R_RX_PLOAD      0x61    // ��RX FIFO��Ч���ݣ�1-32�ֽڣ�����������ɺ����ݱ������Ӧ���ڽ���ģʽ
#define W_TX_PLOAD      0xA0    // дTX FIFO��Ч���ݣ�1-32�ֽڣ�д�������ֽ�0��ʼ��Ӧ���ڷ���ģʽ
#define FLUSH_TX        0xE1    // ���TX FIFO�Ĵ�����Ӧ���ڷ���ģʽ
#define FLUSH_RX        0xE2    // ���RX FIFO�Ĵ�����Ӧ���ڽ���ģʽ
#define REUSE_TX_PL     0xE3    // ����ʹ����һ����Ч���ݣ���CEΪ�߹����У����ݰ������ϵ����·���
#define NOP             0xFF    // �ղ���������������״̬�Ĵ���

// nRF24L01�Ĵ�����ַ 
#define CONFIG      0x00  // ���üĴ���
#define EN_AA       0x01  // ���Զ�Ӧ�𡱹��ܼĴ�
#define EN_RX_ADDR  0x02  // ����ͨ��ʹ�ܼĴ���
#define SETUP_AW    0x03  // ��ַ������üĴ���
#define SETUP_RETR  0x04  // �Զ��ط����üĴ���
#define RF_CH       0x05  // ��Ƶͨ��Ƶ�����üĴ���
#define RF_SETUP    0x06  // ��Ƶ���üĴ���
#define STATUS      0x07  // ״̬�Ĵ���
#define OBSERVE_TX  0x08  // ���ͼ��Ĵ���
#define CD          0x09  // �ز����Ĵ���
#define RX_ADDR_P0  0x0A  // ����ͨ��0���յ�ַ�Ĵ���
#define RX_ADDR_P1  0x0B  // ����ͨ��1���յ�ַ�Ĵ���
#define RX_ADDR_P2  0x0C  // ����ͨ��2���յ�ַ�Ĵ���
#define RX_ADDR_P3  0x0D  // ����ͨ��3���յ�ַ�Ĵ���
#define RX_ADDR_P4  0x0E  // ����ͨ��4���յ�ַ�Ĵ���
#define RX_ADDR_P5  0x0F  // ����ͨ��5���յ�ַ�Ĵ���
#define TX_ADDR     0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0    0x11  // ����ͨ��0��Ч���ݿ�����üĴ���
#define RX_PW_P1    0x12  // ����ͨ��1��Ч���ݿ�����üĴ���
#define RX_PW_P2    0x13  // ����ͨ��2��Ч���ݿ�����üĴ���
#define RX_PW_P3    0x14  // ����ͨ��3��Ч���ݿ�����üĴ���
#define RX_PW_P4    0x15  // ����ͨ��4��Ч���ݿ�����üĴ���
#define RX_PW_P5    0x16  // ����ͨ��5��Ч���ݿ�����üĴ���
#define FIFO_STATUS 0x17  // FIFO״̬�Ĵ���

uchar sta;    // ״̬���� 
#define RX_DR  (sta & 0x40)  // ���ճɹ��жϱ�־
#define TX_DS  (sta & 0x20)  // ����ɹ��жϱ�־
#define MAX_RT (sta & 0x10)  // �ط�����жϱ�־
uchar TX_Addr[]={0x34,0x43,0x10,0x10,0x01};
uchar TX_Buffer[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x00};
uchar RX_Buffer[RX_DATA_WITDH] = {0x00};

void nRF24L01_Init(void)
{
  GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  
  sleep_ms(2);
  
  CE_0;//����ģʽ��
  CSN_1;
  SCK_0;
  //IRQ=1;
}

uchar SPI_RW(uchar byte)
{
  uchar i;
  for(i=0;i<8;i++)      //һ�ֽ�8λѭ��8��д��
  {
    if(byte&0x80)       //����������λ��1//�����ʶ��ֽڼĴ���ʱ����Ҫ��/д��������ֽڵĸ�λ
      MOSI_1;           //��NRF24L01д1
    else                //����д0
      MOSI_0;
    
    byte<<=1;           //��һλ�Ƶ����λ
    SCK_1;              //SCK���ߣ�д��һλ���ݣ�ͬʱ��ȡһλ���� 
    if(MISO) 
      byte|=0x01;
    SCK_0;              //SCK����
  }
  
  return byte;          //���ض�ȡһ�ֽ�
}

uchar SPI_W_Reg(uchar reg,uchar value)
{
  uchar status;         //����״̬
  CSN_0;                //SPIƬѡ
  status=SPI_RW(reg);   //д��Ĵ�����ַ��ͬʱ��ȡ״̬
  SPI_RW(value);        //д��һ�ֽ�
  CSN_1;                //
  return status;        //����״̬
}

uchar SPI_R_byte(uchar reg)
{
  uchar reg_value;
  CSN_0;                //SPIƬѡ
  SPI_RW(reg);          //д���ַ
  reg_value=SPI_RW(0);  //��ȡ�Ĵ�����ֵ
  CSN_1;
  return reg_value;     //���ض�ȡ��ֵ
}

uchar SPI_R_DBuffer(uchar reg,uchar *Dat_Buffer,uchar Dlen)
{
  uchar status,i;
  CSN_0;                //SPIƬѡ
  status=SPI_RW(reg);   //д��Ĵ�����ַ��ͬʱ״̬
  for(i=0;i<Dlen;i++)
  {
    Dat_Buffer[i]=SPI_RW(0);    //�洢����
  }
  
  CSN_1;
  return status;
}

uchar SPI_W_DBuffer(uchar reg,uchar *TX_Dat_Buffer,uchar Dlen)
{
  uchar status,i;
  CSN_0;                //SPIƬѡ������ʱ��
  status=SPI_RW(reg);
  for(i=0;i<Dlen;i++)
  {
    SPI_RW(TX_Dat_Buffer[i]);   //��������
  }
  CSN_1;
  return status;
}

void nRF24L01_Set_TX_Mode(uchar *TX_Data)
{
  CE_0;                 //������д�Ĵ���֮ǰһ��Ҫ�������ģʽ�����ģʽ��
  SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
  /*д�Ĵ���ָ��+���սڵ��ַ+��ַ���*/
  SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
  /*Ϊ�˽����豸Ӧ���źţ�����ͨ��0��ַ�뷢�͵�ַ��ͬ*/
  SPI_W_DBuffer(W_TX_PLOAD,TX_Data,TX_DATA_WITDH);
  /*д��Ч���ݵ�ַ+��Ч����+��Ч���ݿ��*/
  SPI_W_Reg(W_REGISTER+EN_AA,0x01);
  /*����ͨ��0�Զ�Ӧ��*/
  SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
  /*ʹ�ܽ���ͨ��0*/
  SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
  /*�Զ��ط���ʱ250US+86US���ط�10��*/
  SPI_W_Reg(W_REGISTER+RF_CH,0x40);
  /*(2400+40)MHZѡ����Ƶͨ��0X40*/
  SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);
  /*1Mbps����,���书��:0DBM,�������Ŵ�������*/
  SPI_W_Reg(W_REGISTER+CONFIG,0x0e);
  /*����ģʽ,�ϵ�,16λCRCУ ��,CRCʹ��*/
  CE_1;//��������
  sleep_ms(5);
  /*CE�ߵ�ƽ����ʱ������10US����*/
}

uchar Check_Rec(void)
{
  uchar status;
  sta=SPI_R_byte(R_REGISTER+STATUS);
  if(RX_DR)
  {
    CE_0;
    SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
    status=1;
  }
  
  SPI_W_Reg(W_REGISTER+STATUS,0xff);
  return status;
}

uchar Check_Ack(void)
{
  sta=SPI_R_byte(R_REGISTER+STATUS);
  /*��ȡ�Ĵ�״̬*/
  if(TX_DS||MAX_RT)/*���TX_DS��MAX_RTΪ1,������жϺ����TX_FIFO�Ĵ�����ֵ*/
  {
    SPI_W_Reg(W_REGISTER+STATUS,0xff);
    CSN_0;
    SPI_RW(FLUSH_TX);
    CSN_1;
    return 0;
  }
  else
    return 1;
}

void nRF24L01_Set_RX_Mode(void)
{
  CE_0;//����
  SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
  SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
  SPI_W_Reg(W_REGISTER+EN_AA,0x01);//auot ack
  SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
  SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
  SPI_W_Reg(W_REGISTER+RX_PW_P0,RX_DATA_WITDH);
  SPI_W_Reg(W_REGISTER+RF_CH,0);
  SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//0db,lna
  SPI_W_Reg(W_REGISTER+CONFIG,0x0f);
  CE_1;
  sleep_ms(5);
}

uchar nRF24L01_RX_Data(void)
{
  // uchar i,status;
  sta=SPI_R_byte(R_REGISTER+STATUS);
  if(RX_DR)
  {
    CE_0;
    SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
    SPI_W_Reg(W_REGISTER+STATUS,0xff);
    CSN_0;
    SPI_RW(FLUSH_RX);
    CSN_1;
    return 1;
  }
  else
    return 0;
}