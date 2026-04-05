#include "Entity.h"

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mEntityType {NONE} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType} { }

Entity::~Entity() { UnloadTexture(mTexture); };

void Entity::checkCollisionY(Entity **collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity *collidableEntity = collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - 
                              (collidableEntity->mColliderDimensions.y / 2.0f));
            
            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (mVelocity.y > 0) 
            {
                if (collidableEntity->getEntityType() == NPC) {
                    mIsCollidingEnemyBottom = true;
                    continue;
                }

                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
                mIsCollidingBottom = true;
            } else if (mVelocity.y < 0) 
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
                mIsCollidingTop = true;

                if (collidableEntity->mEntityType == BLOCK)
                    collidableEntity->deactivate();
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
            // When standing on a platform, we're always slightly overlapping
            // it vertically due to gravity, which causes false horizontal
            // collision detections. So the solution I found is only resolve X
            // collisions if there's significant Y overlap, preventing the 
            // platform we're standing on from acting like a wall.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            // Skip if barely touching vertically (standing on platform)
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            if (collidableEntity->getEntityType() == NPC) {
                mIsCollidingEnemySide = true;
                continue;
            }

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;

                // Collision!
                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
 
                // Collision!
                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe    = { mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe      = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe     = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) && mVelocity.y < 0.0f)
    {
        mPosition.y += yOverlap;   // push down
        mVelocity.y  = 0.0f;
        mIsCollidingTopMap = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) && mVelocity.y > 0.0f)
    {
        mPosition.y -= yOverlap;   // push up
        mVelocity.y  = 0.0f;
        mIsCollidingBottomMap = true;
    } 
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y };

    Vector2 rightCentreProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x -= xOverlap * 1.01f;   // push left
        mVelocity.x  = 0.0f;
        mIsCollidingRightMap = true;
    }

    // COLLISION ON LEFT (moving left)
    if (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x += xOverlap * 1.01;   // push right
        mVelocity.x  = 0.0f;
        mIsCollidingLeftMap = true;
    }
}

bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::AIWander() {
    // Wanderer paces between two boundary tiles
    if (mIsBoundedWanderer) {
        if      (mPosition.x <= mWanderMinX) mDirection = RIGHT;
        else if (mPosition.x >= mWanderMaxX) mDirection = LEFT;
    }

    if (mDirection == LEFT) moveLeft();
    else                    moveRight();
}

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 600.0f) 
            mAIState = FOLLOWING;
        break;

    case FOLLOWING:
        // Calculate normalized direction vector
        Vector2 direction = Vector2Subtract(target->getPosition(), mPosition);

        if (GetLength(direction) > 0.0f) {
            direction.x /= GetLength(direction);
            direction.y /= GetLength(direction);
        }

        // Calculate velocities
        mMovement.x = direction.x;
        mVelocity.y = direction.y * mSpeed;

        mDirection = (mVelocity.x >= 0.0f) ? RIGHT : LEFT;
        break;
    }
}

void Entity::AIFly(Map *map) {
    // Randomize flying direction
    if (!mFlying) {
        float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
        mFlyDirection = {cosf(angle), sinf(angle)};
        mFlying = true;
    }

    // Handle collisions with map boundaries
    if (map != nullptr) {
        float halfWidth  = mColliderDimensions.x / 2.0f;
        float halfHeight = mColliderDimensions.y / 2.0f;
        
        // Horizontal corrections for out-of-bounds clipping
        if (mPosition.x - halfWidth <= map->getLeftBoundary()) {
            mFlyDirection.x = fabs(mFlyDirection.x);
            mPosition.x = map->getLeftBoundary() + halfWidth;
            mIsCollidingLeftMap = true;
        }
        else if (mPosition.x + halfWidth >= map->getRightBoundary()) {
            mFlyDirection.x = -fabs(mFlyDirection.x);
            mPosition.x = map->getRightBoundary() - halfWidth;
            mIsCollidingRightMap = true;
        }

        // Vertical corrections for out-of-bounds clipping
        if (mPosition.y - halfHeight <= map->getTopBoundary()) {
            mFlyDirection.y = fabs(mFlyDirection.y);
            mPosition.y = map->getTopBoundary() + halfHeight;
            mIsCollidingTopMap = true;
        }
        else if (mPosition.y + halfHeight >= map->getBottomBoundary()) {
            mFlyDirection.y = -fabs(mFlyDirection.y);
            mPosition.y = map->getBottomBoundary() - halfHeight;
            mIsCollidingBottomMap = true;
        }
    }

    // Handle collisions with platforms (randomized direction)
    if (mIsCollidingTopMap || mIsCollidingBottomMap) mFlyDirection.y = -mFlyDirection.y;
    if (mIsCollidingLeftMap || mIsCollidingRightMap) mFlyDirection.x = -mFlyDirection.x;

    if (mIsCollidingTopMap || mIsCollidingBottomMap ||
        mIsCollidingLeftMap || mIsCollidingRightMap)
    {
        float angle = atan2f(mFlyDirection.y, mFlyDirection.x) +
            (((float) rand() / RAND_MAX) * 2.0f - 1.0f) * (PI / 4.0f);

        mFlyDirection = {cosf(angle), sinf(angle)};

        // Small correction for collision handling
        if (mIsCollidingLeftMap || mIsCollidingRightMap) mPosition.y += mFlyDirection.y * 2.0f;
        if (mIsCollidingTopMap || mIsCollidingBottomMap) mPosition.x += mFlyDirection.x * 2.0f;
    }

    // Bounce!
    mMovement.x = mFlyDirection.x;
    mVelocity.y = mFlyDirection.y * mSpeed;

    mDirection = (mFlyDirection.x >= 0.0f) ? RIGHT : LEFT;
}

void Entity::AIActivate(Entity *target, Map *map)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;
    
    case FLYER:
        AIFly(map);
        break;
    
    default:
        break;
    }
}

void Entity::update(float deltaTime, Entity *player, Map *map, 
    Entity **collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;
    
    if (mEntityType == NPC) AIActivate(player, map);

    resetColliderFlags();

    mVelocity.x = mMovement.x * mSpeed;

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;
        
        // STEP 2: The player now acquires an upward velocity
        mVelocity.y -= mJumpingPower;
    }

    if (mIsInvincible) {
        mInvincibleTimer += deltaTime;
        mHitTimer        += deltaTime;

        if (mInvincibleTimer >= mInvincibleDuration) {
            mIsInvincible    = false;
            mInvincibleTimer = 0.0f;
            mHitTimer        = 0.0f;
        }
    }

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(map);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(map);

    if (mTextureType == ATLAS && GetLength(mMovement) != 0 &&
       (mIsCollidingBottomMap || mEntityType == NPC))
        animate(deltaTime);
}

void Entity::render()
{
    if(mEntityStatus == INACTIVE) return;

    Color flicker = WHITE;
    if (mIsInvincible) {
        if ((int)(mHitTimer / mHitInterval) % 2 == 0) flicker = WHITE;
        else                                          flicker = (Color){ 255, 255, 255, 80 };
    }

    Rectangle textureArea;

    switch (mTextureType)
    {
        case SINGLE:
            // Whole texture (UV coordinates)
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner (of texture)
                static_cast<float>(mTexture.width),
                static_cast<float>(mTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle(
                &mTexture,
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );

            if (mDirection == LEFT) textureArea.width = -textureArea.width;
        
        default: break;
    }

    // Destination rectangle – centred on gPosition
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
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, flicker
    );

    // displayCollider();
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