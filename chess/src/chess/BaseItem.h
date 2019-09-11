#pragma once

class BaseItem
{
public:
	enum STATE
	{
		STATIC = 0,
		MOVEMENT = 1,
	};
public:
	BaseItem() : m_state( BaseItem::STATE::STATIC ) {};
	~BaseItem() {};
	const STATE getState() const;
protected:
	void setState( const STATE state );
private:
	STATE m_state;
};

inline void BaseItem::setState( const BaseItem::STATE state )
{
	m_state = state;
}

inline const BaseItem::STATE BaseItem::getState() const
{
	return m_state;
}