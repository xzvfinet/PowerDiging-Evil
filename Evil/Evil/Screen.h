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
		uiComponent.push_back(hpBar);
		uiComponent[0] = Object(Vector3(0, 8, 0));
		uiComponent[0].loadObj("hp" + to_string(character->getLife()));
	}

	virtual ~Screen()
	{
	}

	virtual void update(double delta) = 0
	{
		if (GS::inCinematic()) {
			character->playCinematic();
			if (character->currentFrame > character->maxFrame - 1) {
				character->alreadyWatched();
				GS::setCinematic(false);
				character->currentFrame = 0;
			}
		}
		else {
			character->RefreshCamera();
		}

		currentTime = time(NULL);

		if (character->getLife() == 0)
			GS::setGameEnd(true);
	}

	virtual void render() = 0
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// UI
		glMatrixMode(GL_PROJECTION);
		perspective = GLUtil::Ortho(-10, 10, -10, 10, 0.01, 100);
		glLoadMatrixf(perspective.get());
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (hpVal != character->getLife()) {
			uiComponent[0] = Object(Vector3(0, 8, 0));
			uiComponent[0].loadObj("hp" + to_string(character->getLife()));
			hpVal = character->getLife();
		}

		uiComponent[0].draw();

		glMatrixMode(GL_PROJECTION);
		perspective = GLUtil::perspective(90.f, 1, 0.01f, 100.0f);
		glLoadMatrixf(perspective.get());

		glMatrixMode(GL_MODELVIEW);
		modelview = GLUtil::LookAt(character->getPosition(), character->getLook(), Vector3(0, 1, 0));

		glLoadIdentity();
		glMultMatrixf(modelview.get());

		for (size_t i = 0; i < objs.size(); i++)
			objs[i].draw();

	}

	GLuint getTextureId(string name)
	{

	}

	void getDamaged()
	{

	}

	void Character_Objects_Collision_Detection() // 캐릭터와 오브젝트가 충돌했는 지 update 마다 체크한다.
	{
		for (size_t i = 0; i < objs.size(); i++) // 모든 오브젝트를 검사
		{
			// 뚫을 수 없는 오브젝트의 경우
			if (objs[i].get_passable() == false)
			{
				// 캐릭터가 키를 입력받아 이동하는 4가지 경우에 오브젝트와 충돌하는 지 점검, 충돌할 경우에는 캐릭터의 Collision 멤버 변수가 "true"가 된다.
				if (character->collision_check(objs[i].get_box(), character->getCdelta(0) / character->getCdelta(0).length())
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(0) / character->getCdelta(0).length()))
				{
					character->setFrontCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(1) / character->getCdelta(1).length())
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(1) / character->getCdelta(1).length()))
				{
					character->setBackCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(2) / character->getCdelta(2).length())
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(2) / character->getCdelta(2).length()))
				{
					character->setLeftCollision(true);
				}
				if (character->collision_check(objs[i].get_box(), character->getCdelta(3) / character->getCdelta(3).length())
					|| objs[i].get_box().collision_detection_3D(character->get_box(), -character->getCdelta(3) / character->getCdelta(3).length()))
				{
					character->setRightCollision(true);
				}
			}

			// 캐릭터가 현재 오브젝트와 충돌한 경우
			if (GS::character->collision_check(objs[i].get_box(), -objs[i].get_speed())
				|| objs[i].get_box().collision_detection_3D(GS::character->get_box(), objs[i].get_speed())) // 충돌 했을때 하면 true, 아니면 false 이걸로 뭘 할진 생각해 보자.
			{
				// 오브젝트의 get_type가 1일 경우는 2초마다 Life를 1씩 깎도록 한다.
				if (objs[i].get_type() == 1 && (currentTime - crashTime) > 2)
				{
					crashTime = currentTime;
					character->setLife(character->getLife() - 1);
					cout << "현재 라이프 = " << (int)character->getLife() << endl;
				}
				if (GS::character->checkInteract() && objs[i].get_type() == 2) {  // 인터렉션과 콜리젼을 둘다 체크.
//					objs.erase(objs.begin() + i); // erase로 제거하면 추적하던 오브젝트가 방향을 다시 잡는것을 포기함 이 현상을 고쳐야 할 것
					objs[i] = Object();
					character->setLife(character->getLife() + 1);
				}
			}
		}
	}

	void Object_Object_Collision_Detection()
	{
		for (size_t i = 1; i < objs.size(); i++)
			for (size_t j = 1; j < objs.size(); j++)
			{
				if (objs[i].getTracking() == true && i != j) // i가 무언가를 추적할 때
					if (objs[i].get_passable() == false && objs[j].get_passable() == false) // i와 j가 모두 통과 불가능한 물체일 때
						if (objs[i].get_box().collision_detection_3D(objs[j].get_box(), objs[i].get_speed()) || objs[j].get_box().collision_detection_3D(objs[i].get_box(), -objs[i].get_speed()))
						{
							//							cout << i << " 오브젝트의 speed = " << objs[i].get_speed();
							objs[i].set_speed(Vector3(cos(M_PI / 18)*objs[i].get_speed().x - sin(M_PI / 18)*objs[i].get_speed().z, objs[i].get_speed().y, sin(M_PI / 18)*objs[i].get_speed().x + cos(M_PI / 18)*objs[i].get_speed().z));
							//							cout << " -> " << objs[i].get_speed() << endl;
							objs[i].setCollision(true);
							//						cout << i << " 오브젝트와 " << j << "오브젝트 충돌 체크" << endl;
						}
						else if (objs[i].get_box().collision_detection_3D(objs[j].get_box(), 0.1f*objs[i].getTracking()*(objs[i].get_trackposition() - objs[i].get_box().get_cog()).normalize())
							|| objs[j].get_box().collision_detection_3D(objs[i].get_box(), -0.1f*objs[i].getTracking()*(objs[i].get_trackposition() - objs[i].get_box().get_cog())))
						{
							objs[i].setCollision(false);
						}
						else
						{
							objs[i].trackpos(character->getPosition());
							objs[i].setCollision(false);
						}
			}
	}

	void Character_Room_Collision_Detection() // 캐릭터가 방 밖으로 나가려 하는 경우
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

	void Object_Tracking_Character(int index) // index 번째 오브젝트가 캐릭터를 추적하도록 하는 함수
	{
		// 오브젝트는 캐릭터의 위치를 체크한다. 그리고 그 위치로 추적한다. 도착한 후, 오브젝트는 캐릭터의 위치를 다시 체크하고 추적한다.
		// 오브젝트가 캐릭터와 충돌한 경우, 더 이상 움직이지 않는다.
		if (character->collision_check(objs[index].get_box(), -objs[index].get_speed())
			|| objs[index].get_box().collision_detection_3D(character->get_box(), objs[index].get_speed()))
		{
			objs[index].setTracking(false);
		}
		else
		{
			objs[index].setTracking(true);
		}
	}

protected:

	Character* character;
	std::vector<Object> uiComponent;
	std::vector<Object> objs;
	Box roomBox;
	Object hpBar;
	Matrix4 modelview, perspective;

private:
	time_t currentTime;
	time_t crashTime = 0;
	int hpVal = 1;
};