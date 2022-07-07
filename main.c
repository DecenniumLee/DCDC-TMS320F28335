/*
 * main.c
 *
 *  Created on: 2022��3��14��
 *      Author: ZGJ
 *      �˹���Ϊ˫��DCDC˫�ջ����Ƴ���
 */

/*---------------------Include---------------------*/
#include "DSP28x_Project.h"
#include "key.h"
#include "led.h"
#include "epwm_0.h"
#include "adc.h"
#include "math.h"
#include "pi.h"
#include "iic.h"
#include "oled.h"
/*---------------------------------��������---------------------------------------------*/
interrupt void adc_isr(void);

/*----------------------------------�궨��----------------------------------------------*/
#define  CPU_CLK  150e6               // ϵͳʱ�� 150MHz
#define  PWM_CLK  20e3                // ����Ƶ�� 20kHz
#define  Tsam     1/PWM_CLK      // �������� Tsam=1/20k=50us
#define  SP       CPU_CLK/(2*PWM_CLK) // ePWM���ڼĴ�����ֵ 3750

//����������
#define  Kp_V   0.05                     // ��ѹ������ϵ��
#define  Ki_V   0.05                     // ��ѹ������ϵ��
#define  Kp_I   0.1                     // ����������ϵ��
#define  Ki_I   0.03                     // ����������ϵ��

//�޷�ֵ
#define  Umax       25              // ��ѹ������ֵ
#define  Imax       1.6               // ����������ֵ

/*-------------------------------�������弰��ʼ��----------------------------------------*/
float  Uo=0;                          // �����ѹ
float  IL=0;                          // ���ص���
float  Uin=0;                         // �����ѹ
int    Uo_AD=0;
int    IL_AD=0;
int    Uin_AD=0;
float  Irefm = 1.5;
float  Urefm = 0.7;                    // �����ѹ��ֵ���� Uom*=24V�����ռ�ձ��޶�
float  Iref=0;                        // ����������ָ��ֵ Io*
float  Uref=24.3;                        // �����ѹָ��ֵ Uo*
float     Uoc = 0;                      // �����ѹ�趨ֵ,��ΧΪ0-1���Ƚ����趨ֵΪSP*Uoc
int    start_flag = 0;                // ������־��Ϊ1��ʾװ����������
int    protect_flag = 0;              // ������־��Ϊ1��ʾ����������
PI_t PI_Voltage;
PI_t PI_Current;
#define N 30
void main()
{
    MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();
    //Step 0. ��ʼ������������
    pi_init(&PI_Voltage, Kp_V, Ki_V, Tsam);
    pi_init(&PI_Current, Kp_I, Ki_I, Tsam);
    //Step 1. ��ʼ��ϵͳ����
    InitSysCtrl();  //Initialize System Control

    //Step 2. ��ʼ��GPIO
    InitGpio(); //Initialize GPIO

    //Step 3. ��������жϲ���ʼ���ж�������
    DINT;
    InitPieCtrl();  //Initialize the PIE control registers to their default state
    IER = 0x0000;   //Disable CPU interrupts and clear all CPU interrupt flags
    IFR = 0x0000;
    InitPieVectTable(); //Initialize the PIE vector table with pointers to the shell Interrupt't Service Routines(ISR)
    EALLOW;
    PieVectTable.ADCINT = &adc_isr;       // ����������Ҫ���ж�ӳ�䵽�ж�������
    EDIS;

    //Step 4. ��ʼ������
//    InitAdc();                            // ��ʼ��ADCģ��(ʹ��ADCʱ�ӣ�У��ADC������ADC�ϵ�)
    Led_Init();
    Key_Init();
    Init_I2CA();
    Setup_ePWM1();                        // ����ePWM1ģ��
    Setup_ePWM2();                        // ����ePWM2ģ��
    Setup_Adc();                          // ����ADCģ��
    EPwm1Regs.TBCTR = 0;                  // ������ͬʱ����
    EPwm2Regs.TBCTR = 0;

    //Step 5. ʹ���ж�
    IER |= M_INT8;                        //I2C
    IER |= M_INT1;                        // ʹ��CPU�жϣ�ADCINT�ڵ�1���ж�
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;    // ʹ��PIE�жϣ�ADCINT��PIE��1��ĵ�6���ж�
    PieCtrlRegs.PIEIER8.bit.INTx1 = 1;    //I2C

    EINT;                                 // ʹ�����ж� INTM
    ERTM;                                 // ʹ����ʵʱ�ж� DBGM

    OLED_Init();//OLED��ʼ��
    OLED_Clear();


    OLED_ShowString(0,0,"DC-DC",8);
    OLED_ShowString(0,2,"Ui:",8);
    OLED_ShowString(0,3,"Uo:",8);
    OLED_ShowString(0,4,"Io:",8);
    //Step 6. ѭ���ȴ��ж�
    for(; ;)
    {
        OLED_ShowDecimalNum(24,2,Uin,8);
        OLED_ShowDecimalNum(24,3,Uo,8);
        OLED_ShowDecimalNum(24,4,IL,8);
        OLED_ShowDecimalNum(24,5,Uoc,8);
//        asm("          NOP");
    }
}


interrupt void adc_isr(void)   // ADC�ж��Ӻ���
{
    //���ݶ�ȡ�봦��
    Uo_AD = AdcRegs.ADCRESULT0 >> 4;         // ��ȡADת�����
    IL_AD = AdcRegs.ADCRESULT1 >> 4;
    Uin_AD = AdcRegs.ADCRESULT2 >> 4;
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;

    Uo = Uo_AD*3.0/4095;
    IL = IL_AD*3.0/4095;
    Uin = Uin_AD*3.0/4095;

    Uo = Uo * 13.33333 - 0.0933333;      // �����ѹ����ʵ��ֵ
    IL = (IL/0.82-2.5)/0.4;    //Ӧ����ʵ�ʲ�����·���¼���
    Uin = Uin * 12.820513 - 0.0128205;

    //*******������*********
    //    Uin = 20;
    //    Uo = 24;
    //    IL = 1;
    //*******end*********

    //��ѹ�������
    if ((IL>Imax))//(Uo>Umax)||
    {
        EALLOW;
        EPwm1Regs.TZFRC.bit.OST=1;           // ����ePWM1�������ź�
        EPwm2Regs.TZFRC.bit.OST=1;           // ����ePWM2�������ź�
        EDIS;
        protect_flag = 1;
        LED1_OFF;    // LED1��
    }

    start_flag = 1;
    //Ƿѹ���
    //���������ѹ��Χȷ����ֵ
//        if ((protect_flag==0)&&(Uin>=10)&&(Uin<=40))
//        {
//            EALLOW;
//            EPwm1Regs.TZCLR.bit.OST = 1;           // ��������źţ���PWM������ԭ
//            EPwm2Regs.TZCLR.bit.OST = 1;           // ��������źţ���PWM������ԭ
//            EDIS;
//            start_flag = 1;
//            LED1_ON;  // LED1��
//        }
//        else
//        {
//            EALLOW;
//            EPwm1Regs.TZFRC.bit.OST = 1;           // ����ePWM1�������ź�
//            EPwm2Regs.TZFRC.bit.OST = 1;           // ����ePWM1�������ź�
//            EDIS;
//            start_flag = 0;
//            LED1_OFF;    // LED1��
//        }

    //�ջ�����
    if (start_flag == 1)
    {
        //��ѹ�⻷����
        Iref = pi_calc(&PI_Voltage, Uref, Uo);      //��ѹ��������������ڻ�ָ��ֵ
        if(Iref > Irefm)  Iref = Irefm;            // �Ե����ڻ�ָ��ֵ�����޷�

        //�����ڻ�����
        Uoc = pi_calc(&PI_Current, Iref, IL);
        //*******������*********
        //��ѹ�⻷����
        Uoc = Iref;
        //********end**********
        if(Uoc > Urefm)  Uoc = Urefm;            // �Ե�������������������޷�
    }

        //********************************
        //������
    //    Uoc = 0.5;  //�������Ƶ�����
        //********************************
    EPwm1Regs.CMPA.half.CMPA = (int)((1-Uoc)*SP);
    EPwm2Regs.CMPA.half.CMPA = (int)(Uoc*SP);
//    }
    //����жϱ�־
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1; // ���ADC���жϱ�־λ
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;    // �����1����ж���Ӧ��־λ��ʹCPU������Ӧ��1������ж�
}

