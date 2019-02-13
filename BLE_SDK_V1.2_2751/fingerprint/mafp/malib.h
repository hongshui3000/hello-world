/*
 * malib.h
 * Copyright (C) MicroArray Microelectronics Corp.,LTD 2016 All rights reserved
 *
 * @Author: AK <ywj@microarray.com.cn>
 * @Date:   2016-12-02 15:02:44
 * @Last Modified by:   AK
 * @Last Modified time: 2016-12-19 10:29:52
 * 
 * @Desription 
 */

#ifndef MALIB_H
#define MALIB_H

#include "mafp_defs.h"

typedef unsigned char uchar;
typedef unsigned short WORD; // ����WORDΪ�����ֽڵ�����
typedef unsigned long DWORD; // ����DWORDΪe�ĸ��ֽڵ�����


struct BMP_FILE_HEADER { // λͼ�ļ�ͷ
	WORD bType; // �ļ���ʶ��
	DWORD bSize; // �ļ��Ĵ�С
	WORD bReserved1; // ����ֵ,��������Ϊ0
	WORD bReserved2; //  ����ֵ,��������Ϊ0
	DWORD bOffset; // �ļ�ͷ�����ͼ������λ��ʼ��ƫ����
};

struct BMP_INFO { // λͼ��Ϣͷ
	DWORD bInfoSize; // ��Ϣͷ�Ĵ�С
	DWORD bWidth; // ͼ��Ŀ��
	DWORD bHeight; // ͼ��ĸ߶�
	WORD bPlanes; // ͼ���λ����
	WORD bBitCount; // ÿ�����ص�λ��
	DWORD bCompression; // ѹ������
	DWORD bmpImageSize; // ͼ��Ĵ�С,���ֽ�Ϊ��λ
	DWORD bXPelsPerMeter; // ˮƽ�ֱ���
	DWORD bYPelsPerMeter; // ��ֱ�ֱ���
	DWORD bClrUsed; // ʹ�õ�ɫ����
	DWORD bClrImportant; // ��Ҫ����ɫ��
};

struct RGBQUAD { // ��ɫ��
	uchar rgbBlue; // ��ɫǿ��
	uchar rgbGreen; // ��ɫǿ��
	uchar rgbRed; // ��ɫǿ��
	uchar rgbReserved; // ����ֵ
};


extern void printn(uint8_t *buf, int32_t len);
extern void printn_int(int32_t *buf, int32_t len);
extern int  save_bmp_file(uchar *file, int file_size, uchar *buf, int w, int h);
extern int  load_bmp_body(uchar *file, uchar *buf, int buf_size, int *w, int *h);

extern uint8_t CRC8(uint8_t *dat, uint32_t len);
extern uint32_t getHashIndex(const uint8_t *data);

#endif

