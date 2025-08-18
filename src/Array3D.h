#pragma once

template <typename T>
class Array3D
{
public:
	Array3D() : X(0), Y(0), Z(0), Data(nullptr) {}
	Array3D(unsigned x, unsigned y, unsigned z)
	{
		Init(x, y, z);
	}
	
	void Init(unsigned x, unsigned y, unsigned z)
	{
		X = x;
		Y = y;
		Z = z;
		Data = new T[X * Y * Z];
	}

	void Delete()
	{
		delete[] Data;
	}

	T& at(int x, int y, int z)
	{
		return Data[x * Y * Z + y * Z + z];
	}

	const T& at(int x, int y, int z) const
	{
		return Data[x * Y * Z + y * Z + z];
	}

private:
	T* Data;
	unsigned X, Y, Z;
};
