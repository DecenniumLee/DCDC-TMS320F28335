/*
 * epwm_0.c
 *
 *  Created on: 2022��3��23��
 *      Author: ZGJ
 */

#include "epwm_0.h"

void Setup_ePWM1(void)
{
    // ����ʱ��
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;  //Disable TBCLK within the ePWM
    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
    EDIS;

    // �����������
    InitEPwm1Gpio();

    //ʱ��ģ��TB
    EPwm1Regs.TBPRD = CPU_CLK/(2*PWM_CLK);  // ����ֵ SP = CPU_CLK/(2*PWM_CLK)
    EPwm1Regs.TBPHS.half.TBPHS = 0;         // ��λΪ0
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;      // ʱ��ʱ�� TBCLK=SYSCLKOUT=1/150M
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;          // ������λ�Ĵ���
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;        // ��������

    //�����Ƚ�ģ��CC
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // Ӱ��װ��ģʽ
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // CTR = 0 �� CTR = PRDʱװ��
    EPwm1Regs.CMPA.half.CMPA =0;        // ���ó�ʼռ�ձ�Ϊ0

    //����ģ��AQ
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;       // ���ϼ�����CTR=CMPAʱ��ePWM1A�õ�
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;       // ���¼�����CTR=CMPAʱ��ePWM1A�ø�

    //��������ģ��DB
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;    // ePWM1A��˫��������
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;     // ePWM1A����ת��ePWM1B��ת
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;   // ʹ��˫������ʱ
    EPwm1Regs.DBRED = 30;               // ��������ʱ DBRED*TBCLK = 200ns
    EPwm1Regs.DBFED = 30;               // �½�����ʱ DBRED*TBCLK = 200ns

    //��������ģ��TZ
    EALLOW;
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;        // �����¼�����ʱ��ǿ��ePWM1A��״̬
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;        // �����¼�����ʱ��ǿ��ePWM1B��״̬
    EDIS;

    // ����ʱ����ģ��ʱ������������
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

}

void Setup_ePWM2(void)  // ePWM2ģ�������Ӻ���
{

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;  //Disable TBCLK within the ePWM
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
    EDIS;

    // �����������
    InitEPwm2Gpio();

    //ʱ��ģ��TB
    EPwm2Regs.TBPRD = CPU_CLK/(2*PWM_CLK);             // ����ֵ������ ePWM1 ����ͬ
    EPwm2Regs.TBPHS.half.TBPHS = 0;     // ��λΪ 0
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;  // ʱ��ʱ�� TBCLK=SYSCLKOUT=1/150M
    EPwm2Regs.TBCTL.bit.CLKDIV = 0;
    EPwm2Regs.TBCTL.bit.PHSEN = 0;      // ������λ�Ĵ���
    EPwm2Regs.TBCTL.bit.CTRMODE = 2;    // ��������

    //�����Ƚ�ģ��CC
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // Ӱ��װ��ģʽ
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // CTR = 0 �� CTR = PRDʱװ��
    EPwm2Regs.CMPA.half.CMPA =0;        // ���ó�ʼռ�ձ�Ϊ0

    //����ģ��AQ
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;       // ���ϼ�����CTR=CMPAʱ��ePWM2A�õ�
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;       // ���¼�����CTR=CMPAʱ��ePWM2A�ø�

    //��������ģ��DB
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;    // ePWM2A��˫��������
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;     // ePWM2A����ת��ePWM2B��ת
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;   // ʹ��˫������ʱ
    EPwm2Regs.DBRED = 30;               // ��������ʱ DBRED*TBCLK = 200ns
    EPwm2Regs.DBFED = 30;               // �½�����ʱ DBRED*TBCLK = 200ns

    //��������ģ��TZ
    EALLOW;
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO;        // �����¼�����ʱ��ǿ��ePWM2A��״̬
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;        // �����¼�����ʱ��ǿ��ePWM2B��״̬
    EDIS;

    //�¼�����ģ��ET
    EPwm2Regs.ETSEL.bit.SOCAEN = 1;     // ʹ�� ePWM2SOCA �źŲ���
    EPwm2Regs.ETSEL.bit.SOCASEL = 1;    // ��TBCTR=0ʱ���� ePWM2SOCA�ź�
    EPwm2Regs.ETPS.bit.SOCAPRD = 1;     // �ڵ�1���¼�ʱ���� ePWM2SOCA�ź�

    // ����ʱ����ģ��ʱ������������
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

