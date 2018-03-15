#include "bsp_systick.h"

/* ������ʱ 1us/1ms �δ�ʱ���ļ������� */
static u8 fac_us = 0;
static u16 fac_ms = 0;

/**
 * @brief ��ʼ���δ�ʱ��
 */
void systick_init(void)
{
        /* ���õδ�ʱ����ʱ��Ϊϵͳʱ�ӵ� 8 ��Ƶ */
        SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;

        /* ϵͳʱ��Ϊ SYSTEM_CLOCK, �δ�ʱ����ʱ��Ϊ: ((SYSTEM_CLOCK * 10^6) / 8) */
        /* �������, һ������ֵ��ʱ����: (8 / (SYSTEM_CLOCK * 10^6)) */
        /* ��ô��ʱ 1us �ļ���ֵ����: ((10^-6 * (SYSTEM_CLOCK * 10^6))/ 8) = (SYSTEM_CLOCK / 8) */
        fac_us = SYSTEM_CLOCK / 8;

        /* ��ʱ 1ms �ļ���ֵ�϶����� 1us �� 1000 ���� */
        fac_ms = (u16) fac_us * 1000;
}

/**
 * @brief ��ʱ nus ΢��
 * @param nus ��ʱʱ�� nus <= 1864135us (���ֵ�� 2^24 / fac_us)
 */
void delay_us(u32 nus)
{
        /* ��¼��ȡ�ĵδ�ʱ�����ƼĴ��� CTRL ���ݵ���ʱ���� */
        u32 temp;

        /* ���ö�ʱ������װ��ֵΪ nus * fac_us */
        /* ���ݶ�ʱ��ʱ����δ�ʱ��������װ��ֵ */
        SysTick->LOAD = nus * fac_us;

        /* ���õδ�ʱ���ļ���ֵΪ 0, ��յδ�ʱ�� */
        SysTick->VAL = 0;

        /* �����δ�ʱ��, �˿̵δ�ʱ���Ѿ�ĬĬ���ں�̨��ʼ��ʱ�� */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* �����Ķ�ȡ�δ�ʱ�� COUNTFLAG (CTRL��λ16), �ж϶�ʱ�Ƿ񵽴� */
        /* ��ʱʱ��û�е���֮ǰ����һֱͣ�������ѭ������ */
        do
        {
                temp = SysTick->CTRL;
        } while ((temp & 0x01) && !(temp & (1 << 16)));

        /* �رյδ�ʱ�� */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

        /* ���õδ�ʱ���ļ���ֵΪ 0, ��յδ�ʱ�� */
        SysTick->VAL = 0;
}

/**
 * @brief ��ʱ nms ����
 * @param nms ��ʱʱ�� nms <= 1864ms �㷨ͬ��һ������
 */
static void delay_xms(u16 nms)
{
        u32 temp;
        SysTick->LOAD = (u32) nms * fac_ms;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do
        {
                temp = SysTick->CTRL;
        } while ((temp & 0x01) && !(temp & (1 << 16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0X00;
}

/**
 * @brief ��ʱ nms ����
 * @param nms ��ʱʱ�� nms <= 65535
 */
void delay_ms(u16 nms)
{
        /* ���������ʵ���ǽ� nms �ֽ⿪��, �ֽ��Ϊ��� 1500ms * repeat + remain */
        u8 repeat = nms / 1500;
        u16 remain = nms % 1500;

        while (repeat)
        {
                delay_xms(1500);
                repeat--;
        }

        if (remain)
        {
                delay_xms(remain);
        }
}
