/*
===========================================================================

NM-Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the NM-Doom 3 GPL Source Code ("NM-Doom 3 Source Code").

NM-Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

NM-Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with NM-Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the NM-Doom 3 Source Code is also subject to certain additional terms. 
You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the NM-Doom 3 Source Code. 
If not, please request a copy in writing from id Software at the address below.

===========================================================================
*/

#ifndef __NMMAP_H__
#define __NMMAP_H__

#include "sys/platform.h"

/*
===============================================================================

	Map Templante
	a list than can store two values, and compare to get each other
	first value is imutable

===============================================================================
*/

//#include <map>

template<class typeA, class typeB>
class nmMap 
{
public:
		//defalt constructor
		nmMap()
		{
			this->num = 0;
			this->list[0];
		};

		//
		nmMap(const nmMap<typeA, typeB> &other)
		{
			*this = other;
		};
		
		~nmMap<typeA, typeB>(void)
		{
			Clear();
		};

		//get the second element by second 
		typeB		getByFirst(typeA first)
		{
			int index;
			int length = sizeof(typeB) / sizeof(list);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecA == first)
					index = i;
			}

			assert(index >= 0);
			assert(index < num);
			return list[index].VecB;
		};

		typeA		getBySecond(typeB second)
		{
			int index;
			int length = sizeof(this->list) / sizeof(typeB);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecB == second)
					index = i;
			}

			assert(index >= 0);
			assert(index < num);
			return list[index].VecA;
		};
		
		//set based in the input
		void		setByFirst(typeA MapElement, typeB Input)
		{
			int length = sizeof(list) / sizeof(typeB);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecA == MapElement)
					list[i].VecB == Input;	
			}
		};
		void		setBySecond(typeB MapElement, typeB Input)
		{
			int length = sizeof(list) / sizeof(typeB);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecB == MapElement)
					list[i].VecA == Input;
			}
		};

		//get the first element by the index
		typeA		getFirstAt(int idx)
		{
			assert(idx < num);
			return list[idx].VecA;
		};

		//get the second element by the index
		typeB		getSecondAt(int idx)
		{
			assert(idx < num);
			return list[idx].VecB;
		};

		//Append element at end of the list
		int			AppendElements(const typeA first, const typeB second)
		{
			Map_s	obj = Map_s(first, second);

			list[this->num] = obj;
			this->num++;

			return this->num - 1;
		};

		int			Insert(const typeA first, const typeB second, int index)
		{
			if (index < 0)
			{
				index = 0;
			}
			else if (index > this->num)
			{
				index = this->num;
			}
			for (int i = this->num; i > index; --i)
			{
				list[i] = list[i - 1];
			}
			this->num++;
			list[index] = obj;
			return index;
		};

		// marks the list as empty.  does not deallocate or intialize data.
		void		Clear(void)
		{
			if (this->list == NULL)
			{
				delete[] this->list;
			}
				this->list = NULL;
				this->num = 0;
		};

	
		// returns number of elements in list
		int					Num( void ) const
		{return this->num}

#if 0
		const typeB &		operator[](typeA elementToGet) const
		{
			int index;
			int length = sizeof(this->list) / sizeof(typeB);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecA == elementToGet)
				{
					index = i;
				}
			}

			assert(index >= 0);
			assert(index < num);
			return list[index].VecB;
		};

		typeB &				operator[](typeA index)
		{
			int index;
			int length = sizeof(this->list) / sizeof(typeB);
			for (size_t i = 0; i < length; i++)
			{
				if (list[i].VecA == elementToGet)
				{
					index = i;
				}
			}

			assert(index >= 0);
			assert(index < num);
			return list[index].VecB;
		}
#endif

	// remove the element at the given index
	bool				RemoveIndex(int index)
	{
		int i;
		assert(this->list != NULL);
		assert(index >= 0);
		assert(index < this->num);

		if ((index < 0) || (index >= this->num)) 
		{
			return false;
		}

		this->num--;
		for (i = index; i < this->num; i++) 
		{
			this->list[i] = this->list[i + 1];
		}

		return true;
	}

	//void				SetNum( int newnum );								// set number of elements in list
	size_t				Size( void ) const;									// returns total size of allocated memory including size of list type
	//int					Max( void ) const;									// returns the maximum number of elements in the list

	//size_t				Allocated( void ) const;							// returns total size of allocated memory
	//size_t				MemoryUsed( void ) const;							// returns size of the used elements in the list


	//bool				Remove( const type & obj );							// remove the element
	//void				DeleteContents( bool clear );						// delete the contents of the list

private:

	//structure than hold the 2 values
	struct Map_s
	{
		typeA	VecA;
		typeB	VecB;

		Map_s(){};

		Map_s(typeA	_VecA, typeB	_VecB) { VecA = _VecA; VecB = _VecB; };
	};

	int					size;
	int					num;
	Map_s				list[]; //element lists
};

#if FIX_THIS
template<class typeA, class typeB>
ID_INLINE nmMap<typeA, typeB>::idStaticList()
{
	num = 0;
}

template<class typeA, class typeB>
ID_INLINE nmMap<typeA, typeB>::nmMap(const nmMap<typeA, typeB> &other)
{
	*this = other;
}

template<class typeA, class typeB>
ID_INLINE nmMap<typeA, typeB>::~nmMap(void) 
{
	Clear();
}

template<class typeA, class typeB>
ID_INLINE nmMap<typeA, typeB>::Clear(void) 
{
	if (list) 
	{
		delete[] list;
	}

	list = NULL;
	num = 0;
	size = 0;
}

#pragma GCC diagnostic push
// shut up GCC's stupid "warning: assuming signed overflow does not occur when assuming that
// (X - c) > X is always false [-Wstrict-overflow]"
#pragma GCC diagnostic ignored "-Wstrict-overflow"
/*
================
idList<type>::operator[] const

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template<class typeA, class typeB>
ID_INLINE const typeB &nmMap<typeA, typeB>::operator[](typeA typindex) const
{
	unsigned int i;
	unsigned int length = sizeof(list) / sizeof(Map_s);//get the list bytes size
	for ( i = 0; i < length; i++)
	{
		if (list[i].VecA == typindex)
		{
			break;
			//return list[i].VecB;
		}
	}
	assert(i >= 0);
	return list[i].VecB;
}

template<class typeA, class typeB>
ID_INLINE type &nmMap<typeA, typeB>::operator[](typeA typindex)
{
	unsigned int i;
	unsigned int length = sizeof(list) / sizeof(Map_s);//get the list bytes size
	for (i = 0; i < length; i++)
	{
		if (list[i].VecA == typindex)
		{
			break;
			//return list[i].VecB;
		}
	}

	assert(i >= 0);
	return list[i].VecB;
}
#pragma GCC diagnostic pop
#endif

#endif /* !__NMMAP_H__ */
