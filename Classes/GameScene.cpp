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
    return true;
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
            removeBlock(sameColorBlockTags, blockType);
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
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        m_blockTags[blockType].remove(*it);
        
        CCNode* block = m_background->getChildByTag(*it);
        if (block) {
            block->removeFromParentAndCleanup(true);
        }
        it++;
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}