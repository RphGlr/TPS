#pragma once

template <typename T>
class Singleton
{
private:
	static T* singleton;

protected:
	Singleton() {}
	virtual ~Singleton() {}

public:
	static T* instance()
	{
		if (singleton == nullptr)
		{
			singleton = new T;
		}
		
		return (static_cast<T*>(singleton));
	}

	static void kill()
	{
		if (nullptr != singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}
};

template <typename T>
T* Singleton<T>::singleton = nullptr;