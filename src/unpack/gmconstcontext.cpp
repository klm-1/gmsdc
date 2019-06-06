#include "gmlexprcontext.h"

#include <map>
#include <string>

using namespace std;


const map<ExprContext::Type, map<int, string>> ExprContext::flagVal_{
    {
        FileAttr, {
            { 1, "fa_readonly" },
            { 2, "fa_hidden" },
            { 4, "fa_sysfile" },
            { 8, "fa_volumeid" },
            { 16, "fa_directory" },
            { 32, "fa_archive" },
        }
    },
    {
        PhysicsDrawFlag, {
            { 1, "phy_debug_render_shapes" },
            { 2, "phy_debug_render_joints" },
            { 4, "phy_debug_render_coms" },
            { 8, "phy_debug_render_aabb" },
            { 16, "phy_debug_render_obb" },
            { 32, "phy_debug_render_core_shapes" },
            { 64, "phy_debug_render_collision_pairs" },
        }
    },
    {
        PhysicsPartTypeFlag, {
            { 0, "phy_particle_flag_water" },   // Yep, zero flag!
            { 2, "phy_particle_flag_zombie" },
            { 4, "phy_particle_flag_wall" },
            { 8, "phy_particle_flag_spring" },
            { 16, "phy_particle_flag_elastic" },
            { 32, "phy_particle_flag_viscous" },
            { 64, "phy_particle_flag_powder" },
            { 128, "phy_particle_flag_tensile" },
            { 256, "phy_particle_flag_colourmixing" },
        }
    },
    {
        PhysicsPartDataFlag, {
            { 1, "phy_particle_data_flag_typeflags" },
            { 2, "phy_particle_data_flag_position" },
            { 4, "phy_particle_data_flag_velocity" },
            { 8, "phy_particle_data_flag_colour" },
            { 16, "phy_particle_data_flag_category" },
        }
    },
    {
        PhysicsGroupFlag, {
            { 1, "phy_particle_group_flag_solid" },
            { 2, "phy_particle_group_flag_rigid" },
        }
    },
};

const map<ExprContext::Type, map<int, string>> ExprContext::contextVal_{
    {
        Color, {
            { 0, "c_black" },
            { 128, "c_maroon" },
            { 255, "c_red" },
            { 32768, "c_green" },
            { 32896, "c_olive" },
            { 65280, "c_lime" },
            { 65535, "c_yellow" },
            { 4210752, "c_dkgray" },
            { 4235519, "c_orange" },
            { 8388608, "c_navy" },
            { 8388736, "c_purple" },
            { 8421376, "c_teal" },
            { 8421504, "c_gray" },
            // { 12632256, "c_ltgray" },
            { 12632256, "c_silver" },
            { 16711680, "c_blue" },
            { 16711935, "c_fuchsia" },
            { 16776960, "c_aqua" },
            { 16777215, "c_white" },
        }
    },
    {
        Bool, {
            { 0, "false" },
            { 1, "true" },
        }
    },
    {
        HAlign, {
            { 0, "fa_left" },
            { 1, "fa_center" },
            { 2, "fa_right" }
        }
    },
    {
        VAlign, {
            { 0, "fa_top" },
            { 1, "fa_center" },
            { 2, "fa_bottom" },
        }
    },
    {
        MouseButton, {
            { -1, "mb_any" },
            { 0, "mb_none" },
            { 1, "mb_left" },
            { 2, "mb_right" },
            { 3, "mb_middle" },
        }
    },
    {
        KeyCode, {
            { 0, "vk_nokey" },
            { 1, "vk_anykey" },
            { 8, "vk_backspace" },
            { 9, "vk_tab" },
            { 13, "vk_enter" },
            { 16, "vk_shift" },
            { 17, "vk_control" },
            { 18, "vk_alt" },
            { 19, "vk_pause" },
            { 27, "vk_escape" },
            { 32, "vk_space" },
            { 33, "vk_pageup" },
            { 34, "vk_pagedown" },
            { 35, "vk_end" },
            { 36, "vk_home" },
            { 37, "vk_left" },
            { 38, "vk_up" },
            { 39, "vk_right" },
            { 40, "vk_down" },
            { 44, "vk_printscreen" },
            { 45, "vk_insert" },
            { 46, "vk_delete" },
            { 96, "vk_numpad0" },
            { 97, "vk_numpad1" },
            { 98, "vk_numpad2" },
            { 99, "vk_numpad3" },
            { 100, "vk_numpad4" },
            { 101, "vk_numpad5" },
            { 102, "vk_numpad6" },
            { 103, "vk_numpad7" },
            { 104, "vk_numpad8" },
            { 105, "vk_numpad9" },
            { 106, "vk_multiply" },
            { 107, "vk_add" },
            { 109, "vk_subtract" },
            { 110, "vk_decimal" },
            { 111, "vk_divide" },
            { 112, "vk_f1" },
            { 113, "vk_f2" },
            { 114, "vk_f3" },
            { 115, "vk_f4" },
            { 116, "vk_f5" },
            { 117, "vk_f6" },
            { 118, "vk_f7" },
            { 119, "vk_f8" },
            { 120, "vk_f9" },
            { 121, "vk_f10" },
            { 122, "vk_f11" },
            { 123, "vk_f12" },
            { 160, "vk_lshift" },
            { 161, "vk_rshift" },
            { 162, "vk_lcontrol" },
            { 163, "vk_rcontrol" },
            { 164, "vk_lalt" },
            { 165, "vk_ralt" },
        }
    },
    {
        AchType, {
            { 0, "achievement_type_achievement_challenge" },
            { 1, "achievement_type_score_challenge" },
        }
    },
    {
        GamepadInput, {
            { 32769, "gp_face1" },
            { 32770, "gp_face2" },
            { 32771, "gp_face3" },
            { 32772, "gp_face4" },
            { 32773, "gp_shoulderl" },
            { 32775, "gp_shoulderlb" },
            { 32774, "gp_shoulderr" },
            { 32776, "gp_shoulderrb" },
            { 32777, "gp_select" },
            { 32778, "gp_start" },
            { 32779, "gp_stickl" },
            { 32780, "gp_stickr" },
            { 32781, "gp_padu" },
            { 32782, "gp_padd" },
            { 32783, "gp_padl" },
            { 32784, "gp_padr" },
            { 32785, "gp_axislh" },
            { 32786, "gp_axislv" },
            { 32787, "gp_axisrh" },
            { 32788, "gp_axisrv" },
        }
    },
    {
        AudioType, {
            { 0, "audio_mono" },
            { 1, "audio_stereo" },
            { 2, "audio_3d" },
        }
    },
    {
        BlendMode, {
            { 0, "bm_normal" },
            { 1, "bm_add" },
            { 3, "bm_subtract" },
            { 2, "bm_max" },
        }
    },
    {
        BlendModeExt, {
            { 1, "bm_zero" },
            { 2, "bm_one" },
            { 3, "bm_src_colour" },
            { 4, "bm_inv_src_colour" },
            { 5, "bm_src_alpha" },
            { 6, "bm_inv_src_alpha" },
            { 7, "bm_dest_alpha" },
            { 8, "bm_inv_dest_alpha" },
            { 9, "bm_dest_colour" },
            { 10, "bm_inv_dest_colour" },
            { 11, "bm_src_alpha_sat" },
        }
    },
    {
        BufferFormat, {
            { 1, "buffer_u8" },
            { 2, "buffer_s8" },
            { 3, "buffer_u16" },
            { 4, "buffer_s16" },
            { 5, "buffer_u32" },
            { 6, "buffer_s32" },
            { 7, "buffer_f16" },
            { 8, "buffer_f32" },
            { 9, "buffer_f64" },
            { 10, "buffer_bool" },
            { 11, "buffer_string" },
            { 12, "buffer_u64" },
            { 13, "buffer_text" },
        }
    },
    {
        BufferType, {
            { 0, "buffer_fixed" },
            { 1, "buffer_grow" },
            { 2, "buffer_wrap" },
            { 3, "buffer_fast" },
            { 4, "buffer_vbuffer" },
        }
    },
    {
        BufferSeekBase, {
            { 0, "buffer_seek_start" },
            { 1, "buffer_seek_relative" },
            { 2, "buffer_seek_end" },
        }
    },
    {
        Cursor, {
            { 0, "cr_default" },
            { -1, "cr_none" },
            { -2, "cr_arrow" },
            { -3, "cr_cross" },
            { -4, "cr_beam" },
            { -6, "cr_size_nesw" },
            { -7, "cr_size_ns" },
            { -8, "cr_size_nwse" },
            { -9, "cr_size_we" },
            { -10, "cr_uparrow" },
            { -11, "cr_hourglass" },
            { -12, "cr_drag" },
            { -19, "cr_appstart" },
            { -21, "cr_handpoint" },
            { -22, "cr_size_all" },
        }
    },
    {
        Primitive, {
            { 1, "pr_pointlist" },
            { 2, "pr_linelist" },
            { 3, "pr_linestrip" },
            { 4, "pr_trianglelist" },
            { 5, "pr_trianglestrip" },
            { 6, "pr_trianglefan" },
        }
    },
    {
        Timezone, {
            { 0, "timezone_local" },
            { 1, "timezone_utc" },
        }
    },
    {
        VBMethod, {
            { 0, "vbm_fast" },
            { 1, "vbm_compatible" },
            { 2, "vbm_most_compatible" },
        }
    },
    {
        CallConv, {
            { 0, "dll_cdecl" },
            { 1, "dll_stdcall" },

        }
    },
    {
        CallArg, {
            { 0, "ty_real" },
            { 1, "ty_string" },
        }
    },
    {
        DataStructure, {
            { 1, "ds_type_map" },
            { 2, "ds_type_list" },
            { 3, "ds_type_stack" },
            { 5, "ds_type_grid" },
            { 4, "ds_type_queue" },
            { 6, "ds_type_priority" },
        }
    },
    {
        Effect, {
            { 0, "ef_explosion" },
            { 1, "ef_ring" },
            { 2, "ef_ellipse" },
            { 3, "ef_firework" },
            { 4, "ef_smoke" },
            { 5, "ef_smokeup" },
            { 6, "ef_star" },
            { 7, "ef_spark" },
            { 8, "ef_flare" },
            { 9, "ef_cloud" },
            { 10, "ef_rain" },
            { 11, "ef_snow" },
        }
    },
    {
        Event, {
            { 0, "ev_create" },
            { 1, "ev_destroy" },
            { 3, "ev_step" },
            { 2, "ev_alarm" },
            { 5, "ev_keyboard" },
            { 6, "ev_mouse" },
            { 4, "ev_collision" },
            { 7, "ev_other" },
            { 8, "ev_draw" },
            { 9, "ev_keypress" },
            { 10, "ev_keyrelease" },
        }
    },
    {
        FbLogin, {
            { 0, "fb_login_default" },
            { 1, "fb_login_fallback_to_webview" },
            { 2, "fb_login_no_fallback_to_webview" },
            { 3, "fb_login_forcing_webview" },
            { 4, "fb_login_use_system_account" },
            { 5, "fb_login_forcing_safari" },
        }
    },
    {
        LBSort, {
            { 0, "lb_sort_none" },
            { 1, "lb_sort_ascending" },
            { 2, "lb_sort_descending" },
        }
    },
    {
        LBDisplay, {
            { 0, "lb_disp_none" },
            { 1, "lb_disp_numeric" },
            { 2, "lb_disp_time_sec" },
            { 3, "lb_disp_time_ms" },
        }
    },
    {
        Matrix, {
            { 0, "matrix_view" },
            { 1, "matrix_projection" },
            { 2, "matrix_world" },
        }
    },
    {
        NetworkConf, {
            { 0, "network_config_connect_timeout" },
            { 1, "network_config_use_non_blocking_socket" },
        }
    },
    {
        NetworkType, {
            { 0, "network_socket_tcp" },
            { 1, "network_socket_udp" },
            { 2, "network_socket_bluetooth" },
        }
    },
    {
        SteamOverlay, {
            { 0, "ov_friends" },
            { 1, "ov_community" },
            { 2, "ov_players" },
            { 3, "ov_settings" },
            { 4, "ov_gamegroup" },
            { 5, "ov_achievements" },
        }
    },
    {
        EmitterShape, {
            { 0, "ps_shape_rectangle" },
            { 1, "ps_shape_ellipse" },
            { 2, "ps_shape_diamond" },
            { 3, "ps_shape_line" },
        }
    },
    {
        EmitterDistr, {
            { 0, "ps_distr_linear" },
            { 1, "ps_distr_gaussian" },
            { 2, "ps_distr_invgaussian" },
        }
    },
    {
        PartShape, {
            { 0, "pt_shape_pixel" },
            { 1, "pt_shape_disk" },
            { 2, "pt_shape_square" },
            { 3, "pt_shape_line" },
            { 4, "pt_shape_star" },
            { 5, "pt_shape_circle" },
            { 6, "pt_shape_ring" },
            { 7, "pt_shape_sphere" },
            { 8, "pt_shape_flare" },
            { 9, "pt_shape_spark" },
            { 10, "pt_shape_explosion" },
            { 11, "pt_shape_cloud" },
            { 12, "pt_shape_smoke" },
            { 13, "pt_shape_snow" },
        }
    },
    {
        JointProp, {
            { 0, "phy_joint_anchor_1_x" },
            { 1, "phy_joint_anchor_1_y" },
            { 2, "phy_joint_anchor_2_x" },
            { 3, "phy_joint_anchor_2_y" },
            { 4, "phy_joint_reaction_force_x" },
            { 5, "phy_joint_reaction_force_y" },
            { 6, "phy_joint_reaction_torque" },
            { 7, "phy_joint_motor_speed" },
            { 8, "phy_joint_angle" },
            { 9, "phy_joint_motor_torque" },
            { 10, "phy_joint_max_motor_torque" },
            { 11, "phy_joint_translation" },
            { 12, "phy_joint_speed" },
            { 13, "phy_joint_motor_force" },
            { 14, "phy_joint_max_motor_force" },
            { 15, "phy_joint_length_1" },
            { 16, "phy_joint_length_2" },
            { 17, "phy_joint_damping_ratio" },
            { 18, "phy_joint_frequency" },
            { 19, "phy_joint_lower_angle_limit" },
            { 20, "phy_joint_upper_angle_limit" },
            { 21, "phy_joint_angle_limits" },
            { 22, "phy_joint_max_length" },
            { 23, "phy_joint_max_torque" },
            { 24, "phy_joint_max_force" },
        }
    },
    {
        UGCFiletype, {
            { 0, "ugc_filetype_community" },
            { 1, "ugc_filetype_microtrans" },
        }
    },
    {
        UGCQuery, {
            { 0, "ugc_query_RankedByVote" },
            { 1, "ugc_query_RankedByPublicationDate" },
            { 2, "ugc_query_AcceptedForGameRankedByAcceptanceDate" },
            { 3, "ugc_query_RankedByTrend" },
            { 4, "ugc_query_FavoritedByFriendsRankedByPublicationDate" },
            { 5, "ugc_query_CreatedByFriendsRankedByPublicationDate" },
            { 6, "ugc_query_RankedByNumTimesReported" },
            { 7, "ugc_query_CreatedByFollowedUsersRankedByPublicationDate" },
            { 8, "ugc_query_NotYetRated" },
            { 9, "ugc_query_RankedByTotalVotesAsc" },
            { 10, "ugc_query_RankedByVotesUp" },
            { 11, "ugc_query_RankedByTextSearch" },
        }
    },
    {
        UGCMatch, {
            { 0, "ugc_match_Items" },
            { 1, "ugc_match_Items_Mtx" },
            { 2, "ugc_match_Items_ReadyToUse" },
            { 3, "ugc_match_Collections" },
            { 4, "ugc_match_Artwork" },
            { 5, "ugc_match_Videos" },
            { 6, "ugc_match_Screenshots" },
            { 7, "ugc_match_AllGuides" },
            { 8, "ugc_match_WebGuides" },
            { 9, "ugc_match_IntegratedGuides" },
            { 10, "ugc_match_UsableInGame" },
            { 11, "ugc_match_ControllerBindings" },
        }
    },
    {
        UGCList, {
            { 0, "ugc_list_Published" },
            { 1, "ugc_list_VotedOn" },
            { 2, "ugc_list_VotedUp" },
            { 3, "ugc_list_VotedDown" },
            { 4, "ugc_list_WillVoteLater" },
            { 5, "ugc_list_Favorited" },
            { 6, "ugc_list_Subscribed" },
            { 7, "ugc_list_UsedOrPlayed" },
            { 8, "ugc_list_Followed" },
        }
    },
    {
        UGCSort, {
            { 0, "ugc_sortorder_CreationOrderDesc" },
            { 1, "ugc_sortorder_CreationOrderAsc" },
            { 2, "ugc_sortorder_TitleAsc" },
            { 3, "ugc_sortorder_LastUpdatedDesc" },
            { 4, "ugc_sortorder_SubscriptionDateDesc" },
            { 5, "ugc_sortorder_VoteScoreDesc" },
            { 6, "ugc_sortorder_ForModeration" },
        }
    },
    {
        UGCVisibility, {
            { 0, "ugc_visibility_public" },
            { 1, "ugc_visibility_friends_only" },
            { 2, "ugc_visibility_private" },
        }
    },
    {
        VType, {
            { 1, "vertex_type_float1" },
            { 2, "vertex_type_float2" },
            { 3, "vertex_type_float3" },
            { 4, "vertex_type_float4" },
            { 5, "vertex_type_colour" },
            { 6, "vertex_type_ubyte4" },
        }
    },
    {
        VUsage, {
            { 1, "vertex_usage_position" },
            { 2, "vertex_usage_colour" },
            { 3, "vertex_usage_normal" },
            { 4, "vertex_usage_textcoord" },
            { 5, "vertex_usage_blendweight" },
            { 6, "vertex_usage_blendindices" },
            { 13, "vertex_usage_depth" },
            { 8, "vertex_usage_tangent" },
            { 9, "vertex_usage_binormal" },
            { 12, "vertex_usage_fog" },
            { 14, "vertex_usage_sample" },
        }
    },
};
