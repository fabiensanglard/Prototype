#pragma once

// Usage:	UTIL_xxxxx<typename>(x,x)
typedef unsigned short ushort;

template<typename _T>
bool UTIL_IsBitSet(_T t, _T flag)
{
	if(t & flag)
		return true;
	return false;
}
template<typename _T>
void UTIL_SetBit(_T& t, _T flags)
{
	t |= flags;
}
template<typename _T>
void UTIL_ClearBit(_T& t, _T flags)
{
	t &= ~flags;
}
template<typename _T>
void UTIL_ClearBits(_T& t)
{
	t=(_T)0;
}
template<typename _T>
void UTIL_ToggleBits(_T& t, _T flags)
{
	t^=flags;	
}
template<typename _T>
bool UTIL_AllBitsOff(_T t)
{
	if(t==_T(0))
		return true;
	return false;
}
template<typename _T>
bool UTIL_AllBitsOn(_T t)
{
	if(t==~_T(0))
		return true;
	return false;
}