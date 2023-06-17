#ifndef __OBJECT_H
#define __OBJECT_H

#include "tree.h"
#include "vector.h"
#include "camera.h"

/*
	OBJECT.H

  	The CObject class

	OpenGL Game Programming
	Author: Kevin Hawkins
	Date: 3/29/2001
	Description: The CObject class is derived from CNode and
			   represents an object in the game world.

*/

class CCamera;

class CObject : public CNode
{
protected:
	bool m_bDelete;

	// perform basic physics on the object
	virtual void OnAnimate(scalar_t deltaTime) {}

	// draw the object given the camera position
	virtual void OnDraw(CCamera *camera) {}	

	// collide with other objects
	virtual void OnCollision(CObject *collisionObject) {}

	// perform collision and other preparations for this object
	virtual void OnPrepare() {}

public:
	CVector m_vecPosition;		// object position
	float m_fSize;				// size of bounding sphere (radius)
	float m_faRotMatrix[16];	// rotation matrix
	int m_iIdent;				// identifier
	CObject() { m_bDelete = false; }
	~CObject() {}

	virtual void Load() {}
	virtual void Unload() {}

	// draw object
	void Draw(CCamera *camera) 
	{
		// push modelview matrix on stack
		glPushMatrix();
			OnDraw(camera);		// draw this object
			if (HasChild())		// draw children
				((CObject*)childNode)->Draw(camera);
		glPopMatrix();

		// draw siblings
		if (HasParent() && !IsLastChild())
			((CObject*)nextNode)->Draw(camera);
	}

	// animate object
	void Animate(scalar_t deltaTime) 
	{
		OnAnimate(deltaTime);		// animate this object
	
		// animate children
		if (HasChild())
			((CObject*)childNode)->Animate(deltaTime);

		// animate siblings
		if (HasParent() && !IsLastChild())
			((CObject*)nextNode)->Animate(deltaTime);

		if (m_bDelete)
			delete this;
	}

	// perform collision detection
	void ProcessCollision(CObject *obj) 
	{
		OnCollision(obj);
	}

	// prepare object
	void Prepare()
	{
		OnPrepare();						// prepare this object

		if (HasChild())					// prepare children
			((CObject*)childNode)->Prepare();

		if (HasParent() && !IsLastChild())		// prepare siblings
			((CObject*)nextNode)->Prepare();
	}

	// find root object of cyclic linked list tree
	CObject *FindRoot()
	{
		// if this object has a parent node, return the root of the parent node
		if (parentNode)
			return ((CObject*)parentNode)->FindRoot();

		return this;
	}
};


#endif