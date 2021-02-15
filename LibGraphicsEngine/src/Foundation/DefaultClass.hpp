#ifndef FOUNDATION_DEFAULT_CLASS_HPP
#define FOUNDATION_DEFAULT_CLASS_HPP

// macro that helps make classes non copyable and non movable
#define DEFAULT_CLASS(TypeName) \
	TypeName() = default; \
	TypeName(const TypeName&) = default; \
	TypeName(TypeName&&) = default; \
	TypeName& operator = (const TypeName&) = default; \
	TypeName& operator = (TypeName&&) = default; \
	virtual ~TypeName() = default;

#endif /* FOUNDATION_DEFAULT_CLASS_HPP */