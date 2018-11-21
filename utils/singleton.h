#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#define  Declare_Singleton(classname) \
			public: \
				static classname& Instance() \
				{\
					static classname _instance;\
					return _instance;\
				}\
			protected: \
				classname(); \
				virtual ~classname(); \
			private: \
				classname(const classname& rInstance); \
				classname& operator = (const classname& rInstance)

#endif

