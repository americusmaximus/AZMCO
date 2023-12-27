/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Basic.hxx"
#include "Renderer.Basic.hxx"
#include "Native.Basic.hxx"

#define RENDERER_MODULE_FAILURE FALSE
#define RENDERER_MODULE_SUCCESS TRUE

#define RENDERER_MODULE_DX7_ACCELERATION_AVAILABLE 85

#define RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS 1400

#define RENDERER_MODULE_VERSION_113 113

#define RENDERER_MODULE_SIGNATURE_D3D7 0x44334437 /* D3D7 */
#define RENDERER_MODULE_SIGNATURE_D3D8 0x44334438 /* D3D8 */

#define RENDERER_MODULE_VERSION_DX7 7
#define RENDERER_MODULE_VERSION_DX8 8

#define RENDERER_MODULE_MESSAGE_SEVERITY_ERROR 0
#define RENDERER_MODULE_MESSAGE_SEVERITY_WARNING 1
#define RENDERER_MODULE_MESSAGE_SEVERITY_MESSAGE 2

#define MAX_RENDERER_MODULE_DEVICE_NAME_LENGTH 32

#define RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY 0
#define RENDERER_MODULE_TEXTURE_LOCATION_LOCAL_VIDEO_MEMORY 1
#define RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY 2

#define RENDERER_MODULE_SELECT_STATE_MASK 0x0000FFFF
#define RENDERER_MODULE_SELECT_STATE_STAGE_MASK 0xFFFF0000

#define MAKETEXTURESTAGEMASK(x) (x << 16)
#define MAKETEXTURESTAGEVALUE(x) ((x >> 16) & RENDERER_MODULE_SELECT_STATE_MASK)

#define RENDERER_TEXTURE_STAGE_0 0
#define RENDERER_TEXTURE_STAGE_1 1
#define RENDERER_TEXTURE_STAGE_2 2
#define RENDERER_TEXTURE_STAGE_3 3
#define RENDERER_TEXTURE_STAGE_4 4
#define RENDERER_TEXTURE_STAGE_5 5
#define RENDERER_TEXTURE_STAGE_6 6
#define RENDERER_TEXTURE_STAGE_7 7

#define RENDERER_MODULE_SELECT_MIPMAP_MASK 0x0000FFFF
#define MAKETEXTUREMIPMAPVALUE(x) (x & 0x0000FFFF)

#define RENDERER_MODULE_DEVICE_TYPE_1_INVALID (-1)
#define RENDERER_MODULE_DEVICE_TYPE_1_ACCELERATED 0
#define RENDERER_MODULE_DEVICE_TYPE_1_NULL 1
#define RENDERER_MODULE_DEVICE_TYPE_1_RGB 2
#define RENDERER_MODULE_DEVICE_TYPE_1_REF 3
#define RENDERER_MODULE_DEVICE_TYPE_1_RAMP 4
#define RENDERER_MODULE_DEVICE_TYPE_1_MMX 5

#define RENDERER_MODULE_ANTIALIASING_NONE 0
#define RENDERER_MODULE_ANTIALIASING_SORT_DEPENDENT 1
#define RENDERER_MODULE_ANTIALIASING_SORT_INDEPENDENT 2

#define RENDERER_MODULE_BLEND_SOURCE_ALPHA_INVERSE_SOURCE_ALPHA 0
#define RENDERER_MODULE_BLEND_SOURCE_ALPHA_ONE 1
#define RENDERER_MODULE_BLEND_ZERO_INVERSE_SOURCE_ALPHA 2
#define RENDERER_MODULE_BLEND_DESTINATION_COLOR_ZERO 3

#define RENDERER_MODULE_BLEND_ONE 0
#define RENDERER_MODULE_BLEND_ZERO 1
#define RENDERER_MODULE_BLEND_SOURCE_ALPHA 2
#define RENDERER_MODULE_BLEND_INVERSE_SOURCE_ALPHA 3
#define RENDERER_MODULE_BLEND_DESTINATION_ALPHA 4
#define RENDERER_MODULE_BLEND_INVERSE_DESTINATION_ALPHA 5
#define RENDERER_MODULE_BLEND_SOURCE_COLOR 6
#define RENDERER_MODULE_BLEND_DESTINATION_COLOR 7
#define RENDERER_MODULE_BLEND_INVERSE_SOURCE_COLOR 8
#define RENDERER_MODULE_BLEND_INVERSE_DESTINATION_COLOR 9

#define RENDERER_MODULE_TEXTURE_TEXTURE_COLOR 0
#define RENDERER_MODULE_TEXTURE_TEXTURE_DIFFUSE_COLOR 1
#define RENDERER_MODULE_TEXTURE_BLEND_TEXTURE_ALPHA_DIFFUSE 2
#define RENDERER_MODULE_TEXTURE_MODULATE_TEXTURE_DIFFUSE_COLOR 3
#define RENDERER_MODULE_TEXTURE_SELECT_TEXTURE_COLOR 4
#define RENDERER_MODULE_TEXTURE_SUMMARIZE_TEXTURE_DIFFUSE_COLOR 5

#define RENDERER_MODULE_TEXTURE_HINT_NONE 0
#define RENDERER_MODULE_TEXTURE_HINT_DYNAMIC 1
#define RENDERER_MODULE_TEXTURE_HINT_STATIC 2

#define RENDERER_MODULE_CULL_NONE 0
#define RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE 1
#define RENDERER_MODULE_CULL_CLOCK_WISE 2

#define RENDERER_MODULE_DITHER_INACTIVE 0
#define RENDERER_MODULE_DITHER_ACTIVE 1

#define RENDERER_MODULE_DEPTH_INACTIVE 0
#define RENDERER_MODULE_DEPTH_ACTIVE 1
#define RENDERER_MODULE_DEPTH_ACTIVE_W 2

#define RENDERER_MODULE_SHADE_FLAT 0
#define RENDERER_MODULE_SHADE_GOURAUD 1
#define RENDERER_MODULE_SHADE_GOURAUD_SPECULAR 2
#define RENDERER_MODULE_SHADE_3 3 /* TODO */
#define RENDERER_MODULE_SHADE_4 4 /* TODO */

#define RENDERER_MODULE_TEXTURE_FILTER_POINT 0
#define RENDERER_MODULE_TEXTURE_FILTER_LENEAR 1
#define RENDERER_MODULE_TEXTURE_FILTER_ANISOTROPY 2

#define RENDERER_MODULE_ANTI_ALIAS_NONE 0
#define RENDERER_MODULE_ANTI_ALIAS_SORT_DEPENDENT 1
#define RENDERER_MODULE_ANTI_ALIAS_SORT_INDEPENDENT 2

#define RENDERER_MODULE_ALPHA_BLEND_NONE 0
#define RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_1 1
#define RENDERER_MODULE_ALPHA_BLEND_ACTIVE 2
#define RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_3 3

#define RENDERER_MODULE_TEXTURE_MIP_FILTER_NONE 0
#define RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT 1
#define RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT_ADVANCED 2
#define RENDERER_MODULE_TEXTURE_MIP_FILTER_LINEAR 3

#define RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP 0
#define RENDERER_MODULE_TEXTURE_ADDRESS_WRAP 1
#define RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR 2

#define RENDERER_MODULE_FOG_COLOR_MASK 0X00FFFFFF

#define RENDERER_MODULE_STATE_FLAT_FANS_INACTIVE 0
#define RENDERER_MODULE_STATE_FLAT_FANS_ACTIVE 1

#define RENDERER_MODULE_FOG_INACTIVE 0
#define RENDERER_MODULE_FOG_ACTIVE 1
#define RENDERER_MODULE_FOG_ACTIVE_LINEAR 2
#define RENDERER_MODULE_FOG_ACTIVE_EXP 4
#define RENDERER_MODULE_FOG_ACTIVE_EXP2 8

#define RENDERER_MODULE_BACK_BUFFER_INACTIVE 0
#define RENDERER_MODULE_BACK_BUFFER_ACTIVE 1

#define RENDERER_MODULE_ALPHA_TEST_0 0
#define RENDERER_MODULE_ALPHA_TEST_1 1
#define RENDERER_MODULE_ALPHA_TEST_2 2
#define RENDERER_MODULE_ALPHA_TEST_4 4
#define RENDERER_MODULE_ALPHA_TEST_8 8
#define RENDERER_MODULE_ALPHA_TEST_16 16

#define RENDERER_MODULE_DEPTH_FUNCTION_NEVER 0
#define RENDERER_MODULE_DEPTH_FUNCTION_LESS 1
#define RENDERER_MODULE_DEPTH_FUNCTION_EQUAL 2
#define RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL 3
#define RENDERER_MODULE_DEPTH_FUNCTION_GREATER 4
#define RENDERER_MODULE_DEPTH_FUNCTION_NOT_EQUAL 5
#define RENDERER_MODULE_DEPTH_FUNCTION_GREATER_EQUAL 6
#define RENDERER_MODULE_DEPTH_FUNCTION_ALWAYS 7

#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL 0
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD 1
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE 2
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE 3
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_SUBTRACT 4
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ARG2 5
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_2X 6
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_4X 7
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_TEXTURE_ALPHA 8
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED 9
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_DOT_PRODUCT_3 10
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP 11
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP_LUMINANCE 12
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_BLEND_FACTOR_ALPHA 20
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_BLEND_FACTOR_ALPHA_ARG1 21
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED_ALTERNATIVE 96
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_BLEND_FACTOR_ALPHA_ALTERNATIVE 98
#define RENDERER_MODULE_TEXTURE_STAGE_BLEND_BLEND_FACTOR_ALPHA_ARG1_ALTERNATIVE 99

#define RENDERER_MODULE_VERTEX_TYPE_RTLVX 0
#define RENDERER_MODULE_VERTEX_TYPE_RTLVX2 1

#define RENDERER_MODULE_STENCIL_INACTIVE 0
#define RENDERER_MODULE_STENCIL_ACTIVE 1
#define RENDERER_MODULE_STENCIL_UNKNOWN 2

#define RENDERER_MODULE_LOG_INACTIVE 0
#define RENDERER_MODULE_LOG_ACTIVE 1

#define RENDERER_MODULE_HINT_INACTIVE 0
#define RENDERER_MODULE_HINT_ACTIVE 1

#define RENDERER_MODULE_LINE_DOUBLE_INACTIVE 0
#define RENDERER_MODULE_LINE_DOUBLE_ACTIVE 1

#define RENDERER_MODULE_STENCIL_FUNCTION_NEVER 0
#define RENDERER_MODULE_STENCIL_FUNCTION_LESS 1
#define RENDERER_MODULE_STENCIL_FUNCTION_EQUAL 2
#define RENDERER_MODULE_STENCIL_FUNCTION_LESS_EQUAL 3
#define RENDERER_MODULE_STENCIL_FUNCTION_GREATER 4
#define RENDERER_MODULE_STENCIL_FUNCTION_NOT_EQUAL 5
#define RENDERER_MODULE_STENCIL_FUNCTION_GREATER_EQUAL 6
#define RENDERER_MODULE_STENCIL_FUNCTION_ALWAYS 7

#define RENDERER_MODULE_STENCIL_FAIL_KEEP 0
#define RENDERER_MODULE_STENCIL_FAIL_ZERO 1
#define RENDERER_MODULE_STENCIL_FAIL_REPLACE 2
#define RENDERER_MODULE_STENCIL_FAIL_INCREMENT_CLAMP 3
#define RENDERER_MODULE_STENCIL_FAIL_DECREMENT_CLAMP 4
#define RENDERER_MODULE_STENCIL_FAIL_INVERT 5
#define RENDERER_MODULE_STENCIL_FAIL_INCREMENT 7
#define RENDERER_MODULE_STENCIL_FAIL_DECREMENT 8

#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_KEEP 0
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_ZERO 1
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_REPLACE 2
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT_CLAMP 3
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT_CLAMP 4
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_INVERT 5
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT 7
#define RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT 8

#define RENDERER_MODULE_STENCIL_PASS_KEEP 0
#define RENDERER_MODULE_STENCIL_PASS_ZERO 1
#define RENDERER_MODULE_STENCIL_PASS_REPLACE 2
#define RENDERER_MODULE_STENCIL_PASS_INCREMENT_CLAMP 3
#define RENDERER_MODULE_STENCIL_PASS_DECREMENT_CLAMP 4
#define RENDERER_MODULE_STENCIL_PASS_INVERT 5
#define RENDERER_MODULE_STENCIL_PASS_INCREMENT 7
#define RENDERER_MODULE_STENCIL_PASS_DECREMENT 8

#define RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE 0
#define RENDERER_MODULE_ALPHA_BLEND_SOURCE_ONE 1
#define RENDERER_MODULE_ALPHA_BLEND_ZERO_INVERSE_SOURCE 2
#define RENDERER_MODULE_ALPHA_BLEND_DESTINATION_COLOR_ZERO 3

#define RENDERER_MODULE_TOGGLE_NO_VERTICAL_SYNC 0
#define RENDERER_MODULE_TOGGLE_VERTICAL_SYNC 1

#define RENDERER_MODULE_ALPHA_FUNCTION_NEVER 0
#define RENDERER_MODULE_ALPHA_FUNCTION_LESS 1
#define RENDERER_MODULE_ALPHA_FUNCTION_EQUAL 2
#define RENDERER_MODULE_ALPHA_FUNCTION_LESS_EQUAL 3
#define RENDERER_MODULE_ALPHA_FUNCTION_GREATER 4
#define RENDERER_MODULE_ALPHA_FUNCTION_NOT_EQUAL 5
#define RENDERER_MODULE_ALPHA_FUNCTION_GREATER_EQUAL 6
#define RENDERER_MODULE_ALPHA_FUNCTION_ALWAYS 7

#define RENDERER_MODULE_FILL_SOLID 0
#define RENDERER_MODULE_FILL_WIRE 1

#define RENDERER_MODULE_ACCELERATION_NORMAL 0
#define RENDERER_MODULE_ACCELERATION_TRANSFORM_AND_LIGHT 1

#define RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_0 0
#define RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_1 1
#define RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_2 2
#define RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_3 3

#define RENDERER_MODULE_INDEX_SIZE_1 1 /* U8 */
#define RENDERER_MODULE_INDEX_SIZE_2 2 /* U16 */
#define RENDERER_MODULE_INDEX_SIZE_4 4 /* U32 */
#define RENDERER_MODULE_INDEX_SIZE_8 8 /* U64 */

#define RENDERER_MODULE_STATE_NONE 0
#define RENDERER_MODULE_STATE_SELECT_TEXTURE 1
#define RENDERER_MODULE_STATE_SELECT_CULL_STATE 2
#define RENDERER_MODULE_STATE_SELECT_MATERIAL 3 /* BACKGROUND COLOR */
#define RENDERER_MODULE_STATE_SELECT_DEPTH_STATE 4
#define RENDERER_MODULE_STATE_SELECT_DITHER_STATE 5
#define RENDERER_MODULE_STATE_SELECT_SHADE_STATE 6
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE 7
#define RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE 8
#define RENDERER_MODULE_STATE_SELECT_ANTI_ALIAS_STATE 9
#define RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE 10 /* TRANSPARENCY */
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE 11
#define RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR 12
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE 13 /* TEXTURE CLAMP */
#define RENDERER_MODULE_STATE_SELECT_FOG_DENSITY 14
#define RENDERER_MODULE_STATE_SELECT_FOG_COLOR 15
#define RENDERER_MODULE_STATE_SELECT_LINE_WIDTH 16
#define RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE 17
#define RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE 18
#define RENDERER_MODULE_STATE_SELECT_LAMBDAS 19
#define RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS 20
#define RENDERER_MODULE_STATE_SELECT_FOG_STATE 21
#define RENDERER_MODULE_STATE_SELECT_FOG_START 22
#define RENDERER_MODULE_STATE_SELECT_FOG_END 23
#define RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE 24
#define RENDERER_MODULE_STATE_SELECT_WINDOW 25
#define RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA 26
#define RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA 27
#define RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA 28
#define RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE 29
#define RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE 30
#define RENDERER_MODULE_STATE_SELECT_VERSION 31
#define RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA 32
#define RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA 33
#define RENDERER_MODULE_STATE_34 34
#define RENDERER_MODULE_STATE_RESTORE_RENDERER_SURFACES 35
#define RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE 36
#define RENDERER_MODULE_STATE_37 37
#define RENDERER_MODULE_STATE_ACQUIRE_RENDERER_INSTANCE 38
#define RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA 39
#define RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION 40
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE 41
#define RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE 42
#define RENDERER_MODULE_STATE_SELECT_BACK_BUFFER_STATE 43
#define RENDERER_MODULE_STATE_SELECT_PALETTE 44
#define RENDERER_MODULE_STATE_45 45
#define RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE 46
#define RENDERER_MODULE_STATE_SELECT_STENCIL_STATE 47
#define RENDERER_MODULE_STATE_SELECT_STENCIL_FUNCTION 48
#define RENDERER_MODULE_STATE_49 49
#define RENDERER_MODULE_STATE_50 50
#define RENDERER_MODULE_STATE_51 51
#define RENDERER_MODULE_STATE_SELECT_STENCIL_FAIL_STATE 52
#define RENDERER_MODULE_STATE_SELECT_STENCIL_DEPTH_FAIL_STATE 53
#define RENDERER_MODULE_STATE_SELECT_STENCIL_PASS_STATE 54
#define RENDERER_MODULE_STATE_55 55
#define RENDERER_MODULE_STATE_SELECT_BLEND_STATE 56
#define RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE 57
#define RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE 58
#define RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX 59
#define RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE 60 /* FLIPRATE */
#define RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE 61
#define RENDERER_MODULE_STATE_62 62
#define RENDERER_MODULE_STATE_ACQUIRE_GUARD_BANDS 63
#define RENDERER_MODULE_STATE_SELCT_ALPHA_FUNCTION 64
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_U 65
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_V 66
#define RENDERER_MODULE_STATE_SELECT_CLEAR_DEPTH_STATE 67
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE 68
#define RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_ACTIVE_STATE 69
#define RENDERER_MODULE_STATE_INDEX_SIZE 70
#define RENDERER_MODULE_STATE_SELECT_LOG_STATE 71
#define RENDERER_MODULE_STATE_72 72
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_MIN_FILTER_STATE 73
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_MAG_FILTER_STATE 74
#define RENDERER_MODULE_STATE_75 75
#define RENDERER_MODULE_STATE_76 76
#define RENDERER_MODULE_STATE_77 77
#define RENDERER_MODULE_STATE_78 78
#define RENDERER_MODULE_STATE_79 79
#define RENDERER_MODULE_STATE_80 80
#define RENDERER_MODULE_STATE_81 81
#define RENDERER_MODULE_STATE_82 82
#define RENDERER_MODULE_STATE_83 83
#define RENDERER_MODULE_STATE_84 84
#define RENDERER_MODULE_STATE_85 85
#define RENDERER_MODULE_STATE_86 86
#define RENDERER_MODULE_STATE_87 87
#define RENDERER_MODULE_STATE_88 88
#define RENDERER_MODULE_STATE_89 89
#define RENDERER_MODULE_STATE_90 90
#define RENDERER_MODULE_STATE_91 91
#define RENDERER_MODULE_STATE_92 92
#define RENDERER_MODULE_STATE_93 93
#define RENDERER_MODULE_STATE_94 94
#define RENDERER_MODULE_STATE_95 95
#define RENDERER_MODULE_STATE_96 96
#define RENDERER_MODULE_STATE_97 97
#define RENDERER_MODULE_STATE_98 98
#define RENDERER_MODULE_STATE_99 99
#define RENDERER_MODULE_STATE_100 100
#define RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE_ALTERNATIVE 101
#define RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE_ALTERNATIVE 102
#define RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE_ALTERNATIVE 103
#define RENDERER_MODULE_STATE_SELECT_BLEND_STATE_ALTERNATIVE 104
#define RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS_ALTERNATIVE 105
#define RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE_ALTERNATIVE 106
#define RENDERER_MODULE_STATE_107 107
#define RENDERER_MODULE_STATE_108 108
#define RENDERER_MODULE_STATE_MAX_PENDING_STATE 109
#define RENDERER_MODULE_STATE_110 110
#define RENDERER_MODULE_STATE_SELECT_HINT_STATE 304
#define RENDERER_MODULE_STATE_SELECT_LINE_DOUBLE_STATE 305
#define RENDERER_MODULE_STATE_SELECT_DEVICE_TYPE 400
#define RENDERER_MODULE_STATE_401 401
#define RENDERER_MODULE_STATE_ACQUIRE_DEVICE_CAPABILITIES_DX8 401
#define RENDERER_MODULE_STATE_402 402
#define RENDERER_MODULE_STATE_SELECT_SOURCE_BLEND_STATE_DX8 402
#define RENDERER_MODULE_STATE_ACQUIRE_DEVICE_CAPABILITIES 403
#define RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND_STATE_DX8 403
#define RENDERER_MODULE_STATE_SELECT_SOURCE_BLEND_STATE 404
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_FACTOR_DX8 404
#define RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND_STATE 405
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_MATRIX_DX8 405
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_STATE 406
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_SCALE_DX8 406
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_HINT_STATE 407
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_OFFSET_DX8 407
#define RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_ACTIVE_STATE_ALTERNATIVE 408
#define RENDERER_MODULE_STATE_RENDER_PACKET_DX8 408
#define RENDERER_MODULE_STATE_SELECT_FILL_MODE_STATE 409
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE_DX8 409
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_FACTOR 410
#define RENDERER_MODULE_STATE_ACQUIRE_MAX_ANISOTROPY_DX8 410
#define RENDERER_MODULE_STATE_ACQUIRE_RENDERER_OBJECTS 411
#define RENDERER_MODULE_STATE_SELECT_MAX_ANISOTROPY_DX8 411
#define RENDERER_MODULE_STATE_ACQUIRE_MAIN_RENDERER_SURFACE 412
#define RENDERER_MODULE_STATE_SELECT_VERTEX_STREAM_STRIDE_DX8 412
#define RENDERER_MODULE_STATE_ACQUIRE_BACK_RENDERER_SURFACE 413
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_MATRIX 415
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_SCALE 416
#define RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_OFFSET 417
#define RENDERER_MODULE_STATE_SELECT_TOGGLE_ACTIVE_STATE 418
#define RENDERER_MODULE_STATE_TEST_COOPERATIVE_LEVEL 419
#define RENDERER_MODULE_STATE_SELECT_ACCELERATION_TYPE 420
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_WORLD 421
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_VIEW 422
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_PROJECTION 423
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_WORLD 424
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_VIEW 425
#define RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_PROJECTION 426
#define RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_WORLD 427
#define RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_VIEW 428
#define RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_PROJECTION 429
#define RENDERER_MODULE_STATE_SELECT_LIGHT 430
#define RENDERER_MODULE_STATE_ACQUIRE_LIGHT 431
#define RENDERER_MODULE_STATE_SELECT_LIGHT_STATE 432
#define RENDERER_MODULE_STATE_SELECT_CURRENT_MATERIAL 433
#define RENDERER_MODULE_STATE_ACQUIRE_CURRENT_MATERIAL 434
#define RENDERER_MODULE_STATE_RENDER_PACKET 435
#define RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_AND_LIGHT_CAPABILITIES 436
#define RENDERER_MODULE_STATE_SELECT_AMBIENT_STATE 437
#define RENDERER_MODULE_STATE_SELECT_CLIPPING_STATE 438
#define RENDERER_MODULE_STATE_SELECT_LIGHTING_STATE 439
#define RENDERER_MODULE_STATE_INITIALIZE_VERTEX_BUFFER 440
#define RENDERER_MODULE_STATE_ACQUIRE_RENDERER_DEVICE_DX8 440
#define RENDERER_MODULE_STATE_441 441
#define RENDERER_MODULE_STATE_SELECT_WINDOW_STENCIL_STATE_DX8 441
#define RENDERER_MODULE_STATE_442 442
#define RENDERER_MODULE_STATE_RENDER_BUFFERED_PACKET 443
#define RENDERER_MODULE_STATE_OPTIMIZE_VERTEX_BUFFER 444
#define RENDERER_MODULE_STATE_SELECT_CLIP_PLANE 445
#define RENDERER_MODULE_STATE_446 446
#define RENDERER_MODULE_STATE_SELECT_CLIP_STATE 447
#define RENDERER_MODULE_STATE_SELECT_VERTEX_BLEND_STATE 448
#define RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE_ALTERNATIVE 449
#define RENDERER_MODULE_STATE_450 450
#define RENDERER_MODULE_STATE_ACQUIRE_MODULATE_BLENDING_STATE 451
#define RENDERER_MODULE_STATE_SELECT_CLIPPING_STATE_DX8 499

#define RENDERER_MODULE_CAPS_NONE 0
#define RENDERER_MODULE_CAPS_LINE_WIDTH 0x1
#define RENDERER_MODULE_CAPS_TEXTURE_SQUARE 0x2
#define RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 0x4
#define RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2 0x8
#define RENDERER_MODULE_CAPS_SOFTWARE 0x10
#define RENDERER_MODULE_CAPS_WINDOWED 0x20
#define RENDERER_MODULE_CAPS_GLOBAL_CUT 0x40
#define RENDERER_MODULE_CAPS_TRILINEAR_PASS 0x80
#define RENDERER_MODULE_CAPS_UNKNOWN 0x100

#define RENDERER_MODULE_DEVICE_TYPE_ENVIRONMENT_PROPERTY_NAME "THRASH_D3DDEVICETYPE"
#define RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME "THRASH_DISPLAY"

#define RENDERER_MODULE_WIRE_FRAME_DX7_ENVIRONMENT_PROPERTY_NAME "DX7_WIREFRAME"
#define RENDERER_MODULE_WIRE_FRAME_DX8_ENVIRONMENT_PROPERTY_NAME "DX8_WIREFRAME"

#define RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE 0x464
#define RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES 0x465
#define RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE 0x468

typedef const HWND(STDCALLAPI* RENDERERMODULEACQUIREWINDOWLAMBDA)(void);
typedef const u32(STDCALLAPI* RENDERERMODULEEXECUTECALLBACK)(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result); // TODO
typedef const u32(STDCALLAPI* RENDERERMODULERELEASEMEMORYLAMBDA)(const void*);
typedef const void(STDCALLAPI* RENDERERMODULEEXECUTELAMBDA)(const u32 message, const RENDERERMODULEEXECUTECALLBACK lambda);
typedef const void(STDCALLAPI* RENDERERMODULELOCKWINDOWLAMBDA)(const BOOL state);
typedef const void(STDCALLAPI* RENDERERMODULELOGLAMBDA)(const u32 severity, const char* message);
typedef const void(STDCALLAPI* RENDERERMODULESELECTINSTANCELAMBDA)(const void*);
typedef const void(STDCALLAPI* RENDERERMODULESELECTSTATELAMBDA)(const u32, void*);
typedef void* (STDCALLAPI* RENDERERMODULEALLOCATEMEMORYLAMBDA)(const u32 size);

namespace RendererModule
{
    struct RendererModuleWindowLock
    {
        void* Data;
        u32 Stride;
        s32 Format;
        u32 Width;
        u32 Height;
    };

    // NOTE: Used in DirectX 7 renderers.
    struct RendererModuleDeviceCapabilities7
    {
        BOOL IsAccelerated;
        u32 RendererDepthBits;
        u32 RenderScreenBits;
        u32 RendererDeviceDepthBits;
        BOOL IsDepthVideoMemoryAvailable;
        BOOL IsDepthAvailable;
        BOOL IsGreenAllowSixBits;
        BOOL IsVideoMemoryAvailable;
        BOOL IsDitherAvailable;
        BOOL IsNoVerticalSync;
        BOOL IsWBufferAvailable;
        BOOL IsWFogAvailable;
        BOOL IsWindowModeAvailable;
        BOOL IsInterpolationAvailable;
        BOOL IsDepthRemovalAvailable;
        BOOL IsPerspectiveTextures;
        BOOL IsAlphaBlending;
        BOOL IsAlphaProperBlending;
        BOOL IsAlphaTextures;
        BOOL IsModulateBlending;
        BOOL IsSourceAlphaBlending;
        BOOL IsAntiAliasingAvailable;
        BOOL IsColorBlending;
        BOOL IsAnisotropyAvailable;
        BOOL IsGammaAvailable;
        BOOL IsSpecularGouraudBlending;
        BOOL IsStencilBufferAvailable;
        BOOL IsSpecularBlending;
        s32 Unk29; // TODO
        BOOL IsTextureIndependentUVs;
        BOOL IsMipMapBiasAvailable;
        s32 Unk32; // TODO
        u32 MinTextureWidth;
        u32 MaxTextureWidth;
        u32 MinTextureHeight;
        u32 MaxTextureHeight;
        u32 MultipleTextureWidth;
        BOOL IsPowerOfTwoTexturesWidth;
        u32 MultipleTextureHeight;
        BOOL IsPowerOfTwoTexturesHeight;
        u32 MaximumSimultaneousTextures;
        BOOL IsSquareOnlyTextures;
        BOOL IsAntiAliasEdges;
        f32 GuardBandLeft;
        f32 GuardBandRight;
        f32 GuardBandTop;
        f32 GuardBandBottom;
        f32 MaxTextureRepeat;
    };

    struct RendererModuleLambdaContainer
    {
        void* L1; // TODO
        RENDERERMODULEACQUIREWINDOWLAMBDA AcquireWindow;
        RENDERERMODULEEXECUTELAMBDA Execute;
        void* L4; // TODO
        RENDERERMODULESELECTINSTANCELAMBDA SelectInstance;
        RENDERERMODULELOCKWINDOWLAMBDA LockWindow;
        RENDERERMODULELOGLAMBDA Log;
        void* L8; // TODO
    };

    struct RendererModuleDescriptorDeviceCapabilities
    {
        u32 Width;
        u32 Height;
        u32 Bits;
        u32 Format;

        BOOL IsActive;

        u32 Unk03; // TODO
        u32 Unk04; // TODO
        u32 Unk05; // TODO
        u32 Unk06; // TODO
        u32 Unk07; // TODO
    };

    struct RendererModuleDescriptor
    {
        u32 Signature;
        s32 Unk1; // TODO
        u32 Version;
        u32 Caps;
        u32 MinimumTextureWidth;
        u32 MaximumTextureWidth;
        u32 MultipleTextureWidth;
        u32 MinimumTextureHeight;
        u32 MaximumTextureHeight;
        u32 MultipleTextureHeight;
        u32 ClipAlign;

        s32 Unk4; // TODO

        s32* TextureFormatStates; // TODO

        s32 Unk5; // TODO
        s32* Unk6; // TODO

        struct
        {
            u32 Count;
            RendererModuleDescriptorDeviceCapabilities* Capabilities;
        } Capabilities;

        u32 MaximumSimultaneousTextures;
        s32 Unk7; // TODO
        char Name[MAX_RENDERER_MODULE_DEVICE_NAME_LENGTH];
        u32 SubType;

        u32 MemorySize;
        u32 MemoryType;

        // DirectX 6.0

        const char* Author;

        u32 DXV;
    };

    // NOTE: D3DLIGHTTYPE
    typedef enum RendererModuleLightType
    {
        RendererModuleLightTypeNone = 0,
        RendererModuleLightTypePoint = 1,
        RendererModuleLightTypeSpot = 2,
        RendererModuleLightTypeDirectional = 3,
        RendererModuleLightTypeMax = U32_MAX
    } RendererModuleLightType;

    // NOTE: D3DLIGHT7
    struct RendererModuleLight
    {
        RendererModuleLightType Type;
        u32 Diffuse;
        u32 Specular;
        u32 Ambient;
        f32x3 Position;
        f32x3 Direction;
        f32 Range;
        f32 Falloff;
        f32 Attenuation0;
        f32 Attenuation1;
        f32 Attenuation2;
        f32 Theta;
        f32 Phi;
    };

    struct RendererModuleMaterial
    {
        u32 Diffuse;
        u32 Ambient;
        u32 Specular;
        u32 Emissive;
        f32 Power;
    };

    typedef enum RendererModulePrimitiveType
    {
        RendererModulePrimitiveTypeNone = 0,
        RendererModulePrimitiveTypePointList = 1,
        RendererModulePrimitiveTypeLineList = 2,
        RendererModulePrimitiveTypeLineStrip = 3,
        RendererModulePrimitiveTypeTriangleList = 4,
        RendererModulePrimitiveTypeTriangleStrip = 5,
        RendererModulePrimitiveTypeTriangleFan = 6,
        RendererModulePrimitiveTypeMax = U32_MAX
    } RendererModulePrimitiveType;

    struct RendererModulePacket
    {
        RendererModulePrimitiveType Type;
        u32 FVF;
        void* Vertexes;
        u32 VertexCount;
        u16* Indexes;
        u32 IndexCount;
    };

    struct RendererModuleBufferPacket
    {
        void* Vertexes;
        RendererModulePrimitiveType Type;
        s32 Unk02; // TODO
        s32 Unk03; // TODO
        s32 Unk04; // TODO
        u32 VertexCount;
        u16* Indexes;
        u32 IndexCount;
    };

    struct RendererModuleGuardBands
    {
        s32 Left;
        s32 Right;
        s32 Top;
        s32 Bottom;
    };

    struct RendererModuleTransformAndLightCapabilites
    {
        BOOL IsActive;
        u32 MaxActiveLights;
        s32 Unk03; // TODO
        u32 MaxUserClipPlanes;
        u32 MaxVertexBlendMatrices;
        BOOL IsTransformLightBufferSystemMemoryAvailable;
        BOOL IsTransformLightBufferVideoMemoryAvailable;
    };

    struct RendererModuleTextureStageBumpMappingMatrix
    {
        f32 M00, M01, M10, M11;
    };

    struct RendererModuleVertexBuffer
    {
        void* Buffer;
        s32 Unk01; // TODO
        u32 FVF;
        u32 Capabilities;
        s32 Unk04; // TODO
        u32 VertexCount;
    };
}