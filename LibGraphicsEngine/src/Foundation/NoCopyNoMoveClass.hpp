#ifndef FOUNDATION_NO_COPY_NO_MOVE_CLASS_HPP
#define FOUNDATION_NO_COPY_NO_MOVE_CLASS_HPP

// macro that helps make classes non copyable and non movable
#define NO_COPY_NO_MOVE_CLASS(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
	TypeName& operator = (const TypeName&) = delete; \
	TypeName& operator = (TypeName&&) = delete;

#endif /* FOUNDATION_NO_COPY_NO_MOVE_CLASS_HPP */