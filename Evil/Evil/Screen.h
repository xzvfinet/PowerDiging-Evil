#pragma once

//#include "GLApp.h"
#include <GLFW\glfw3.h>
#include <vector>
#include <ctime>

#include "Character.h"
#include "Object.h"
#include "Matrices.h"
#include "GLUtil.h"


using namespace std;

class Screen
{
public:
	Screen()
	{
		character = GS::character;
	}

	virtual ~Screen()
	{
	}

	virtual void update(double delta) = 0
	{
		character->RefreshCamera();

		currentTime = time(NULL);

		if (GS::character->getLife() == 0)
			GS::setGameEnd(true);

		for (size_t i = 0; i < objs.size(); i++)
		{
			if (GS::character->collision_check(objs[i].get_box(), Vector3(0.f, 0.f, 0.f))
				|| objs[i].get_box().collision_detection_3D(GS::character->get_box(), Vector3(0.f, 0.f, 0.f))) // 충돌 했을때 하면 true, 아니면 false 이걸로 뭘 할진 생각해 보자.
			{
				if (objs[i].get_type() == 1 && (currentTime - crashTime) > 2)
				{
					crashTime = currentTime;
					GS::character->setLife(GS::character->getLife() - 1);
					cout << "현재 라이프 = " << (int)GS::character->getLife() << endl;
				}
			}

		}
	}

	virtual void render() = 0
	{
		//cout << "Screen.h의 render()" << endl;s
		//cout << "여기에 모든 Screen에서 적용할 model, view 행렬을 호출하면 된다." << endl;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		Matrix4 look = GLUtil::LookAt(GS::character->getPosition(), GS::character->getLook(), Vector3(0, 1, 0));
		Matrix4 perspective = GLUtil::perspective(90.f, 1, 0.01f, 100.0f);
		glLoadIdentity();
		glMultMatrixf(perspective.get());
		glMultMatrixf(look.get());

		for (size_t i = 0; i < objs.size(); i++) {
			//			glMultMatrixf(objs[i].get_matrix().get());
			objs[i].draw();
			//			glMultMatrixf(objs[i].get_matrix().invert().get());
		}
	}

	GLuint getTextureId(string name)
	{

	}

	void getDamaged()
	{

	}

	inline void Character_Objects_Collision_Detection()
	{
		for (int i = 0; i < objs.size(); i++)
		{
			if (objs[i].get_passable() == false)
			{
				if (character->collision_check(objs[i].get_box(), character->getCdelta(0))
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(0)))
				{
					character->setFrontCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(1))
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(1)))
				{
					character->setBackCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(2))
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(2)))
				{
					character->setLeftCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(3))
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(3)))
				{
					character->setRightCollision(true);
				}
			}
		}
	}

	inline void Character_Room_Collision_Detection()
	{
		if (!roomBox.collision_detection_in(character->get_box(), character->getCdelta(0)))
		{
			character->setFrontCollision(true);
		}
		else
			character->setFrontCollision(false);

		if (!roomBox.collision_detection_in(character->get_box(), character->getCdelta(1)))
		{
			character->setBackCollision(true);
		}
		else
			character->setBackCollision(false);

		if (!roomBox.collision_detection_in(character->get_box(), character->getCdelta(2)))
		{
			character->setLeftCollision(true);
		}
		else
			character->setLeftCollision(false);

		if (!roomBox.collision_detection_in(character->get_box(), character->getCdelta(3)))
		{
			character->setRightCollision(true);
		}
		else
			character->setRightCollision(false);
	}

	void Object_Tracking_Character(int index)
	{
		if (character->collision_check(objs[index].get_box(), Vector3(0, 0, 0))
			|| objs[index].get_box().collision_detection_3D(character->get_box(), Vector3(0, 0, 0)))
		{
			objs[index].setTracking(false);
		}
		else
		{
			objs[index].setTracking(true);
			objs[index].trackpos(character->getPosition());
		}
	}

protected:
	Character* character;
	std::vector<Object> objs;
	Box roomBox;

private:
	time_t currentTime;
	time_t crashTime = 0;
};