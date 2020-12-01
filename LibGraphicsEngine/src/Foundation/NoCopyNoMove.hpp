#ifndef FOUNDATION_NO_COPY_NO_MOVE_HPP
#define FOUNDATION_NO_COPY_NO_MOVE_HPP

// macro that helps make classes non copyable and non movable
#define NO_COPY_NO_MOVE(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName(const TypeName&&) = delete; \
	void operator =(const TypeName&) = delete; \
	void operator =(const TypeName&&) = delete;

#endif /* FOUNDATION_NO_COPY_NO_MOVE_HPP */