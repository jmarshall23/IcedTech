#pragma once

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

extern uint16_t FloatToFloat16(float value);
extern float Float16ToFloat(uint16_t value);

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

class Float16
{
protected:
	short mValue;
public:
	Float16();
	Float16(float value);
	Float16(const Float16& value);

	operator float();
	operator float() const;

	friend Float16 operator + (const Float16& val1, const Float16& val2);
	friend Float16 operator - (const Float16& val1, const Float16& val2);
	friend Float16 operator * (const Float16& val1, const Float16& val2);
	friend Float16 operator / (const Float16& val1, const Float16& val2);

	Float16& operator =(const Float16& val);
	Float16& operator +=(const Float16& val);
	Float16& operator -=(const Float16& val);
	Float16& operator *=(const Float16& val);
	Float16& operator /=(const Float16& val);
	Float16& operator -();
};

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16::Float16()
{
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16::Float16(float value)
{
	mValue = FloatToFloat16(value);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16::Float16(const Float16 &value)
{
	mValue = value.mValue;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16::operator float()
{
	return Float16ToFloat(mValue);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16::operator float() const
{
	return Float16ToFloat(mValue);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator =(const Float16& val)
{
	mValue = val.mValue;
	return *this;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator +=(const Float16& val)
{
	*this = *this + val;
	return *this;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator -=(const Float16& val)
{
	*this = *this - val;
	return *this;

}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator *=(const Float16& val)
{
	*this = *this * val;
	return *this;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator /=(const Float16& val)
{
	*this = *this / val;
	return *this;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16& Float16::operator -()
{
	*this = Float16(-(float)*this);
	return *this;
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/
/*+----+                                 Friends                                       +----+*/
/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16 operator + (const Float16& val1, const Float16& val2)
{
	return Float16((float)val1 + (float)val2);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16 operator - (const Float16& val1, const Float16& val2)
{
	return Float16((float)val1 - (float)val2);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16 operator * (const Float16& val1, const Float16& val2)
{
	return Float16((float)val1 * (float)val2);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

inline Float16 operator / (const Float16& val1, const Float16& val2)
{
	return Float16((float)val1 / (float)val2);
}

/*+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+*/

ID_INLINE uint16_t FloatToFloat16(float value)
{
	uint16_t   fltInt16;
	int     fltInt32;
	memcpy(&fltInt32, &value, sizeof(float));
	fltInt16 = ((fltInt32 & 0x7fffffff) >> 13) - (0x38000000 >> 13);
	fltInt16 |= ((fltInt32 & 0x80000000) >> 16);

	return fltInt16;
}

ID_INLINE float Float16ToFloat(uint16_t fltInt16)
{
	int fltInt32 = ((fltInt16 & 0x8000) << 16);
	fltInt32 |= ((fltInt16 & 0x7fff) << 13) + 0x38000000;

	float fRet;
	memcpy(&fRet, &fltInt32, sizeof(float));
	return fRet;
}
