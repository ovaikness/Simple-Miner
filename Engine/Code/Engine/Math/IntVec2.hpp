#pragma once


//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

	static IntVec2 const ZERO;
	static IntVec2 const UP;
	static IntVec2 const DOWN;
	static IntVec2 const LEFT;
	static IntVec2 const RIGHT;
public:
	// Construction/Destruction
	~IntVec2() {}												// destructor (do nothing)
	IntVec2() {}												// default constructor (do nothing)
	IntVec2(const IntVec2& copyFrom);							// copy constructor (from another IntVec2)
	explicit IntVec2(int initialX, int initialY);		// explicit constructor (from x, y)

															// Operators (const)
	bool		operator==(const IntVec2& compare) const;		// IntVec2 == IntVec2
	bool		operator!=(const IntVec2& compare) const;		// IntVec2 != IntVec2
	IntVec2	const operator+(const IntVec2& vecToAdd) const;		// IntVec2 + IntVec2
	IntVec2	const operator-(const IntVec2& vecToSubtract) const;	// IntVec2 - IntVec2
	IntVec2	const operator-() const;								// -IntVec2, i.e. "unary negation"
	IntVec2	const operator*(int uniformScale) const;			// IntVec2 * int
	IntVec2	const operator*(const IntVec2& vecToMultiply) const;	// IntVec2 * IntVec2
	IntVec2	const operator/(int inverseScale) const;			// IntVec2 / int

																// Operators (self-mutating / non-const)
	void		operator+=(const IntVec2& vecToAdd);				// IntVec2 += IntVec2
	void		operator-=(const IntVec2& vecToSubtract);		// IntVec2 -= IntVec2
	void		operator*=(const int uniformScale);			// IntVec2 *= int
	void		operator/=(const int uniformDivisor);		// IntVec2 /= int
	void		operator=(const IntVec2& copyFrom);				// IntVec2 = IntVec2
	bool		operator<(const IntVec2& compare) const;

																// Standalone "friend" functions that are conceptually, but not actually, part of IntVec2::
	friend const IntVec2 operator*(int uniformScale, const IntVec2& vecToScale);	// int * IntVec2

	void SetFromText(char const* text);
	float GetLength() const;
	int GetLengthSquared() const;
	int GetTaxicabLength() const;
	float GetOrientationDegrees() const;
	float GetOrientationRadians() const;
	IntVec2 GetRotated90Degrees() const;
	IntVec2 GetRotatedMinus90Degrees() const;
	void Rotate90Degrees();
	void RotateMinus90Degrees();
};
