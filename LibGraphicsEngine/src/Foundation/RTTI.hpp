#ifndef FOUNDATION_RTTI_HPP
#define FOUNDATION_RTTI_HPP

#include "Foundation/TypeDefines.hpp"

namespace GraphicsEngine
{
	class RTTI
	{
		protected:
			RTTI(void) { }

		public:
			virtual ~RTTI(void) { }

		/**
			\brief Get the class name
		*/
		virtual const char_t* GetClassName_(void) const = 0;
	};
}

#define GE_RTTI( X ) \
    public: \
        static constexpr const char_t* __CLASS_NAME = #X; \
        virtual const char_t* GetClassName_( void ) const override { return __CLASS_NAME; }

#endif // FOUNDATION_RTTI_HPP