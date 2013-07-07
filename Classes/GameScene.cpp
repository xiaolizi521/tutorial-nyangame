//
//  GameScene.cpp
//  nyangame
//
//  Created by tomi on 7/3/13.
//
//

#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "CCPlaySE.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

CCScene* GameScene::scene() {
    CCScene* scene = CCScene::create();
    GameScene* layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    // get tap events
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    
    // 変数初期化
    initForVariables();
    
    // 背景表示
    showBackground();
    
    // コマ表示
    showBlock();
    
    // preload background mp3 file
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(MP3_REMOVE_BLOCK);

    return true;
}

// 背景表示
void GameScene::showBackground() {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // 背景を生成
    m_background = CCSprite::create(PNG_BACKGROUND);
    m_background->setPosition(ccp(winSize.width / 2.0, winSize.height / 2.0));
    addChild(m_background, kZOrderBackground, kTagBackground);
}

// 変数初期化
void GameScene::initForVariables() {
    // 乱数初期化
    srand((unsigned)time(NULL));
    
    // コマの１辺の長さを取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;
    
    blockTypes.push_back(kBlockRed);
    blockTypes.push_back(kBlockBlue);
    blockTypes.push_back(kBlockYellow);
    blockTypes.push_back(kBlockGreen);
    blockTypes.push_back(kBlockGray);
}

// 位置取得
CCPoint GameScene::getPosition(int posIndexX, int posIndexY ) {
    float offsetX = m_background->getContentSize().width * 0.168;
    float offsetY = m_background->getContentSize().height * 0.029;
    
    return CCPoint((posIndexX + 0.5) * m_blockSize + offsetX,
                   (posIndexY + 0.5) * m_blockSize + offsetY);
}

int GameScene::getTag(int posIndexX, int posIndexY) {
    return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

void GameScene::showBlock() {
    for(int x = 0; x < MAX_BLOCK_X; x++) {
        for(int y = 0; y < MAX_BLOCK_Y; y++) {
            kBlock blockType = (kBlock)(rand() % kBlockcount);
            
            int tag = getTag(x, y);
            m_blockTags[blockType].push_back(tag);
            
            BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
            pBlock->setPosition(getPosition(x, y));
            m_background->addChild(pBlock, kZOrderBlock, tag);
        }
    }
}

// タップ開始イベント
bool GameScene::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    return !m_animating;
}

void GameScene::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);
    
    if (tag != 0) {
        // search nearest blocks
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);
        
        if (sameColorBlockTags.size() > 1) {
            m_animating = true;
            
            removeBlock(sameColorBlockTags, blockType);
            
            movingBlockAnimation1(sameColorBlockTags);
        }
    }
}

void GameScene::getTouchBlockTag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType) {
    for(int x=0;x<MAX_BLOCK_X;x++) {
        for(int y=0;y<MAX_BLOCK_Y;y++) {
            int currentTag = getTag(x, y);
            CCNode* node = m_background->getChildByTag(currentTag);
            if (node && node->boundingBox().containsPoint(touchPoint)) {
                tag = currentTag;
                blockType = ((BlockSprite*)node)->getBlockType();
                
                return;
            }
        }
    }
}

bool GameScene::hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag) {
    list<int>::iterator it;
    for(it=blockTagList.begin(); it != blockTagList.end(); it++) {
        if (*it == searchBlockTag) {
            return true;
        }
    }
    return false;
}

// タップされたコマと同色で且つ接しているコマの配列を返す
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType) {
    
    // initialize blocks that have same color
    list<int> sameColorBlockTags;
    sameColorBlockTags.push_back(baseTag);
    
    list<int>::iterator it = sameColorBlockTags.begin();
    while (it != sameColorBlockTags.end()) {
        int tags[] = {
            *it + 100, // right block tag
            *it - 100, // left block tag
            *it + 1, // up block tag
            *it - 1, // down block tag
        };
        
        for(int i=0;i<sizeof(tags) / sizeof(tags[0]);i++) {
            // すでにリストにあるか検索
            if (!hasSameColorBlock(sameColorBlockTags, tags[i])) {
                // コマ配列にあるか検索
                if (hasSameColorBlock(m_blockTags[blockType], tags[i])) {
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        }
        
        it++;
    }
    
    return sameColorBlockTags;
}

void GameScene::removeBlock(std::list<int> blockTags, kBlock blockType) {
    bool first = true;
    
    
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        m_blockTags[blockType].remove(*it);
        
        CCNode* block = m_background->getChildByTag(*it);
        if (block) {
            CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0);
            
            CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));
            
            CCFiniteTimeAction* sequence = CCSequence::create(scale, func, NULL);
            
            CCFiniteTimeAction* action;
            if (first) {
                CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);
                
                action = CCSpawn::create(sequence, playSe, NULL);
                
                first = false;
            } else {
                action = sequence;
            }
            
            block->runAction(action);
            
        }
        it++;
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}

void GameScene::removingBlock(cocos2d::CCNode *block) {
    block->removeFromParentAndCleanup(true);
}

GameScene::PositionIndex GameScene::getPositionIndex(int tag) {
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;
    
    return PositionIndex(pos1_x, pos1_y);
}

void GameScene::setNewPosition1(int tag, GameScene::PositionIndex posIndex) {
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosY = blockSprite->getNextPosY();
    if (nextPosY == -1) {
        nextPosY = posIndex.y;
    }
    
    blockSprite->setNextPos(posIndex.x, --nextPosY);
}

void GameScene::searchNewPosition1(std::list<int> blocks) {
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end()) {
        PositionIndex posIndex1 = getPositionIndex(*it1);
        
        vector<kBlock>::iterator it2 = blockTypes.begin();
        while (it2 != blockTypes.end()) {
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            while (it3 != m_blockTags[*it2].end()) {
                PositionIndex posIndex2 = getPositionIndex(*it3);
                
                if (posIndex1.x == posIndex2.x &&
                    posIndex1.y < posIndex2.y) {
                    setNewPosition1(*it3, posIndex2);
                }
                
                it3++;
            }
            
            it2++;
        }
        
        it1++;
    }
}

void GameScene::moveBlock() {
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while(it2 != m_blockTags[*it1].end()) {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();
            
            if (nextPosX != -1 || nextPosY != -1) {
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);
                
                *it2 = newTag;
                
                CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                blockSprite->runAction(move);
            }
            
            it2++;
        }
        
        it1++;
    }
}

void GameScene::movingBlockAnimation1(std::list<int> blocks) {
    searchNewPosition1(blocks);
    
    moveBlock();
    
    scheduleOnce(schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME);
}

// コマの移動完了
void GameScene::movedBlocks() {
    m_animating = false;
}

void GameScene::setNewPosition2(int tag, GameScene::PositionIndex posIndex) {
    BlockSprite *blockSprite = (BlockSprite *)m_background->getChildByTag(tag);
    int nextPosX = blockSprite->getNextPosX();
    if (nextPosX == -1) {
        nextPosX = posIndex.x;
    }
    
    blockSprite->setNextPos(--nextPosX, posIndex.y);
}

// 存在する列を取得する
map<int, bool> GameScene::getExistsBlockColumn() {
    map<int, bool> xBlock;
    for(int i=0;i<MAX_BLOCK_X;i++) {
        xBlock[i] = false;
    }
    
    // コマ種類のループ
    vector<kBlock>::iterator it1  = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // 存在するコマのx位置を記録
            xBlock[getPositionIndex(*it2).x] = true;
            
            it2++;
        }
        
        it1++;
    }
    
    return xBlock;
}

// 消えたコマ列を埋めるように新しい位置をセット
void GameScene::searchNewPosition2() {
    map<int, bool> xBlock = getExistsBlockColumn();
    
    bool first = true;
    for(int i = MAX_BLOCK_X - 1; i>=0; i--) {
        if (xBlock[i]) {
            first = false;
            continue;
        }
        else {
            if (first) {
                continue;
            }
            else {
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end()) {
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end()) {
                        PositionIndex posIndex = getPositionIndex(*it2);
                        
                        if (i < posIndex.x) {
                            setNewPosition2(*it2, posIndex);
                        }
                        
                        it2++;
                    }
                    
                    it1++;
                }
            }
        }
    }
}

void GameScene::movingBlocksAnimation2() {
    searchNewPosition2();
    
    moveBlock();
    
    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);
}