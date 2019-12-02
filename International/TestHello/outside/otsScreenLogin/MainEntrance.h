/**
* @file MainEntrance.h
* @brief 实现main函数直接调用功能
*/
#pragma once
extern int _userType;

namespace otsScreenLogin
{
	/** 外部调用接口，在main函数中调用创建对象 */
	extern int mainEntrance(int argc, char *argv[], bool isLocalResource = false);
}

