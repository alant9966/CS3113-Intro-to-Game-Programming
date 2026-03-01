#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

enum KnightStatus {   UP, DOWN, IDLE, ATTACK  };
enum EntityStatus {      ACTIVE, INACTIVE     };
enum EntityType   {     KNIGHT, BALL, NONE    };
enum PlayStatus   { TWO_PLAYER, SINGLE_PLAYER };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;

    Vector2 mScale;
    Vector2 mColliderDimensions;
    
    Texture2D mTexture;
    Texture2D mAttackTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    
    std::map<KnightStatus, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    KnightStatus mKnightStatus;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime   = 0.0f,
          mAttackTimer     = 0.0f;

    int mSpeed;
    float mAngle;
    bool mFlipped;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    bool isColliding(Entity *other) const;
    void checkCollisionY(Entity **collidableEntities, int collisionCheckCount);
    void checkCollisionX(Entity **collidableEntities, int collisionCheckCount);
    void resetColliderFlags() 
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 300;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, 
        std::map<KnightStatus, std::vector<int>> animationAtlas, 
        EntityType entityType);
    ~Entity();

    void update(float deltaTime, Entity **collidableEntities, int collisionCheckCount, Vector2 screenParams);
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void activate()   { mEntityStatus  = ACTIVE;   }
    void deactivate() { mEntityStatus  = INACTIVE; }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE ? true : false; }

    void moveUp()    { mMovement.y = -1; if (mAttackTimer <= 0.0f) mKnightStatus = UP;   }
    void moveDown()  { mMovement.y =  1; if (mAttackTimer <= 0.0f) mKnightStatus = DOWN; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }
    void resetSpeed()    { mSpeed    =  DEFAULT_SPEED; }
    void resetPos(Vector2 pos);

    Vector2      getPosition()              const { return mPosition;              }
    Vector2      getMovement()              const { return mMovement;              }
    Vector2      getVelocity()              const { return mVelocity;              }
    Vector2      getScale()                 const { return mScale;                 }
    Vector2      getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2      getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D    getTexture()               const { return mTexture;               }
    TextureType  getTextureType()           const { return mTextureType;           }
    KnightStatus getKnightStatus()          const { return mKnightStatus;          }
    EntityType   getEntityType()            const { return mEntityType;            }
    int          getFrameSpeed()            const { return mFrameSpeed;            }
    int          getSpeed()                 const { return mSpeed;                 }
    float        getAngle()                 const { return mAngle;                 }
    
    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<KnightStatus, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                       }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                       }
    void setVelocity(Vector2 newVelocity)
        { mVelocity = newVelocity;                       }
    void setScale(Vector2 newScale)
        { mScale = newScale;                             }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath);       }
    void setColliderDimensions(Vector2 newDimensions) 
        { mColliderDimensions = newDimensions;           }
    void setSpriteSheetDimensions(Vector2 newDimensions) 
        { mSpriteSheetDimensions = newDimensions;        }
    void setSpeed(int newSpeed)
        { mSpeed  = newSpeed;                            }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                        }
    void setAngle(float newAngle) 
        { mAngle = newAngle;                             }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                      }
    void setFlipped(bool isFlipped)
        { mFlipped = isFlipped;                          }
    void setKnightStatus(KnightStatus newStatus)
        // Lock out state switches during an attack
        { if (mAttackTimer <= 0.0f || newStatus == ATTACK)
              mKnightStatus = newStatus;                  }
    void setAttackTimer(float newTimer)
        { mAttackTimer = newTimer;                       }
    void setAttackTexture(const char *textureFilepath)
        { mAttackTexture = LoadTexture(textureFilepath); }};

#endif // ENTITY_H