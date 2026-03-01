#include "Entity.h"

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mKnightStatus {IDLE}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mSpeed {DEFAULT_SPEED}, mEntityType {NONE}, mFlipped {false} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mScale {scale}, mMovement {0.0f, 0.0f}, mColliderDimensions {scale},
    mTexture {LoadTexture(textureFilepath)}, mTextureType {SINGLE},
    mKnightStatus {IDLE}, mAnimationAtlas {{}}, mAnimationIndices {}, 
    mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, mAngle {0.0f}, 
    mEntityType {entityType}, mFlipped {false} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<KnightStatus, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, 
        mScale {scale}, mColliderDimensions {scale}, 
        mTexture {LoadTexture(textureFilepath)}, mTextureType {ATLAS}, 
        mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mKnightStatus {IDLE},
        mAnimationIndices {animationAtlas.at(IDLE)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType},
        mFlipped {false} { }

Entity::~Entity() { UnloadTexture(mTexture); };

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity.
 * 
 * @param collidableEntities An array of pointers to `Entity` objects that 
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */
void Entity::checkCollisionY(Entity **collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {            
            if (mVelocity.y > 0) {
                mIsCollidingBottom = true;
            } else if (mVelocity.y < 0) {
                mIsCollidingTop = true;
            }
        }
    }
}

void Entity::checkCollisionX(Entity **collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {            
            if (mVelocity.x > 0) {
                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mIsCollidingLeft = true;
            }

            // Handle paddle collisions (ping-pong!)
            if (mEntityType == BALL && collidableEntity->getEntityType() == KNIGHT) {
                if (mVelocity.x > 0) {
                    // Clipping adjustment (Knight 2)
                    float knightLeft = (collidableEntity->getPosition().x -
                        collidableEntity->getColliderDimensions().x / 2.0f);
                    mPosition.x = knightLeft - (mColliderDimensions.x / 2.0f) - 0.5f; 
                } else if (mVelocity.x < 0) {
                    // Clipping adjustment (Knight 1)
                    float knightRight = (collidableEntity->getPosition().x +
                        collidableEntity->getColliderDimensions().x / 2.0f);
                    mPosition.x = knightRight + (mColliderDimensions.x / 2.0f) + 0.5f;  
                }

                // Calculate reflection angle based on where the collision occurred
                float yDist = mPosition.y - collidableEntity->getPosition().y;

                mMovement.x *= -1.0f;
                mMovement.y = yDist / (collidableEntity->getScale().y / 2.0f);
                normaliseMovement();

                // Trigger attack animation
                collidableEntity->setKnightStatus(ATTACK);
                collidableEntity->setAttackTimer(0.15f);
            }
        }
    }
}

/**
 * Checks if two entities are colliding based on their positions and collider 
 * dimensions.
 * 
 * @param other represents another Entity with which you want to check for 
 * collision. It is a pointer to the Entity class.
 * 
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive()) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

/**
 * Updates the current frame index of an entity's animation based on the 
 * elapsed time and frame speed.
 * 
 * @param deltaTime represents the time elapsed since the last frame update.
 */
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mKnightStatus);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::displayCollider() 
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}

void Entity::resetPos(Vector2 pos) {
    mPosition = pos;
    mVelocity = { 0, 0 };

    float rand_norm = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    mMovement = {cos(rand_norm * PI), sin(rand_norm * PI)};
}

void Entity::update(float deltaTime, Entity **collidableEntities,
    int collisionCheckCount, Vector2 screenParams)
{
    if(mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;

    // Screen collision checks (with margin)
    if (mEntityType == KNIGHT) {
        if (mPosition.y < mScale.y / 2.0f) {
            mPosition.y = mScale.y / 2.0f;
            mVelocity.y = 0.0f;
        }
        if (mPosition.y > screenParams.y - mScale.y / 2.0f) {
            mPosition.y = screenParams.y - mScale.y / 2.0f;
            mVelocity.y = 0.0f;
        }
    } else if (mEntityType == BALL) {
        // Reverse direction and handle clipping
        if (mPosition.y < mScale.y / 2.0f) {
            mPosition.y = mScale.y / 2.0f;
            mMovement.y *= -1.0f;
        } 
        else if (mPosition.y > screenParams.y - mScale.y / 2.0f) {
            mPosition.y = screenParams.y - mScale.y / 2.0f;
            mMovement.y *= -1.0f;
        }
    }

    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionX(collidableEntities, collisionCheckCount);

    // Swap to attack sprite momentarily
    if (mKnightStatus == ATTACK && mAttackTimer > 0.0f) {
        mAttackTimer -= deltaTime;

        if (mAttackTimer <= 0.0f) {
            mKnightStatus = IDLE;
            mAttackTimer = 0.0f;
        }
    }

    if (mTextureType == ATLAS && mKnightStatus != ATTACK) 
        animate(deltaTime);
}

void Entity::render()
{
    if(mEntityStatus == INACTIVE) return;

    Rectangle   textureArea;
    Texture2D   currTexture     = mTexture;
    TextureType currTextureType = mTextureType;
    
    if (mKnightStatus == ATTACK) {
        currTexture = mAttackTexture;
        currTextureType = SINGLE;
    }

    switch (currTextureType)
    {
        case SINGLE:
            // Whole texture (UV coordinates)
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner (of texture)
                static_cast<float>(currTexture.width),
                static_cast<float>(currTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle(
                &currTexture, 
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );
        
        default: break;
    }

    // Flip texture horizontally (for gKnight2)
    if (mFlipped) textureArea.width *= -1.0f;

    // Destination rectangle – centreded on gPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        currTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // displayCollider();
}