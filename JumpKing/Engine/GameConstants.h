#pragma once

namespace GameConstants
{
    // 윈도우 설정
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 360;
    constexpr int WINDOW_SCALE = 2;
    constexpr float TARGET_FRAME_RATE = 120.0f;

    // 플레이어 스피드
    constexpr float PLAYER_MOVE_SPEED = 70.0f;

    // 플레이어 중력
    constexpr float PLAYER_GRAVITY = 900.0f;
    constexpr float PLAYER_MAX_FALL_SPEED = 600.0f;

    // 플레이어 점프
    constexpr float PLAYER_MIN_JUMP_SPEED = 100.0f;
    constexpr float PLAYER_MAX_JUMP_SPEED = 600.0f;
    constexpr float PLAYER_MAX_JUMP_CHARGE_TIME = 0.6f;
    constexpr int PLAYER_MAX_JUMP_CHARGE_STEP = 35;

    // 플레이어 점프 방향
    constexpr float PLAYER_LOW_HORIZONTAL_JUMP_DIRECTION = 0.85f;
    constexpr float PLAYER_HORIZONTAL_JUMP_DIRECTION = 0.35f;
    constexpr float PLAYER_VERTICAL_JUMP_DIRECTION = -1.0f;

    // 플레이어 Collider 크기
    constexpr float PLAYER_COLLIDER_WIDTH = 19.0f;
    constexpr float PLAYER_COLLIDER_HEIGHT = 22.0f;

    // 플레이어 벽 반사
    constexpr float PLAYER_WALL_BOUNCE_RESTITUTION = 0.5f;

    // 플레이어 애니메이션 재생 시간(초)
    constexpr float PLAYER_MOVE_ANIM_DURATION = 0.2f;
}