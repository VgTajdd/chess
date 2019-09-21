#pragma once

class BaseItem
{
public:
	const int static UNDEFINED = 0;
	const int static STAND = 1;
public:
	BaseItem() : m_state( BaseItem::STAND ) {};
	~BaseItem() {};
	const int getState() const;
	virtual void gotoState( const int state );
protected:
	void setState( const int state );
private:
	int m_state;
};

inline void BaseItem::setState( const int state )
{
	m_state = state;
}

inline const int BaseItem::getState() const
{
	return m_state;
}

inline void BaseItem::gotoState( const int state )
{
	m_state = state;
}