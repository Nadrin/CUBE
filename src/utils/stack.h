/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define CUBE_PUSH Stack.Push(this)
#define CUBE_POP  Stack.Pop(this)

#define CUBE_DECLSTACK(T) static ObjectStack<T> Stack
#define CUBE_STACK(T)     ObjectStack<T> T::Stack

namespace CUBE {

template<class T>
class ObjectStack
{
private:
	std::deque<typename T*> stack;
public:
	void Push(T* object)
	{
		stack.push_back(object);
	}

	void Pop(T* object)
	{
		assert(!stack.empty());
		assert(object == stack.back());
		stack.pop_back();
	}

	T* Current() const
	{
		if(stack.empty())
			return nullptr;
		return stack.back();
	}

	T* Previous() const
	{
		if(stack.size() < 2)
			return nullptr;
		return *(stack.crbegin()+1);
	}

	bool IsEmpty() const
	{
		return stack.empty();
	}
};

} // CUBE