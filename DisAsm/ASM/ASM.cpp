﻿// ASM.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <iomanip>
#include "CAsmEngine.h"

CAsmEngine TheEngine;

int main()
{
	// 随便瞎打的代码
	static byte AsmTestCode[] = {
		0x89,0xD8,0x89,0xC8,0x8B,0x87,0x22,0x11,0x8B,0x47,0xF0,
		0xC7,0x87,0x10,0x10,0x66,0x66,0xA1,0x44,0x88,0xB0,0x20,
		0xA0,0xE0,0xFF,0x8E,0x1E,0x20,0x01,0x53,0x1E,0x8F,0x47,
		0x55,0x8F,0x47,0xAB,0x87,0x1E,0x30,0x20,0x93,0xE4,0x66,
		0xEF,0xD7,0x8D,0x47,0x20,0xC5,0x5F,0x20,0xC4,0x0E,0x68,
		0x06,0x00,0x00,0x05,0x65,0x00,0x05,0x50,0x66,0x04,0x50,
		0x05,0x80,0x00,0x04,0x88,0x14,0x44,0x15,0x77,0x88,
		0x2D,0x55,0x00,0x37,0x27,0x49,0x3F,0x2F,
		0x3D,0x66,0x06,0x3B,0x1E,0x88,0x00,
		0x39,0x06,0x84,0x04,0xD1,0xE0,0xD1,0xE3,
		0xD0,0xE0,0xD3,0xE0,0xD3,0xC8,0xD1,0xDA,0xAD,0xAC,
		0x9D,0xF3,0xA4,
		0x9A,0x20,0x01,0x20,0x08,
		0x9A,0x80,0x00,0x16,0x00,
		0xE8,0x83,0x00,0xCA,0x55,0x12,
		0xD9,0x45,0x00,0xFF,0x5F,0x20,0xFF,0x2F,0xFF,0x27,
		0x81,0x81,0x48,0x88,0x27,0x95,0xD5,0x0A,0x84,0x99,0x20,
		0x00,0x48,0xac,0x51,0x47,0x00,0x00,0x00
	};

	// 作业代码编译完过后捞出来看看还原度
	static byte HomeWorkCode[] = {
	0xB8, 0x00, 0x00, 0x8E, 0xD8, 0x8D, 0x16, 0x00, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0x8D, 0x16, 0x3B,
	0x00, 0xB4, 0x0A, 0xCD, 0x21, 0xA0, 0x3C, 0x00, 0x04, 0x02, 0xB4, 0x00, 0x8B, 0xF0, 0xC6, 0x84,
	0x3B, 0x00, 0x24, 0x8D, 0x16, 0x3C, 0x01, 0xB4, 0x09, 0xCD, 0x21, 0x8D, 0x16, 0x18, 0x00, 0xB4,
	0x09, 0xCD, 0x21, 0x8D, 0x16, 0x3D, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0x8D, 0x16, 0x3C, 0x01, 0xB4,
	0x09, 0xCD, 0x21, 0x8D, 0x16, 0x28, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0x8A, 0x16, 0x3C, 0x00, 0xB1,
	0x04, 0xD2, 0xEA, 0x80, 0xFA, 0x0A, 0x72, 0x03, 0x80, 0xC2, 0x07, 0x80, 0xC2, 0x30, 0xB4, 0x02,
	0xCD, 0x21, 0x8A, 0x16, 0x3C, 0x00, 0x80, 0xE2, 0x0F, 0x80, 0xFA, 0x0A, 0x72, 0x03, 0x80, 0xC2,
	0x07, 0x80, 0xC2, 0x30, 0xCD, 0x21, 0x8D, 0x16, 0x3C, 0x01, 0xB4, 0x09, 0xCD, 0x21, 0xB4, 0x4C,
	0xCD, 0x21
	};

	// 项目代码
	static byte data[] = {
	0x55, 0x8B, 0xEC, 0x52, 0x8B, 0x56, 0x04, 0xB4, 0x09, 0xCD, 0x21, 0x5A, 0x5D, 0xC3, 0xB4, 0x01,
	0xCD, 0x21, 0xC3, 0x55, 0x8B, 0xEC, 0x52, 0xB4, 0x02, 0x8B, 0x56, 0x04, 0xCD, 0x21, 0x5A, 0x5D,
	0xC3, 0x55, 0x8B, 0xEC, 0x51, 0x52, 0x8B, 0x56, 0x04, 0xB1, 0x04, 0xD2, 0xEA, 0x80, 0xFA, 0x0A,
	0x72, 0x03, 0x80, 0xC2, 0x07, 0x80, 0xC2, 0x30, 0x8A, 0xC2, 0x32, 0xE4, 0x50, 0xE8, 0xD3, 0xFF,
	0x83, 0xC4, 0x02, 0x8B, 0x56, 0x04, 0x80, 0xE2, 0x0F, 0x80, 0xFA, 0x0A, 0x72, 0x03, 0x80, 0xC2,
	0x07, 0x80, 0xC2, 0x30, 0x8A, 0xC2, 0x32, 0xE4, 0x50, 0xE8, 0xB7, 0xFF, 0x83, 0xC4, 0x02, 0x5A,
	0x59, 0x5D, 0xC3, 0x55, 0x8B, 0xEC, 0x51, 0x8B, 0x4E, 0x04, 0x8A, 0xC5, 0x32, 0xE4, 0x50, 0xE8,
	0xAF, 0xFF, 0x83, 0xC4, 0x02, 0x8A, 0xC1, 0x32, 0xE4, 0x50, 0xE8, 0xA4, 0xFF, 0x83, 0xC4, 0x02,
	0x59, 0x5D, 0xC3, 0x55, 0x8B, 0xEC, 0x51, 0x57, 0xB9, 0xFF, 0xFF, 0xB0, 0x24, 0x8B, 0x7E, 0x06,
	0xF2, 0xAE, 0xB8, 0xFF, 0xFF, 0x2B, 0xC1, 0x48, 0x5F, 0x59, 0x5D, 0xCB, 0x55, 0x8B, 0xEC, 0x83,
	0xC4, 0xFC, 0x51, 0x53, 0x56, 0x57, 0xFF, 0x76, 0x06, 0x0E, 0xE8, 0xD6, 0xFF, 0x83, 0xC4, 0x02,
	0x89, 0x46, 0xFE, 0xFF, 0x76, 0x08, 0x0E, 0xE8, 0xC9, 0xFF, 0x83, 0xC4, 0x02, 0x89, 0x46, 0xFC,
	0x83, 0xF8, 0x01, 0x75, 0x22, 0x8B, 0x7E, 0x06, 0x8B, 0x4E, 0xFE, 0x8B, 0x5E, 0x08, 0x8A, 0x07,
	0xFC, 0xAE, 0x74, 0x07, 0x49, 0x75, 0xFA, 0xB0, 0x00, 0xEB, 0x02, 0xB0, 0xFF, 0x0A, 0xC0, 0x75,
	0x04, 0xEB, 0x25, 0xEB, 0x02, 0xEB, 0x2B, 0x39, 0x46, 0xFE, 0x7C, 0x1C, 0x8B, 0x76, 0x06, 0x8B,
	0x7E, 0x08, 0x8B, 0x4E, 0xFC, 0xF3, 0xA6, 0x85, 0xC9, 0x74, 0x17, 0x46, 0x56, 0x2B, 0x76, 0x06,
	0x3B, 0x76, 0xFE, 0x5E, 0x7F, 0x02, 0xEB, 0xE7, 0x33, 0xC0, 0x5F, 0x5E, 0x5B, 0x59, 0x8B, 0xE5,
	0x5D, 0xCB, 0x2B, 0x76, 0xFC, 0x8B, 0xC6, 0x5F, 0x5E, 0x5B, 0x59, 0x8B, 0xE5, 0x5D, 0xCB, 0x55,
	0x8B, 0xEC, 0x53, 0x51, 0x52, 0x8B, 0x1E, 0x46, 0x01, 0xB4, 0x42, 0xB9, 0x00, 0x00, 0x8B, 0x56,
	0x04, 0xB0, 0x00, 0xCD, 0x21, 0x5A, 0x59, 0x5B, 0x5D, 0xC3, 0x55, 0x8B, 0xEC, 0x53, 0x51, 0x52,
	0xFF, 0x76, 0x08, 0xE8, 0xD9, 0xFF, 0x83, 0xC4, 0x02, 0x8B, 0x1E, 0x46, 0x01, 0x8B, 0x4E, 0x06,
	0x8B, 0x56, 0x04, 0xB4, 0x3F, 0xCD, 0x21, 0x5A, 0x59, 0x5B, 0x5D, 0xC3, 0x55, 0x8B, 0xEC, 0x53,
	0x51, 0x52, 0xFF, 0x76, 0x08, 0xE8, 0xB7, 0xFF, 0x83, 0xC4, 0x02, 0x8B, 0x1E, 0x46, 0x01, 0x8B,
	0x4E, 0x06, 0x8B, 0x56, 0x04, 0xB4, 0x40, 0xCD, 0x21, 0x5A, 0x59, 0x5B, 0x5D, 0xC3, 0x33, 0xC0,
	0x50, 0xB8, 0x02, 0x00, 0x50, 0xB8, 0x48, 0x01, 0x50, 0xE8, 0xAE, 0xFF, 0x83, 0xC4, 0x06, 0xC3,
	0x52, 0x53, 0xBA, 0x8C, 0x01, 0xB4, 0x0A, 0xCD, 0x21, 0xBB, 0x8C, 0x01, 0xB8, 0x00, 0x00, 0x8A,
	0x47, 0x01, 0x03, 0xD8, 0xC6, 0x47, 0x02, 0x24, 0x5B, 0x5A, 0xC3, 0x55, 0x8B, 0xEC, 0x83, 0xC4,
	0xF8, 0x56, 0x53, 0x51, 0x52, 0xE8, 0xD8, 0xFF, 0xC7, 0x46, 0xFE, 0x00, 0x00, 0xB8, 0x00, 0x00,
	0x8D, 0x1E, 0x8D, 0x01, 0x8A, 0x07, 0x89, 0x46, 0xFC, 0x89, 0x46, 0xF8, 0x83, 0x46, 0xF8, 0x01,
	0x81, 0x46, 0xF8, 0x8C, 0x01, 0x9C, 0xFD, 0xBA, 0x00, 0x00, 0x8B, 0x76, 0xF8, 0xEB, 0x1F, 0xAC,
	0x8A, 0xD8, 0x80, 0xEB, 0x30, 0x8B, 0xCA, 0xB8, 0x01, 0x00, 0x53, 0xB3, 0x0A, 0xEB, 0x03, 0xF6,
	0xE3, 0x49, 0x83, 0xF9, 0x00, 0x77, 0xF8, 0x5B, 0xF6, 0xE3, 0x01, 0x46, 0xFE, 0x42, 0x3B, 0x56,
	0xFC, 0x72, 0xDC, 0x9D, 0x8B, 0x46, 0xFE, 0x5A, 0x59, 0x5B, 0x5E, 0x8B, 0xE5, 0x5D, 0xC3, 0x55,
	0x8B, 0xEC, 0x53, 0x51, 0x52, 0x56, 0x57, 0xE8, 0x76, 0xFF, 0xBB, 0x8C, 0x01, 0x8D, 0x77, 0x02,
	0x8B, 0x7E, 0x04, 0xB8, 0x00, 0x00, 0x8A, 0x47, 0x01, 0x83, 0xC0, 0x01, 0x8B, 0xC8, 0x9C, 0xFC,
	0xF3, 0xA4, 0x9D, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x5D, 0xC3, 0x53, 0xB8, 0x49, 0x00, 0x50, 0xE8,
	0xD1, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x44, 0x00, 0x50, 0xE8, 0xC7, 0xFD, 0x83, 0xC4, 0x02, 0xB8,
	0x3A, 0x00, 0x50, 0xE8, 0xBD, 0xFD, 0x83, 0xC4, 0x02, 0xFF, 0x36, 0x4A, 0x01, 0xE8, 0x03, 0xFE,
	0x83, 0xC4, 0x02, 0xB8, 0x20, 0x00, 0x50, 0xE8, 0xA9, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x6E, 0x00,
	0x50, 0xE8, 0x9F, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x3A, 0x00, 0x50, 0xE8, 0x95, 0xFD, 0x83, 0xC4,
	0x02, 0x8D, 0x1E, 0x4D, 0x01, 0x53, 0xE8, 0x77, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x20, 0x00, 0x50,
	0xE8, 0x80, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x70, 0x00, 0x50, 0xE8, 0x76, 0xFD, 0x83, 0xC4, 0x02,
	0xB8, 0x3A, 0x00, 0x50, 0xE8, 0x6C, 0xFD, 0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x5D, 0x01, 0x53, 0xE8,
	0x4E, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x20, 0x00, 0x50, 0xE8, 0x57, 0xFD, 0x83, 0xC4, 0x02, 0xB8,
	0x61, 0x00, 0x50, 0xE8, 0x4D, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x3A, 0x00, 0x50, 0xE8, 0x43, 0xFD,
	0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x6D, 0x01, 0x53, 0xE8, 0x25, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x34,
	0x01, 0x50, 0xE8, 0x1B, 0xFD, 0x83, 0xC4, 0x02, 0x5B, 0xC3, 0xB8, 0x00, 0x00, 0x8E, 0xD8, 0x8E,
	0xC0, 0xB4, 0x3D, 0xB0, 0x02, 0xBA, 0x37, 0x01, 0xCD, 0x21, 0x73, 0x0E, 0xB8, 0x08, 0x01, 0x50,
	0xE8, 0xFD, 0xFC, 0x83, 0xC4, 0x02, 0xB4, 0x4C, 0xCD, 0x21, 0xA3, 0x46, 0x01, 0xB8, 0xEC, 0x00,
	0x50, 0xE8, 0xEC, 0xFC, 0x83, 0xC4, 0x02, 0xE8, 0x64, 0xFE, 0xB8, 0x24, 0x01, 0x50, 0xE8, 0xDF,
	0xFC, 0x83, 0xC4, 0x02, 0xFF, 0x36, 0x48, 0x01, 0xE8, 0x38, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x2D,
	0x00, 0x50, 0xE8, 0xDE, 0xFC, 0x83, 0xC4, 0x02, 0xB8, 0x3E, 0x00, 0x50, 0xE8, 0xD4, 0xFC, 0x83,
	0xC4, 0x02, 0xFF, 0x36, 0x4A, 0x01, 0xE8, 0x1A, 0xFD, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50,
	0xE8, 0xAD, 0xFC, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0xA3, 0xFC, 0x83, 0xC4, 0x02,
	0xB8, 0x00, 0x00, 0x50, 0xE8, 0x99, 0xFC, 0x83, 0xC4, 0x02, 0xE8, 0xA1, 0xFC, 0x50, 0xB8, 0x34,
	0x01, 0x50, 0xE8, 0x8B, 0xFC, 0x83, 0xC4, 0x02, 0x58, 0x3C, 0x31, 0x74, 0x03, 0xE9, 0xB1, 0x00,
	0xC6, 0x06, 0x4C, 0x01, 0x01, 0xB8, 0x98, 0x00, 0x50, 0xE8, 0x74, 0xFC, 0x83, 0xC4, 0x02, 0x8D,
	0x1E, 0x4D, 0x01, 0x53, 0xE8, 0x78, 0xFE, 0x83, 0xC4, 0x02, 0x53, 0xE8, 0x62, 0xFC, 0x83, 0xC4,
	0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0x58, 0xFC, 0x83, 0xC4, 0x02, 0xB8, 0xA6, 0x00, 0x50, 0xE8,
	0x4E, 0xFC, 0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x5D, 0x01, 0x53, 0xE8, 0x52, 0xFE, 0x83, 0xC4, 0x02,
	0x53, 0xE8, 0x3C, 0xFC, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0x32, 0xFC, 0x83, 0xC4,
	0x02, 0xB8, 0xB5, 0x00, 0x50, 0xE8, 0x28, 0xFC, 0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x6D, 0x01, 0x53,
	0xE8, 0x2C, 0xFE, 0x83, 0xC4, 0x02, 0x53, 0xE8, 0x16, 0xFC, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01,
	0x50, 0xE8, 0x0C, 0xFC, 0x83, 0xC4, 0x02, 0xBB, 0x40, 0x00, 0xA1, 0x48, 0x01, 0xF6, 0xE3, 0x83,
	0xC0, 0x02, 0xFF, 0x06, 0x48, 0x01, 0xFF, 0x36, 0x48, 0x01, 0x8F, 0x06, 0x4A, 0x01, 0xFF, 0x0E,
	0x4A, 0x01, 0x8B, 0xC8, 0x33, 0xC0, 0x50, 0xB8, 0x02, 0x00, 0x50, 0xB8, 0x48, 0x01, 0x50, 0xE8,
	0x3A, 0xFD, 0x83, 0xC4, 0x06, 0x51, 0x53, 0xB8, 0x4C, 0x01, 0x50, 0xE8, 0x2E, 0xFD, 0x83, 0xC4,
	0x06, 0x3C, 0x32, 0x75, 0x20, 0xBB, 0x40, 0x00, 0xA1, 0x4A, 0x01, 0xFF, 0x0E, 0x4A, 0x01, 0xF6,
	0xE3, 0x83, 0xC0, 0x02, 0x8B, 0xC8, 0x51, 0xB8, 0x01, 0x00, 0x50, 0xB8, 0xAE, 0x01, 0x50, 0xE8,
	0x0A, 0xFD, 0x83, 0xC4, 0x06, 0x3C, 0x33, 0x74, 0x03, 0xE9, 0x01, 0x01, 0xB8, 0x5D, 0x00, 0x50,
	0xE8, 0x9D, 0xFB, 0x83, 0xC4, 0x02, 0xE8, 0xA5, 0xFB, 0x50, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0x8F,
	0xFB, 0x83, 0xC4, 0x02, 0x58, 0xB9, 0x00, 0x00, 0xC7, 0x06, 0x4A, 0x01, 0x00, 0x00, 0x3C, 0x31,
	0x75, 0x34, 0xEB, 0x28, 0xB8, 0x40, 0x00, 0xF7, 0x26, 0x4A, 0x01, 0x83, 0xC0, 0x02, 0x50, 0xB8,
	0x40, 0x00, 0x50, 0xB8, 0x4C, 0x01, 0x50, 0xE8, 0xA0, 0xFC, 0x83, 0xC4, 0x06, 0x80, 0x3E, 0x4C,
	0x01, 0x01, 0x75, 0x03, 0xE8, 0x93, 0xFD, 0xFF, 0x06, 0x4A, 0x01, 0x41, 0x3B, 0x0E, 0x48, 0x01,
	0x72, 0xD2, 0xFF, 0x0E, 0x4A, 0x01, 0x3C, 0x32, 0x75, 0x4B, 0xB8, 0xC6, 0x00, 0x50, 0xE8, 0x3F,
	0xFB, 0x83, 0xC4, 0x02, 0xE8, 0xE4, 0xFC, 0x8B, 0xD8, 0x53, 0xE8, 0x96, 0xFB, 0x83, 0xC4, 0x02,
	0xB8, 0x34, 0x01, 0x50, 0xE8, 0x29, 0xFB, 0x83, 0xC4, 0x02, 0x3B, 0x1E, 0x48, 0x01, 0x73, 0x25,
	0xB8, 0x40, 0x00, 0xF7, 0xE3, 0x83, 0xC0, 0x02, 0x50, 0xB8, 0x40, 0x00, 0x50, 0xB8, 0x4C, 0x01,
	0x50, 0xE8, 0x46, 0xFC, 0x83, 0xC4, 0x06, 0x80, 0x3E, 0x4C, 0x01, 0x01, 0x75, 0x07, 0xE8, 0x39,
	0xFD, 0x89, 0x1E, 0x4A, 0x01, 0x3C, 0x33, 0x75, 0x54, 0xB8, 0xD8, 0x00, 0x50, 0xE8, 0xF0, 0xFA,
	0x83, 0xC4, 0x02, 0xE8, 0x7A, 0xFC, 0xEB, 0x3B, 0xB8, 0x40, 0x00, 0xF7, 0x26, 0x4A, 0x01, 0x83,
	0xC0, 0x02, 0x50, 0xB8, 0x40, 0x00, 0x50, 0xB8, 0x4C, 0x01, 0x50, 0xE8, 0x0C, 0xFC, 0x83, 0xC4,
	0x06, 0x80, 0x3E, 0x4C, 0x01, 0x01, 0x75, 0x16, 0xB8, 0x8E, 0x01, 0x50, 0xB8, 0x4D, 0x01, 0x50,
	0x0E, 0xE8, 0x58, 0xFB, 0x83, 0xC4, 0x04, 0x0B, 0xC0, 0x74, 0x03, 0xE8, 0xEC, 0xFC, 0xFF, 0x06,
	0x4A, 0x01, 0x41, 0x3B, 0x0E, 0x48, 0x01, 0x72, 0xBF, 0xFF, 0x0E, 0x4A, 0x01, 0x3C, 0x34, 0x74,
	0x03, 0xE9, 0x90, 0x00, 0xC6, 0x06, 0x4C, 0x01, 0x01, 0xB8, 0x98, 0x00, 0x50, 0xE8, 0x90, 0xFA,
	0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x4D, 0x01, 0x53, 0xE8, 0x94, 0xFC, 0x83, 0xC4, 0x02, 0x53, 0xE8,
	0x7E, 0xFA, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0x74, 0xFA, 0x83, 0xC4, 0x02, 0xB8,
	0xA6, 0x00, 0x50, 0xE8, 0x6A, 0xFA, 0x83, 0xC4, 0x02, 0x8D, 0x1E, 0x5D, 0x01, 0x53, 0xE8, 0x6E,
	0xFC, 0x83, 0xC4, 0x02, 0x53, 0xE8, 0x58, 0xFA, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8,
	0x4E, 0xFA, 0x83, 0xC4, 0x02, 0xB8, 0xB5, 0x00, 0x50, 0xE8, 0x44, 0xFA, 0x83, 0xC4, 0x02, 0x8D,
	0x1E, 0x6D, 0x01, 0x53, 0xE8, 0x48, 0xFC, 0x83, 0xC4, 0x02, 0x53, 0xE8, 0x32, 0xFA, 0x83, 0xC4,
	0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0x28, 0xFA, 0x83, 0xC4, 0x02, 0xB8, 0x40, 0x00, 0xF7, 0x26,
	0x4A, 0x01, 0x83, 0xC0, 0x02, 0x50, 0xB8, 0x40, 0x00, 0x50, 0xB8, 0x4C, 0x01, 0x50, 0xE8, 0x6B,
	0xFB, 0x83, 0xC4, 0x06, 0x3C, 0x35, 0x75, 0x04, 0xB4, 0x4C, 0xCD, 0x21, 0xB8, 0x34, 0x01, 0x50,
	0xE8, 0xFD, 0xF9, 0x83, 0xC4, 0x02, 0xB8, 0x34, 0x01, 0x50, 0xE8, 0xF3, 0xF9, 0x83, 0xC4, 0x02,
	0xE9, 0x04, 0xFD
	};

	// 写分支时候做参考用
	// TheEngine.PrintClashCode();

#define BYTECODE data

	int offset = 0;
	TheEngine.SetAsmAddress(0);
	while (offset < sizeof(BYTECODE))
	{
		cout << setiosflags(ios::uppercase) << setw(4) << setfill('0') << hex << TheEngine.GetAsmAddress() << " ";
		int n = TheEngine.ReadCode(BYTECODE + offset);
		for (byte i = 0; i < n; i++)
		{
			cout << setiosflags(ios::uppercase) << setw(2) << setfill('0') << hex << (WORD)BYTECODE[offset + i];
		}
		if (n <= 5)
		{
			if (n == 1)
			{
				cout << "\t";
			}
			cout << "\t\t";
		}
		else
		{
			cout << "\t";
		}
		cout << TheEngine.GetDisAsm() << endl;
		offset += n;
	}

	//for (size_t i = 0; i < sizeof(HomeWorkCode); i++)
	//{
	//	cout << setw(2) << setfill('0') << hex << (unsigned short)HomeWorkCode[i] << " ";
	//}

	system("pause");
}