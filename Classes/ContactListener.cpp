#include "ContactListener.h"
#include "Option.h"

//ContactListener�̃R���X�g���N�^
ContactListener::ContactListener(b2World* world, HelloWorld* HW)
{
	thisWorld = world;		//world��ݒ�
	HeWorld = HW;			//���C���[��ݒ�H
}

void ContactListener::BeginContact(b2Contact* contact)
{
	
	b2Fixture* FixtuerA = contact->GetFixtureA();
	b2Fixture* FixtuerB = contact->GetFixtureB();

	
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);	//�Qworld�̏d�͂�ݒ�
	b2World* testworld = new b2World(gravity); 

	//Fixture��Null�`�F�b�N
	CCAssert(FixtuerA != NULL,"FixtureA is NULL");
	CCAssert(FixtuerB != NULL,"FixtureB is NULL");
	
	//�ڐG�����I�u�W�F�N�g��body���擾
	b2Body* BodyA = FixtuerA->GetBody();
	b2Body* BodyB = FixtuerB->GetBody();
	
	//Body��Null�`�F�b�N
	CCAssert(BodyA != NULL,"BodyA��NULL");
	CCAssert(BodyB != NULL,"BodyB��NULL");

	//Body�̃^�C�v���擾
	int BodyTypeA = BodyA->GetType();
	int BodyTypeB = BodyB->GetType();
	
	//�ڐG�������̂������Ƃ����I�Ȃ��̂̏ꍇ�Ɏ��s
	if((BodyTypeA == b2_dynamicBody)&&(BodyTypeB == b2_dynamicBody))
	{
		//�ڐG�����I�u�W�F�N�g��userdata���擾
		CCSprite* SpriteA = (CCSprite*)BodyA->GetUserData();
		CCSprite* SpriteB = (CCSprite*)BodyB->GetUserData();
	
		//A��B�̃I�u�W�F�N�g�̃^�O���擾
		int TagA = SpriteA->getTag();
		int TagB = SpriteB->getTag();

		//�^�O��Enemy�̏ꍇ�폜
		if(TagA == TAG_ENEMY)
		{
			InvisibleSprite(SpriteA);	//SpritA���\��
			HeWorld->AddScore(10);		//score��10���Z
			DeleteBody(BodyA, thisWorld);

		}
		if(TagB == TAG_ENEMY)
		{
			InvisibleSprite(SpriteB);	//SpritA���\��
			HeWorld->AddScore(10);		//score��10���Z
		}

		//�^�O��Bomb�̏ꍇ�폜
		if(TagA == TAG_BOMB)
		{
			InvisibleSprite(SpriteA);
		}
		if(TagB == TAG_BOMB)
		{
			InvisibleSprite(SpriteB);
		}
	}

	//�Е����ÓI(��ʒ[�Ȃ�)�I�u�W�F�N�g�������ꍇ
	else if(BodyTypeA == b2_dynamicBody)
	{
		//�ڐG�����I�u�W�F�N�g��userdata���擾
		CCSprite* SpriteA = (CCSprite*)BodyA->GetUserData();
	
		//A��B�̃I�u�W�F�N�g�̃^�O���擾
		int TagA = SpriteA->getTag();
		
		//�^�O��Bomb�̏ꍇ�폜
		if((TagA == TAG_BOMB)||(TagA == TAG_ENEMY))
		{
			InvisibleSprite(SpriteA);
		}
	}

	else if(BodyTypeB == b2_dynamicBody)
	{
		//�ڐG�����I�u�W�F�N�g��userdata���擾
		CCSprite* SpriteB = (CCSprite*)BodyB->GetUserData();
	
		//A��B�̃I�u�W�F�N�g�̃^�O���擾
		int TagB = SpriteB->getTag();
		
		//�^�O��Bomb�̏ꍇ�폜
		if((TagB == TAG_BOMB)||(TagB == TAG_ENEMY))
		{
			InvisibleSprite(SpriteB);
		}
	}
}

//Sprite��\�����\�b�h
void ContactListener::InvisibleSprite(CCSprite* sprite)
{
	sprite->setVisible(false);
}

//Body�폜���\�b�h
void ContactListener::DeleteBody(b2Body* body, b2World* world)
{
	world->DestroyBody(body);
}
