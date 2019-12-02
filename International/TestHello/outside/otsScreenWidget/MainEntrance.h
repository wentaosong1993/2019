/**
* @file MainEntrance.h
* @brief 实现main函数直接调用功能
*/
#ifndef OTSDISPLAY_MainEntrance_H
#define OTSDISPLAY_MainEntrance_H

namespace otsScreenWidget
{
	/** 外部调用接口，在main函数中调用创建对象 */
	extern int mainEntrance(int argc, char *argv[], bool isLocalResource = false);
}

#endif // OTSDISPLAY_MainEntrance_H
