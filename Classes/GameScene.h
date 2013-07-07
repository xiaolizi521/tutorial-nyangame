//
//  GameScene.h
//  nyangame
//
//  Created by tomi on 7/3/13.
//
//

#ifndef __nyangame__GameScene__
#define __nyangame__GameScene__

#include "cocos2d.h"
#include "Config.h"

#define MAX_BLOCK_X 8
#define MAX_BLOCK_Y 8
#define REMOVING_TIME 0.1f
#define MOVING_TIME 0.2f

#define PNG_BACKGROUND "background.png"
#define MP3_REMOVE_BLOCK "removeBlock.mp3"

#define FONT_RED "redFont.fnt"
#define FONT_BLUE "blueFont.fnt"
#define FONT_YELLOW "yellowFont.fnt"
#define FONT_GREEN "greenFont.fnt"
#define FONT_GRAY "grayFont.fnt"

class GameScene : public cocos2d::CCLayer {
protected:
    enum kTag {
        kTagBackground = 1, // 画像のタグ
        kTagRedLabel,
        kTagBlueLabel,
        kTagYellowLabel,
        kTagGreenLabel,
        kTagGrayLabel,

        kTagBaseBlock = 10000,
    };
    
    enum kZOrder {
        kZOrderBackground, // z-order
        kZOrderLabel, // ラベル同士が表示上重なることがないため，1種類のみ用意
        kZOrderBlock,
    };
    
    struct PositionIndex {
        PositionIndex(int x1, int y1) {
            x = x1;
            y = y1;
        }
        
        int x;
        int y;
    };
    
    cocos2d::CCSprite* m_background;
    void showBackground();
    
    float m_blockSize;
    std::map<kBlock, std::list<int> > m_blockTags;
    void initForVariables();
    void showBlock();
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);
    int getTag(int posIndexX, int posIndexY);
    
    void getTouchBlockTag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType);
    std::list<int> getSameColorBlockTags(int baseTag, kBlock blockType);
    void removeBlock(std::list<int> blockTags, kBlock blockType);
    bool hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag);
    
    void removingBlock(cocos2d::CCNode* block);
    
    std::vector<kBlock> blockTypes;
    PositionIndex getPositionIndex(int tag);
    void setNewPosition1(int tag, PositionIndex posIndex);
    void searchNewPosition1(std::list<int> blocks);
    void moveBlock();
    void movingBlockAnimation1(std::list<int> blocks);
    
    bool m_animating;
    void movedBlocks();
    std::map<int, bool> getExistsBlockColumn();
    void searchNewPosition2();
    void setNewPosition2(int tag, PositionIndex posIndex);
    void movingBlocksAnimation2();
    
    void showLabel();
    
public:
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(GameScene);
    
    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent *pEvent);
};

#endif /* defined(__nyangame__GameScene__) */
