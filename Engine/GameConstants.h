#pragma once

namespace GameConstants
{
    // 윈도우 설정
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 360;
    constexpr int WINDOW_SCALE = 2;
    constexpr float TARGET_FRAME_RATE = 120.0f;

    // 플레이어 스피드
    constexpr float PLAYER_MOVE_SPEED = 84.0f;

    // 디버그 노클립 이동 속도
    constexpr float PLAYER_NOCLIP_SPEED = 1500.0f;

    // 플레이어 중력
    constexpr float PLAYER_GRAVITY = 972.0f;
    constexpr float PLAYER_MAX_FALL_SPEED = 660.0f;

    // 플레이어 점프
    constexpr float PLAYER_MAX_JUMP_SPEED = 660.0f;
    constexpr float PLAYER_JUMP_SPEED_CURVE_EXPONENT = 1.13f;
    constexpr float PLAYER_JUMP_DIRECTIONAL_SPEED_BONUS = 54.0f;
    constexpr float PLAYER_MAX_JUMP_CHARGE_TIME = 0.6f;
    constexpr int PLAYER_MAX_JUMP_CHARGE_STEP = 35;

    // 플레이어 점프 각도
    constexpr float PLAYER_JUMP_MAX_ANGLE_RADIANS = 1.0471975512f; // π/3 (60°)
    constexpr float PLAYER_JUMP_ANGLE_CHARGE_DIVISOR = 45.5f;
    constexpr float PLAYER_JUMP_ANGLE_CHARGE_REFERENCE = 30.0f;

    // 플레이어 Collider 크기
    constexpr float PLAYER_COLLIDER_WIDTH = 19.0f;
    constexpr float PLAYER_COLLIDER_HEIGHT = 22.0f;

    // 플레이어 벽 반사
    constexpr float PLAYER_WALL_BOUNCE_RESTITUTION = 0.925f;
    constexpr float PLAYER_CEILING_BOUNCE_RESTITUTION = 0.4625f;
    constexpr float PLAYER_WALL_ANGLE_ELASTICITY = 0.5f;

    // 플레이어 애니메이션 재생 시간(초)
    constexpr float PLAYER_MOVE_ANIM_DURATION = 0.2f;

    constexpr float PLAYER_SLOPE_SLIDE_ACCEL = 1000.0f;
    constexpr float PLAYER_SLOPE_SNAP_TOLERANCE = 4.0f;
    constexpr float PLAYER_LANDING_SLIP_NORMAL = 0.0f;
    constexpr float PLAYER_LANDING_SLIP_ICE = 0.9f;

    // 하드 랜딩 스턴 지속시간(초) - 임시값, 플레이테스트로 조정 필요
    constexpr float PLAYER_STUN_DURATION = 0.5f;

    // 점프 시작 파티클 재생 시간(초)
    constexpr float PARTICLE_JUMP_ANIM_DURATION = 0.2f;

    // 경사면 슬라이드 속도 상한 / 마찰 - King.py:165,787,789
    constexpr float PLAYER_SLOPE_MAX_SLIDE_SPEED = 420.0f;
    constexpr float PLAYER_SLOPE_FRICTION = 21.0f;
    constexpr float PLAYER_SLOPE_ICE_FRICTION = 6.0f;

    // 바람 - 임시값
    constexpr float WIND_PHASE_SPEED = 0.377f;   // rad/s, 위상 누적 속도
    constexpr float WIND_AMPLITUDE = 6.25f;      // 사인파 진폭
    constexpr float WIND_FORCE_ACCEL = 100.0f;    // Player::_velocity.x에 적용되는 px/s^2

    // 날씨 - 임시값
    constexpr float WEATHER_FRAME_INTERVAL = 0.1f;   // 애니메이션 프레임당 초 (4프레임 루프)

    // 스크롤링 새 - 임시값
    constexpr float BIRD_ANIM_DURATION = 0.8f;   // 8프레임 전체 루프당 초 (프레임당 0.1초)

    // 히든 월 - 페이드 속도 (초당 불투명도 비율, 페이드인/아웃 공통)
    // -> 완전히 페이드되는 데 약 (255/55)/60 ≈ 0.077초 -> ≈ 13.0/초
    constexpr float HIDDEN_WALL_FADE_SPEED = 13.0f;
}