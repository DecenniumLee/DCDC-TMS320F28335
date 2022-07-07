/*
 * adc.c
 *
 *  Created on: 2022��3��29��
 *      Author: ZGJ
 */

#include "adc.h"

void Setup_Adc(void)  // ADCģ�������Ӻ���
{
    EALLOW;
    SysCtrlRegs.HISPCP.all = 3; // HSPCLK = SYSCLKOUT/(2*3)=25MHz
    EDIS;

    InitAdc();   // For this example, init the ADC

    //����ADCʱ��
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;     // FCLK = HSPCLK / (2 * ADCCLKPS) = 12.5MHz
    AdcRegs.ADCTRL1.bit.CPS = 0;          // ADCCLK = FCLK / (CPS + 1) = 12.5MHz
    AdcRegs.ADCTRL1.bit.ACQ_PS = 5;       // ��������ÿ��ͨ������ʱ�� =(2+ACQ_PS)*ADCCLK

    //����ת��ģʽ
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0;     // ����/ֹͣģʽ
    AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;     // ��������������
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;     // ����������ģʽ
    AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;    // ˳�����ģʽ

    //���ò���ͨ��
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 2; // ������ͨ����Ϊ 3
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;  // ����ADCINA0��Ϊ��1������ͨ��
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;  // ����ADCINA1��Ϊ��2������ͨ��
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2;  // ����ADCINA2��Ϊ��3������ͨ��

    //���ô�����ʽ
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;// ����ePWM�Ĵ����ź�����SEQ1
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // ʹ��SEQ1�ж�
}

