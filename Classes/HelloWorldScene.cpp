#include "HelloWorldScene.h"
#include "network/HttpClient.h"
#include "SimpleAudioEngine.h"
#include "Option.h"
#include "PhysiSprite.h"
#include "picojson.h"
#include <stdio.h>

USING_NS_CC;
USING_NS_CC_EXT;

HelloWorld::HelloWorld()
{
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    HudLayer *hud = new HudLayer();
    hud->init();
    scene->addChild(hud);
    layer->_hud = hud;
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

	this->initPhysics();
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("pickup.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("hit.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("move.caf");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("TileMap.caf");
    
	ESpriteBatchNode = CCSpriteBatchNode::create("Player.png");

	CreateBackground();	//�w�i�������\�b�h��

    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");
    
    if(objectGroup == NULL){
        CCLog("tile map has no objects object layer");
        return false;
    }
	//CreatePlayer(ccp(x,y));
	//CreateBomb(spawnpoint);

	//�X�R�A�p���x���̕\��
	ScoreLabel =  CCLabelTTF::create("Score: 0","arial",30);
	ScoreLabel->setPosition(ccp(ScreenSize.width * 0.2f, ScreenSize.height * 0.97f ));
	this->addChild(ScoreLabel);
	score = 0;

	//�o�ߎ��ԗp���x��
	time = 0;
	TimeLabel = CCLabelTTF::create("Time: 0", "arial", 30);
	TimeLabel->setPosition(ccp(ScreenSize.width * 0.8f, ScreenSize.height * 0.97f));
	this->addChild(TimeLabel);

	this->setTouchEnabled(true);
	this->schedule(schedule_selector(HelloWorld::CreateEnemy), 1.5f); 

	//���ԕ\����1�b���ƂɍX�V
	this->schedule(schedule_selector(HelloWorld::ElapsedTime),1.0f);

	scheduleUpdate();	//update���\�b�h�����s
	
	//�N���b�N�ʒu��\�������邩�ǂ����̐���(true = �\��/false = ��\��)
	TouchLabelbool = true;
	if(TouchLabelbool){
		TouchPosLabel();
	}
	
	severMessageLabel();
    
    return true;
}

void HelloWorld::update(float dt)
{
	_world->Step(dt,10,10);

}

//�o�ߎ��ԍX�V���\�b�h
void HelloWorld::ElapsedTime(float dt)
{
	time++;
	CCString* message = CCString::createWithFormat("Time: %i",time);
	TimeLabel->setString(message->getCString());
}

//�^�b�`�ʒu�\�����\�b�h
void HelloWorld::TouchPosLabel(){
	
	//�^�b�`�ʒu�iX�j�\��
	touchPosX = CCLabelTTF::create("TouchPosX: ", "arial", 20);
	touchPosX->setPosition(ccp(ScreenSize.width * 0.2f, ScreenSize.height * 0.8f));
	this->addChild(touchPosX);

	//�^�b�`�ʒu�iY�j�\��
	touchPosY = CCLabelTTF::create("TouchPosY: ", "arial", 20);
	touchPosY->setPosition(ccp(ScreenSize.width * 0.2f, ScreenSize.height * 0.7f));
	this->addChild(touchPosY);

	TouchLabelbool = true;
}

//�^�b�`�ʒu���l�X�V���\�b�h
void HelloWorld::TouchPosLabelRenewal(CCPoint point)
{
	CCString* touchX = CCString::createWithFormat("TouchX: %f",point.x / PTM_RATIO);
	touchPosX->setString(touchX->getCString());
	
	CCString* touchY = CCString::createWithFormat("TouchX: %f",point.y / PTM_RATIO);
	touchPosY->setString(touchY->getCString());
}

//JSON�v�����\�b�h�̗\��
//void HelloWorld::onHttpRequestCompleted(CCHttpClient sender, CCHttpResponse response)

//JSON�f�[�^���T�[�o�[���ɑ��M����\��̃��\�b�h
void HelloWorld::HttpPostDate(string Pname, int score)
{
	CCDictionary* postdate = CCDictionary::create();
	postdate->setObject(CCString::create(Pname),"name");		//PlayerName���Z�b�g
	postdate->setObject(CCInteger::create(score),"score");		//Score���Z�b�g
}

//��M����JSON�f�[�^�𐮗����郁�\�b�h
void HelloWorld::SortDate()
{

}

//�T�[�o�[�����M�������b�Z�[�W��\��
void HelloWorld::severMessageLabel()
{
	request = new CCHttpRequest();
	request->setUrl("http://localhost:8080/helo");	//���N�G�X�g���s��URL
	request->setRequestType(CCHttpRequest::kHttpGet);

	CCLabelTTF* severMessage = CCLabelTTF::create("SeverMessage: ", "arial", 20);
	severMessage->setPosition(ccp(ScreenSize.width * 0.2f, ScreenSize.height * 0.6f));
	this->addChild(severMessage);
}

void HelloWorld::AddScore(int point)
{
	score += point;
	CCString* message = CCString::createWithFormat("Score: %i",score);
	ScoreLabel->setString(message->getCString());
}

//�w�i�ݒ胁�\�b�h
void HelloWorld::CreateBackground()
{
    _tileMap = new CCTMXTiledMap();
    _tileMap->initWithTMXFile("sample.tmx");
    //_background = _tileMap->layerNamed("Background");
    //_foreground = _tileMap->layerNamed("Foreground");
    
    //_meta = _tileMap->layerNamed("Meta");
    //_meta->setVisible(false);
    
    //this->addChild(_tileMap);

}

//Bomb�쐬
void HelloWorld::CreateBomb(CCPoint point)
{
	PhysiSprite* BombSprite = new PhysiSprite();	//PhysiSprite��Bomb�p�̃X�v���C�g���쐬
	BombSprite->autorelease();
	BombSprite->setTag(TAG_BOMB);	//�^�O��TAG_BOMB�ɐݒ�
	BombSprite->initWithFile("Bomb.png");	//BombSprite�Ɏg�p����e�N�X�`����ݒ�
	BombSprite->setPosition(point);	//�쐬�ʒu��ݒ�
	this->addChild(BombSprite);	//BombSprite�̕\��

	//Bomb�̕��������̐ݒ�
	b2BodyDef BombBodyDef;
	BombBodyDef.type = b2_dynamicBody;	//���I�Ɏg�p����̂Ń_�C�i�~�b�N�{�f�B�ɐݒ�
	BombBodyDef.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);	//

	BombBodyDef.userData = BombSprite;

	b2Body* Bomb_body = _world->CreateBody(&BombBodyDef);
	Bomb_body->SetLinearVelocity(b2Vec2(0.0, -10.0));
	b2CircleShape BombShape;
	BombShape.m_radius = BombSprite->getContentSize().width * 0.5 / PTM_RATIO;

	b2FixtureDef BombFixDef;
	BombFixDef.shape = &BombShape;
	BombFixDef.density = 1.0f;
	BombFixDef.friction = 0.9;

	Bomb_body->CreateFixture(&BombFixDef);
	BombSprite->setPhysiBody(Bomb_body);

	//this->setViewPointCenter(BombSprite->getPosition());

}

//Enemy�쐬
void HelloWorld::CreateEnemy(float dt)
{
	PhysiSprite* EnemySprite = new PhysiSprite();
	EnemySprite->autorelease();
	EnemySprite->setTag(TAG_ENEMY);
	EnemySprite->initWithFile("Player.png");
	EnemySprite->setPosition(ccp(0,0));
	this->addChild(EnemySprite);

	b2BodyDef EnemyBodyDef;
	EnemyBodyDef.type = b2_dynamicBody;
	float posX = (float)(rand()%(95-10+1)+10) / 100 ;
	EnemyBodyDef.position.Set(ScreenSize.width * posX / PTM_RATIO, ScreenSize.height * 0.1f / PTM_RATIO);
	EnemyBodyDef.userData = EnemySprite;

	enemyBody = _world->CreateBody(&EnemyBodyDef);
	
	enemyBody->SetLinearVelocity(b2Vec2(0.0, 10.0));

	b2CircleShape EnemyShape;
	EnemyShape.m_radius = EnemySprite->getContentSize().width * 0.5 / PTM_RATIO;

	b2FixtureDef BombFixDef;
	BombFixDef.shape = &EnemyShape;
	BombFixDef.density = 1.0f;
	BombFixDef.friction = 0.9;

	enemyBody->CreateFixture(&BombFixDef);
	EnemySprite->setPhysiBody(enemyBody);

	//this->setViewPointCenter(BombSprite->getPosition());

}

//�v���C���[�쐬
void HelloWorld::CreatePlayer(CCPoint point)
{

	CCSprite* playerSprite = CCSprite::create("Player.png");	//�v���C���[�ɓ��Ă�摜�̐ݒ�
	playerSprite->setPosition(point);	//�����ʒu�̐ݒ�
	this->addChild(playerSprite);	//playerSprite�̐���

	//���������̏�����
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;	//���I�ɐݒ�
	bodyDef.userData = playerSprite;	//playerSprite��userDate�Ɋi�[
	bodyDef.position.Set(point.x/PTM_RATIO, point.y/PTM_RATIO);	//bodyDef�̐����ʒu��ݒ�

	P_body = _world->CreateBody(&bodyDef);	//bodyDef�̐ݒ肪������P_body�Ƃ���b2body�𐶐�

	//�`��̏�����
	b2CircleShape circleShape;	//�`����T�[�N���ɐݒ�
	circleShape.m_radius = 50/PTM_RATIO;	//�a��50�s�N�Z��

	b2FixtureDef fixDef;
	fixDef.shape = &circleShape;	//�`������
	fixDef.density = 1.0f;	//���x
	fixDef.friction = 0.5f;	//���C��
	fixDef.restitution = 1.0f;	//�����W��
	P_body->CreateFixture(&fixDef);	//P_body�Ɍ`���ݒ�

	this->setViewPointCenter(playerSprite->getPosition());

}

//b2Body�폜���\�b�h
void HelloWorld::RemoveOBJ(b2Body* body)
{
	_world->DestroyBody(body);
}

//����������
void HelloWorld::initPhysics()
{
	//���[���h�̕����ݒ�
	gravity = b2Vec2(0.0f, 0.0f);	//�Qworld�̏d�͂�ݒ�
	_world = new b2World(gravity);	//_world��gravity�����
	
	//�Փ˔����ݒ�
	_contactListener = new ContactListener(_world, this);
	_world->SetContactListener(_contactListener);

	//�Q�[����ʒ[�̐ݒ�
	ScreenSize = CCDirector::sharedDirector()->getWinSize();
	b2BodyDef worldBodyDef;
	worldBodyDef.position.Set(0,0);

	b2Body* worldBody = _world->CreateBody(&worldBodyDef);

	b2EdgeShape worldBox;

	//���
	worldBox.Set(b2Vec2(ScreenSize.width * 0.05f / PTM_RATIO, ScreenSize.height * 0.95f / PTM_RATIO),
		b2Vec2(ScreenSize.width * 0.95f /PTM_RATIO, ScreenSize.height * 0.95f / PTM_RATIO));
	
	worldBody->CreateFixture(&worldBox,0);

	//����
	worldBox.Set(b2Vec2(ScreenSize.width * 0.05f / PTM_RATIO, ScreenSize.height * 0.05f / PTM_RATIO),
		b2Vec2(ScreenSize.width * 0.95f /PTM_RATIO, ScreenSize.height * 0.05f / PTM_RATIO));

	worldBody->CreateFixture(&worldBox,0);

	//����
	worldBox.Set(b2Vec2(ScreenSize.width * 0.05f / PTM_RATIO, ScreenSize.height * 0.05f / PTM_RATIO),
		b2Vec2(ScreenSize.width * 0.05f /PTM_RATIO, ScreenSize.height * 0.95f / PTM_RATIO));

	worldBody->CreateFixture(&worldBox,0);
	
	//�E��
	worldBox.Set(b2Vec2(ScreenSize.width * 0.95f / PTM_RATIO, ScreenSize.height * 0.05f / PTM_RATIO),
		b2Vec2(ScreenSize.width * 0.95f /PTM_RATIO, ScreenSize.height * 0.95f / PTM_RATIO));

	worldBody->CreateFixture(&worldBox,0);
	
 
	//debugDraw�̐ݒ�
	_debugDraw = new GLESDebugDraw( PTM_RATIO );
	_world->SetDebugDraw(_debugDraw);
 
	//DebudDraw���g�p���邽��CCLayer��draw�N���X���I�[�o�[���C�h���Ă���
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    _debugDraw->SetFlags(flags);
}

void HelloWorld::setViewPointCenter(CCPoint position)
{
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

#pragma mark - handle touches

void HelloWorld::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

//�N���b�N�����C�x���g
bool HelloWorld::ccTouchBegan(CCTouch *touch, CCEvent *event)
{

	CCDirector* pDirector = CCDirector::sharedDirector();
	CCPoint touchPoint = pDirector->convertToGL(touch->getLocationInView());		//�^�b�`���ꂽ�ӏ�����ʈʒu�ɕ␳
	
	if(TouchLabelbool){
		TouchPosLabelRenewal(touchPoint);
	}
	//Bomb�𐶐��ł���^�b�`�G���A������
	float MaxX = 14.0f;	//�E���^�b�`�ʒu���E
	float MinX = 1.0f;		//�����^�b�`�ʒu���E
	float MaxY = 29.0f;	//����^�b�`�ʒu���E
	float MinY = 25.0f;	//�����^�b�`�ʒu���E
	if((((touchPoint.x / PTM_RATIO) > 1.0f) && ((touchPoint.x / PTM_RATIO) < 14.0f))&&
		((touchPoint.y / PTM_RATIO) > MinY) && (touchPoint.y / PTM_RATIO) < MaxY)
	{
			CreateBomb(touchPoint);
	}

    return true;
}

void HelloWorld::setPlayerPosition(CCPoint position)
{
    CCPoint tileCoord = this->tileCoordForPosition(position);
    int tileGid = _meta->tileGIDAt(tileCoord);
    if (tileGid) {
        CCDictionary *properties = _tileMap->propertiesForGID(tileGid);
        if (properties) {
            CCString *collision = new CCString();
            *collision = *properties->valueForKey("Collidable");
            if (collision && (collision->compare("True") == 0)) {
                CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("hit.caf");
                return;
            }
            CCString *collectible = new CCString();
            *collectible = *properties->valueForKey("Collectable");
            if (collectible && (collectible->compare("True") == 0)) {
                _meta->removeTileAt(tileCoord);
                _foreground->removeTileAt(tileCoord);
                _numCollected++;
                _hud->numCollectedChanged(_numCollected);
                CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("pickup.caf");
            }
        }
    }
    _player->setPosition(position);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("move.caf");
}

void HelloWorld::ccTouchEnded(CCTouch *touch, CCEvent *event)
{
}

CCPoint HelloWorld::tileCoordForPosition(CCPoint position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return ccp(x, y);
}

void HelloWorld::draw()
{
	CCLayer::draw();
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
	kmGLPushMatrix();
	_world->DrawDebugData();
	kmGLPopMatrix();
}
 
//new�Ń��������m�ۂ���_world��_debugDraw��autorelease����Ȃ����߃f�X�g���N�^�ɂĉ��
HelloWorld::~HelloWorld()
{
	delete _debugDraw;
    _debugDraw = NULL;
 
    delete _world;
    _world = NULL;
}
