#pragma once

namespace GameConstants
{
    // ������ ����
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 360;
    constexpr int WINDOW_SCALE = 2;
    constexpr float TARGET_FRAME_RATE = 120.0f;

    // �÷��̾� ���ǵ�
    constexpr float PLAYER_MOVE_SPEED = 84.0f;   

    // ������ ���Ŭ�� �̵� �ӵ�
    constexpr float PLAYER_NOCLIP_SPEED = 1500.0f;

    // �÷��̾� �߷�
    constexpr float PLAYER_GRAVITY = 972.0f;      
    constexpr float PLAYER_MAX_FALL_SPEED = 660.0f;  

    // �÷��̾� ����    
    constexpr float PLAYER_MAX_JUMP_SPEED = 660.0f;  
    constexpr float PLAYER_JUMP_SPEED_CURVE_EXPONENT = 1.13f;
    constexpr float PLAYER_JUMP_DIRECTIONAL_SPEED_BONUS = 54.0f;   
    constexpr float PLAYER_MAX_JUMP_CHARGE_TIME = 0.6f;
    constexpr int PLAYER_MAX_JUMP_CHARGE_STEP = 35;

    // �÷��̾� ���� ����
    constexpr float PLAYER_JUMP_MAX_ANGLE_RADIANS = 1.0471975512f; // π/3 (60°)
    constexpr float PLAYER_JUMP_ANGLE_CHARGE_DIVISOR = 45.5f;       
    constexpr float PLAYER_JUMP_ANGLE_CHARGE_REFERENCE = 30.0f;          

    // �÷��̾� Collider ũ��
    constexpr float PLAYER_COLLIDER_WIDTH = 19.0f;
    constexpr float PLAYER_COLLIDER_HEIGHT = 22.0f;

    // �÷��̾� �� �ݻ�
    constexpr float PLAYER_WALL_BOUNCE_RESTITUTION = 0.925f;
    constexpr float PLAYER_CEILING_BOUNCE_RESTITUTION = 0.4625f;
    constexpr float PLAYER_WALL_ANGLE_ELASTICITY = 0.5f; 

    // �÷��̾� �ִϸ��̼� ��� �ð�(��)
    constexpr float PLAYER_MOVE_ANIM_DURATION = 0.2f;

    constexpr float PLAYER_SLOPE_SLIDE_ACCEL = 1000.0f;
    constexpr float PLAYER_SLOPE_SNAP_TOLERANCE = 4.0f;
    constexpr float PLAYER_LANDING_SLIP_NORMAL = 0.0f;
    constexpr float PLAYER_LANDING_SLIP_ICE = 0.9f;

    // Hard-landing stun duration (seconds) - placeholder, tune by playtest
    constexpr float PLAYER_STUN_DURATION = 0.5f;

    // 점프 시작 파티클 재생 시간(초)
    constexpr float PARTICLE_JUMP_ANIM_DURATION = 0.2f;

    // Slope slide speed cap / friction - King.py:165,787,789
    constexpr float PLAYER_SLOPE_MAX_SLIDE_SPEED = 420.0f;
    constexpr float PLAYER_SLOPE_FRICTION = 21.0f;
    constexpr float PLAYER_SLOPE_ICE_FRICTION = 6.0f;

    // Wind - placeholder values, tune by playtest
    constexpr float WIND_PHASE_SPEED = 0.377f;   // rad/s, phase accumulator speed
    constexpr float WIND_AMPLITUDE = 6.25f;      // sine wave amplitude
    constexpr float WIND_FORCE_ACCEL = 100.0f;    // px/s^2 applied to Player::_velocity.x

    // Weather - placeholder value, tune by playtest
    constexpr float WEATHER_FRAME_INTERVAL = 0.1f;   // seconds per animation frame (4-frame loop)

    // Scrolling birds - placeholder value, tune by playtest
    constexpr float BIRD_ANIM_DURATION = 0.8f;   // seconds per full 8-frame loop (0.1s/frame)

    // Hidden walls - fade speed (opacity fraction per second, both fade-in/out)
    // JumpKingAtHome-master hiddenwalls.py: opacity -+= 55 (out of 255) per frame @ ~60fps
    // -> full fade takes about (255/55)/60 ≈ 0.077s -> ≈ 13.0/s
    constexpr float HIDDEN_WALL_FADE_SPEED = 13.0f;
}