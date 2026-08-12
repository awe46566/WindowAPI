#pragma once

namespace GameConstants
{
    // ������ ����
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 360;
    constexpr int WINDOW_SCALE = 2;
    constexpr float TARGET_FRAME_RATE = 120.0f;

    // �÷��̾� ���ǵ�
    constexpr float PLAYER_MOVE_SPEED = 70.0f;

    // ������ ���Ŭ�� �̵� �ӵ�
    constexpr float PLAYER_NOCLIP_SPEED = 1500.0f;

    // �÷��̾� �߷�
    constexpr float PLAYER_GRAVITY = 900.0f;
    constexpr float PLAYER_MAX_FALL_SPEED = 600.0f;

    // �÷��̾� ����
    constexpr float PLAYER_MIN_JUMP_SPEED = 100.0f;
    constexpr float PLAYER_MAX_JUMP_SPEED = 600.0f;
    constexpr float PLAYER_MAX_JUMP_CHARGE_TIME = 0.6f;
    constexpr int PLAYER_MAX_JUMP_CHARGE_STEP = 35;

    // �÷��̾� ���� ����
    constexpr float PLAYER_LOW_HORIZONTAL_JUMP_DIRECTION = 0.85f;
    constexpr float PLAYER_HORIZONTAL_JUMP_DIRECTION = 0.35f;
    constexpr float PLAYER_VERTICAL_JUMP_DIRECTION = -1.0f;

    // �÷��̾� Collider ũ��
    constexpr float PLAYER_COLLIDER_WIDTH = 19.0f;
    constexpr float PLAYER_COLLIDER_HEIGHT = 22.0f;

    // �÷��̾� �� �ݻ�
    constexpr float PLAYER_WALL_BOUNCE_RESTITUTION = 0.5f;

    // �÷��̾� �ִϸ��̼� ��� �ð�(��)
    constexpr float PLAYER_MOVE_ANIM_DURATION = 0.2f;

    // Ground movement (land) - placeholder values, tune by playtest
    constexpr float PLAYER_GROUND_ACCEL = 600.0f;
    constexpr float PLAYER_GROUND_FRICTION = 900.0f;
    constexpr float PLAYER_MAX_GROUND_SPEED = PLAYER_MOVE_SPEED;

    // Ice / slope movement - placeholder values, tune by playtest
    constexpr float PLAYER_ICE_ACCEL = 250.0f;
    constexpr float PLAYER_ICE_FRICTION = 80.0f;
    constexpr float PLAYER_SLOPE_SLIDE_ACCEL = 500.0f;
    constexpr float PLAYER_SLOPE_SNAP_TOLERANCE = 4.0f;
    constexpr float PLAYER_ICE_LANDING_SPEED_RETENTION = 0.5f;

    // Slope slide speed cap / friction - placeholder values, tune by playtest
    constexpr float PLAYER_SLOPE_MAX_SLIDE_SPEED = 250.0f;
    constexpr float PLAYER_SLOPE_FRICTION = 100.0f;
    constexpr float PLAYER_SLOPE_ICE_FRICTION = 50.0f;
}