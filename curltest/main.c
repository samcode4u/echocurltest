#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#define CHUNK 0x4000

#define CALL_ZLIB(x)                                            \
    {                                                           \
        int status;                                             \
        status = x;                                             \
        if (status < 0)                                         \
        {                                                       \
            fprintf(stderr,                                     \
                    "%s:%d: %s returned a bad status of %d.\n", \
                    __FILE__, __LINE__, #x, status);            \
            exit(EXIT_FAILURE);                                 \
        }                                                       \
    }

#define windowBits 15
#define GZIP_ENCODING 16

static void strm_init(z_stream *strm)
{
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;
    CALL_ZLIB(deflateInit2(strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                           windowBits | GZIP_ENCODING, 8,
                           Z_DEFAULT_STRATEGY));
}

/* Example josn text */
static const char *message = "{\n"
                             "  \"core-uuid\": \"5f248ed6-8255-49c0-b8ab-5b49b6915a38\",\n"
                             "  \"switchname\": \"9f723054b7a1\",\n"
                             "  \"channel_data\": {\n"
                             "    \"state\": \"CS_REPORTING\",\n"
                             "    \"direction\": \"inbound\",\n"
                             "    \"state_number\": \"11\",\n"
                             "    \"flags\": \"0=1;3=1;38=1;39=1;41=1;54=1;76=1;96=1;113=1;114=1;123=1;160=1;165=1;166=1\",\n"
                             "    \"caps\": \"1=1;2=1;3=1;4=1;5=1;6=1;8=1;9=1\"\n"
                             "  },\n"
                             "  \"callStats\": {\n"
                             "    \"audio\": {\n"
                             "      \"inbound\": {\n"
                             "        \"raw_bytes\": 0,\n"
                             "        \"media_bytes\": 0,\n"
                             "        \"packet_count\": 0,\n"
                             "        \"media_packet_count\": 0,\n"
                             "        \"skip_packet_count\": 1089,\n"
                             "        \"jitter_packet_count\": 0,\n"
                             "        \"dtmf_packet_count\": 0,\n"
                             "        \"cng_packet_count\": 0,\n"
                             "        \"flush_packet_count\": 0,\n"
                             "        \"largest_jb_size\": 0,\n"
                             "        \"jitter_min_variance\": 0,\n"
                             "        \"jitter_max_variance\": 0,\n"
                             "        \"jitter_loss_rate\": 0,\n"
                             "        \"jitter_burst_rate\": 0,\n"
                             "        \"mean_interval\": 0,\n"
                             "        \"flaw_total\": 0,\n"
                             "        \"quality_percentage\": 100,\n"
                             "        \"mos\": 4.5\n"
                             "      },\n"
                             "      \"outbound\": {\n"
                             "        \"raw_bytes\": 187136,\n"
                             "        \"media_bytes\": 187136,\n"
                             "        \"packet_count\": 1088,\n"
                             "        \"media_packet_count\": 1088,\n"
                             "        \"skip_packet_count\": 0,\n"
                             "        \"dtmf_packet_count\": 0,\n"
                             "        \"cng_packet_count\": 0,\n"
                             "        \"rtcp_packet_count\": 0,\n"
                             "        \"rtcp_octet_count\": 0\n"
                             "      }\n"
                             "    }\n"
                             "  },\n"
                             "  \"variables\": {\n"
                             "    \"direction\": \"inbound\",\n"
                             "    \"uuid\": \"eb17bf5d-ce1f-4ea2-932e-54a9c87752c5\",\n"
                             "    \"session_id\": \"4303\",\n"
                             "    \"sip_from_user\": \"%2B12172824030\",\n"
                             "    \"sip_from_uri\": \"%2B12172824030%40148.251.91.106\",\n"
                             "    \"sip_from_host\": \"148.251.91.106\",\n"
                             "    \"video_media_flow\": \"disabled\",\n"
                             "    \"text_media_flow\": \"disabled\",\n"
                             "    \"channel_name\": \"sofia/internal/%2B12172824030%40148.251.91.106\",\n"
                             "    \"sip_call_id\": \"672ab3363c71f6816bf330fa4522a7bd%40209.126.65.30%3A5060-b2b_2\",\n"
                             "    \"sip_local_network_addr\": \"172.18.1.1\",\n"
                             "    \"sip_network_ip\": \"172.18.2.1\",\n"
                             "    \"sip_network_port\": \"5060\",\n"
                             "    \"sip_invite_stamp\": \"1616506937180894\",\n"
                             "    \"sip_received_ip\": \"172.18.2.1\",\n"
                             "    \"sip_received_port\": \"5060\",\n"
                             "    \"sip_via_protocol\": \"udp\",\n"
                             "    \"sip_authorized\": \"true\",\n"
                             "    \"sip_acl_authed_by\": \"domains\",\n"
                             "    \"sip_from_user_stripped\": \"12172824030\",\n"
                             "    \"sip_from_tag\": \"55c7e54a11183b12b1e9df5b1a6db6fb\",\n"
                             "    \"sofia_profile_name\": \"internal\",\n"
                             "    \"sofia_profile_url\": \"sip%3Amod_sofia%40172.18.1.1%3A6060\",\n"
                             "    \"recovery_profile_name\": \"internal\",\n"
                             "    \"sip_Remote-Party-ID\": \"V3230542150008883482%20%3Csip%3A%2B12172824030%40148.251.91.106%3E%3Bprivacy%3Doff%3Bscreen%3Dno\",\n"
                             "    \"sip_cid_type\": \"rpid\",\n"
                             "    \"sip_invite_route_uri\": \"%3Csip%3A192.124.224.73%3Blr%3Bftag%3D55c7e54a11183b12b1e9df5b1a6db6fb%3Bdid%3Ddc7.7c6%3E\",\n"
                             "    \"sip_invite_record_route\": \"%3Csip%3A192.124.224.73%3Blr%3Bftag%3D55c7e54a11183b12b1e9df5b1a6db6fb%3Bdid%3Ddc7.7c6%3E\",\n"
                             "    \"sip_full_via\": \"SIP/2.0/UDP%20192.124.224.73%3A5060%3Bbranch%3Dz9hG4bK6367.c339dec56ab602aa98bae1b7c13838f6.0%3Breceived%3D172.18.2.1,SIP/2.0/UDP%20148.251.91.106%3Brport%3D5060%3Bbranch%3Dz9hG4bK6367.f735b2f67dc4a05755a40c17176622f8.0,SIP/2.0/UDP%20148.251.91.106%3A5067%3Bbranch%3Dz9hG4bK8669bc6b379415124ced515e175c27bd%3Brport%3D5067\",\n"
                             "    \"sip_from_display\": \"V3230542150008883482\",\n"
                             "    \"sip_full_from\": \"V3230542150008883482%20%3Csip%3A%2B12172824030%40148.251.91.106%3E%3Btag%3D55c7e54a11183b12b1e9df5b1a6db6fb\",\n"
                             "    \"sip_full_to\": \"%3Csip%3A18324127874%40192.124.224.73%3E\",\n"
                             "    \"sip_req_user\": \"18324127874\",\n"
                             "    \"sip_req_port\": \"6060\",\n"
                             "    \"sip_req_uri\": \"18324127874%40172.18.1.1%3A6060\",\n"
                             "    \"sip_req_host\": \"172.18.1.1\",\n"
                             "    \"sip_to_user\": \"18324127874\",\n"
                             "    \"sip_to_uri\": \"18324127874%40192.124.224.73\",\n"
                             "    \"sip_to_host\": \"192.124.224.73\",\n"
                             "    \"sip_contact_user\": \"12172824030\",\n"
                             "    \"sip_contact_port\": \"5067\",\n"
                             "    \"sip_contact_uri\": \"12172824030%40148.251.91.106%3A5067\",\n"
                             "    \"sip_contact_host\": \"148.251.91.106\",\n"
                             "    \"sip_user_agent\": \"Sippy%20Softswitch%20v5.0-PRODUCTION.192\",\n"
                             "    \"sip_via_host\": \"192.124.224.73\",\n"
                             "    \"sip_via_port\": \"5060\",\n"
                             "    \"max_forwards\": \"68\",\n"
                             "    \"presence_id\": \"%2B12172824030%40148.251.91.106\",\n"
                             "    \"sip_h_X-User\": \"sip%3ATest1615532288846%40phone.vibconnect.io\",\n"
                             "    \"sip_h_X-Call-Type\": \"sip\",\n"
                             "    \"switch_r_sdp\": \"v%3D0%0D%0Ao%3Droot%202145621670%202145621671%20IN%20IP4%20209.126.65.30%0D%0As%3DAsterisk%20PBX%201.8.23.0-vici%0D%0Ac%3DIN%20IP4%20209.126.65.30%0D%0At%3D0%200%0D%0Am%3Daudio%2012662%20RTP/AVP%200%20101%0D%0Aa%3Drtpmap%3A0%20PCMU/8000%0D%0Aa%3Drtpmap%3A101%20telephone-event/8000%0D%0Aa%3Dfmtp%3A101%200-16%0D%0Aa%3Dptime%3A20%0D%0A\",\n"
                             "    \"ep_codec_string\": \"CORE_PCM_MODULE.PCMU%408000h%4020i%4064000b\",\n"
                             "    \"call_uuid\": \"eb17bf5d-ce1f-4ea2-932e-54a9c87752c5\",\n"
                             "    \"call_type\": \"Pstn\",\n"
                             "    \"continue_on_fail\": \"true\",\n"
                             "    \"sip_user\": \"sip%3ATest1615532288846%40phone.vibconnect.io\",\n"
                             "    \"rtcp_mux\": \"false\",\n"
                             "    \"call_sid\": \"eb17bf5d-ce1f-4ea2-932e-54a9c87752c5\",\n"
                             "    \"rtcp_audio_interval_msec\": \"5000\",\n"
                             "    \"hangup_after_bridge\": \"false\",\n"
                             "    \"export_vars\": \"continue_on_fail,rtcp_mux,rtcp_audio_interval_msec,hangup_after_bridge\",\n"
                             "    \"sofia_session_timeout\": \"0\",\n"
                             "    \"send_silence_when_idle\": \"-1\",\n"
                             "    \"ringback\": \"%25(2000,4000,440.0,480.0)\",\n"
                             "    \"instant_ringback\": \"true\",\n"
                             "    \"rtp_use_codec_string\": \"PCMU,PCMA,OPUS,VP8\",\n"
                             "    \"remote_video_media_flow\": \"inactive\",\n"
                             "    \"remote_text_media_flow\": \"inactive\",\n"
                             "    \"remote_audio_media_flow\": \"sendrecv\",\n"
                             "    \"audio_media_flow\": \"sendrecv\",\n"
                             "    \"rtp_audio_recv_pt\": \"0\",\n"
                             "    \"rtp_use_codec_name\": \"PCMU\",\n"
                             "    \"rtp_use_codec_rate\": \"8000\",\n"
                             "    \"rtp_use_codec_ptime\": \"20\",\n"
                             "    \"rtp_use_codec_channels\": \"1\",\n"
                             "    \"rtp_last_audio_codec_string\": \"PCMU%408000h%4020i%401c\",\n"
                             "    \"original_read_codec\": \"PCMU\",\n"
                             "    \"original_read_rate\": \"8000\",\n"
                             "    \"write_codec\": \"PCMU\",\n"
                             "    \"write_rate\": \"8000\",\n"
                             "    \"dtmf_type\": \"rfc2833\",\n"
                             "    \"local_media_ip\": \"172.18.1.1\",\n"
                             "    \"local_media_port\": \"18416\",\n"
                             "    \"advertised_media_ip\": \"192.124.224.73\",\n"
                             "    \"rtp_use_timer_name\": \"soft\",\n"
                             "    \"rtp_use_pt\": \"0\",\n"
                             "    \"rtp_use_ssrc\": \"948876849\",\n"
                             "    \"rtp_2833_send_payload\": \"101\",\n"
                             "    \"rtp_2833_recv_payload\": \"101\",\n"
                             "    \"remote_media_ip\": \"209.126.65.30\",\n"
                             "    \"remote_media_port\": \"12662\",\n"
                             "    \"endpoint_disposition\": \"EARLY%20MEDIA\",\n"
                             "    \"rtp_local_sdp_str\": \"v%3D0%0D%0Ao%3DFreeSWITCH%201616488522%201616488523%20IN%20IP4%20192.124.224.73%0D%0As%3DFreeSWITCH%0D%0Ac%3DIN%20IP4%20192.124.224.73%0D%0At%3D0%200%0D%0Am%3Daudio%2018416%20RTP/AVP%200%20101%0D%0Aa%3Drtpmap%3A0%20PCMU/8000%0D%0Aa%3Drtpmap%3A101%20telephone-event/8000%0D%0Aa%3Dfmtp%3A101%200-16%0D%0Aa%3Dptime%3A20%0D%0Aa%3Dsendrecv%0D%0Aa%3Drtcp%3A18417%20IN%20IP4%20192.124.224.73%0D%0A\",\n"
                             "    \"sip_nat_detected\": \"true\",\n"
                             "    \"originate_signal_bond\": \"a76a7001-7bc3-4d8a-bea2-04a8b103a220\",\n"
                             "    \"originated_legs\": \"a76a7001-7bc3-4d8a-bea2-04a8b103a220%3BOutbound%20Call%3B18324127874\",\n"
                             "    \"last_bridge_hangup_cause\": \"ALLOTTED_TIMEOUT\",\n"
                             "    \"read_codec\": \"PCMU\",\n"
                             "    \"read_rate\": \"8000\",\n"
                             "    \"originate_disposition\": \"ALLOTTED_TIMEOUT\",\n"
                             "    \"DIALSTATUS\": \"ALLOTTED_TIMEOUT\",\n"
                             "    \"originate_causes\": \"a76a7001-7bc3-4d8a-bea2-04a8b103a220%3BALLOTTED_TIMEOUT\",\n"
                             "    \"originate_failed_cause\": \"ALLOTTED_TIMEOUT\",\n"
                             "    \"current_application_data\": \"XML_CallFlow_Complete\",\n"
                             "    \"current_application\": \"hangup\",\n"
                             "    \"hangup_cause\": \"NORMAL_CLEARING\",\n"
                             "    \"hangup_cause_q850\": \"16\",\n"
                             "    \"digits_dialed\": \"none\",\n"
                             "    \"start_stamp\": \"2021-03-23%2013%3A42%3A17\",\n"
                             "    \"profile_start_stamp\": \"2021-03-23%2013%3A42%3A17\",\n"
                             "    \"progress_stamp\": \"2021-03-23%2013%3A42%3A21\",\n"
                             "    \"progress_media_stamp\": \"2021-03-23%2013%3A42%3A18\",\n"
                             "    \"end_stamp\": \"2021-03-23%2013%3A42%3A40\",\n"
                             "    \"start_epoch\": \"1616506937\",\n"
                             "    \"start_uepoch\": \"1616506937180894\",\n"
                             "    \"profile_start_epoch\": \"1616506937\",\n"
                             "    \"profile_start_uepoch\": \"1616506937180894\",\n"
                             "    \"answer_epoch\": \"0\",\n"
                             "    \"answer_uepoch\": \"0\",\n"
                             "    \"bridge_epoch\": \"0\",\n"
                             "    \"bridge_uepoch\": \"0\",\n"
                             "    \"last_hold_epoch\": \"0\",\n"
                             "    \"last_hold_uepoch\": \"0\",\n"
                             "    \"hold_accum_seconds\": \"0\",\n"
                             "    \"hold_accum_usec\": \"0\",\n"
                             "    \"hold_accum_ms\": \"0\",\n"
                             "    \"resurrect_epoch\": \"0\",\n"
                             "    \"resurrect_uepoch\": \"0\",\n"
                             "    \"progress_epoch\": \"1616506941\",\n"
                             "    \"progress_uepoch\": \"1616506941180868\",\n"
                             "    \"progress_media_epoch\": \"1616506938\",\n"
                             "    \"progress_media_uepoch\": \"1616506938240855\",\n"
                             "    \"end_epoch\": \"1616506960\",\n"
                             "    \"end_uepoch\": \"1616506960020890\",\n"
                             "    \"last_app\": \"hangup\",\n"
                             "    \"last_arg\": \"XML_CallFlow_Complete\",\n"
                             "    \"caller_id\": \"%22V3230542150008883482%22%20%3C%2B12172824030%3E\",\n"
                             "    \"duration\": \"23\",\n"
                             "    \"billsec\": \"0\",\n"
                             "    \"progresssec\": \"4\",\n"
                             "    \"answersec\": \"0\",\n"
                             "    \"waitsec\": \"0\",\n"
                             "    \"progress_mediasec\": \"1\",\n"
                             "    \"flow_billsec\": \"0\",\n"
                             "    \"mduration\": \"22840\",\n"
                             "    \"billmsec\": \"0\",\n"
                             "    \"progressmsec\": \"4000\",\n"
                             "    \"answermsec\": \"0\",\n"
                             "    \"waitmsec\": \"0\",\n"
                             "    \"progress_mediamsec\": \"1060\",\n"
                             "    \"flow_billmsec\": \"0\",\n"
                             "    \"uduration\": \"22839996\",\n"
                             "    \"billusec\": \"0\",\n"
                             "    \"progressusec\": \"3999974\",\n"
                             "    \"answerusec\": \"0\",\n"
                             "    \"waitusec\": \"0\",\n"
                             "    \"progress_mediausec\": \"1059961\",\n"
                             "    \"flow_billusec\": \"0\",\n"
                             "    \"sip_hangup_disposition\": \"send_refuse\",\n"
                             "    \"rtp_audio_in_raw_bytes\": \"0\",\n"
                             "    \"rtp_audio_in_media_bytes\": \"0\",\n"
                             "    \"rtp_audio_in_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_media_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_skip_packet_count\": \"1089\",\n"
                             "    \"rtp_audio_in_jitter_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_dtmf_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_cng_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_flush_packet_count\": \"0\",\n"
                             "    \"rtp_audio_in_largest_jb_size\": \"0\",\n"
                             "    \"rtp_audio_in_jitter_min_variance\": \"0.00\",\n"
                             "    \"rtp_audio_in_jitter_max_variance\": \"0.00\",\n"
                             "    \"rtp_audio_in_jitter_loss_rate\": \"0.00\",\n"
                             "    \"rtp_audio_in_jitter_burst_rate\": \"0.00\",\n"
                             "    \"rtp_audio_in_mean_interval\": \"0.00\",\n"
                             "    \"rtp_audio_in_flaw_total\": \"0\",\n"
                             "    \"rtp_audio_in_quality_percentage\": \"100.00\",\n"
                             "    \"rtp_audio_in_mos\": \"4.50\",\n"
                             "    \"rtp_audio_out_raw_bytes\": \"187136\",\n"
                             "    \"rtp_audio_out_media_bytes\": \"187136\",\n"
                             "    \"rtp_audio_out_packet_count\": \"1088\",\n"
                             "    \"rtp_audio_out_media_packet_count\": \"1088\",\n"
                             "    \"rtp_audio_out_skip_packet_count\": \"0\",\n"
                             "    \"rtp_audio_out_dtmf_packet_count\": \"0\",\n"
                             "    \"rtp_audio_out_cng_packet_count\": \"0\",\n"
                             "    \"rtp_audio_rtcp_packet_count\": \"0\",\n"
                             "    \"rtp_audio_rtcp_octet_count\": \"0\"\n"
                             "  },\n"
                             "  \"app_log\": {\n"
                             "    \"applications\": [\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_stamp\": \"1616506937196854\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"call_type=Pstn\",\n"
                             "        \"app_stamp\": \"1616506937197267\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"export\",\n"
                             "        \"app_data\": \"continue_on_fail=true\",\n"
                             "        \"app_stamp\": \"1616506937197676\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"sip_user=sip:Test1615532288846@phone.vibconnect.io\",\n"
                             "        \"app_stamp\": \"1616506937197998\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"park_timeout=86400\",\n"
                             "        \"app_stamp\": \"1616506937198325\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"export\",\n"
                             "        \"app_data\": \"rtcp_mux=false\",\n"
                             "        \"app_stamp\": \"1616506937198640\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"call_sid=eb17bf5d-ce1f-4ea2-932e-54a9c87752c5\",\n"
                             "        \"app_stamp\": \"1616506937198961\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"export\",\n"
                             "        \"app_data\": \"rtcp_audio_interval_msec=5000\",\n"
                             "        \"app_stamp\": \"1616506937199262\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"export\",\n"
                             "        \"app_data\": \"hangup_after_bridge=false\",\n"
                             "        \"app_stamp\": \"1616506937199575\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"sofia_session_timeout=0\",\n"
                             "        \"app_stamp\": \"1616506937199882\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"set\",\n"
                             "        \"app_data\": \"send_silence_when_idle=-1\",\n"
                             "        \"app_stamp\": \"1616506937200197\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"park\",\n"
                             "        \"app_data\": \"\",\n"
                             "        \"app_stamp\": \"1616506937200518\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"multiset\",\n"
                             "        \"app_data\": \"^^!ringback=%(2000,4000,440.0,480.0)!instant_ringback=true\",\n"
                             "        \"app_stamp\": \"1616506938246544\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"pre_answer\",\n"
                             "        \"app_data\": \"\",\n"
                             "        \"app_stamp\": \"1616506938247005\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"bridge\",\n"
                             "        \"app_stamp\": \"1616506939529369\"\n"
                             "      },\n"
                             "      {\n"
                             "        \"app_name\": \"hangup\",\n"
                             "        \"app_data\": \"XML_CallFlow_Complete\",\n"
                             "        \"app_stamp\": \"1616506960030358\"\n"
                             "      }\n"
                             "    ]\n"
                             "  },\n"
                             "  \"callflow\": [\n"
                             "    {\n"
                             "      \"dialplan\": \"XML\",\n"
                             "      \"profile_index\": \"1\",\n"
                             "      \"extension\": {\n"
                             "        \"name\": \"public_did\",\n"
                             "        \"number\": \"18324127874\",\n"
                             "        \"applications\": [\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"call_type=Pstn\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"export\",\n"
                             "            \"app_data\": \"continue_on_fail=true\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"sip_user=${sip_h_X-User}\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"park_timeout=86400\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"export\",\n"
                             "            \"app_data\": \"rtcp_mux=false\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"call_sid=${uuid}\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"export\",\n"
                             "            \"app_data\": \"rtcp_audio_interval_msec=5000\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"export\",\n"
                             "            \"app_data\": \"hangup_after_bridge=false\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"sofia_session_timeout=0\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"set\",\n"
                             "            \"app_data\": \"send_silence_when_idle=-1\"\n"
                             "          },\n"
                             "          {\n"
                             "            \"app_name\": \"park\",\n"
                             "            \"app_data\": \"\"\n"
                             "          }\n"
                             "        ]\n"
                             "      },\n"
                             "      \"caller_profile\": {\n"
                             "        \"username\": \"+12172824030\",\n"
                             "        \"dialplan\": \"XML\",\n"
                             "        \"caller_id_name\": \"V3230542150008883482\",\n"
                             "        \"ani\": \"+12172824030\",\n"
                             "        \"aniii\": \"\",\n"
                             "        \"caller_id_number\": \"+12172824030\",\n"
                             "        \"network_addr\": \"172.18.2.1\",\n"
                             "        \"rdnis\": \"\",\n"
                             "        \"destination_number\": \"18324127874\",\n"
                             "        \"uuid\": \"eb17bf5d-ce1f-4ea2-932e-54a9c87752c5\",\n"
                             "        \"source\": \"mod_sofia\",\n"
                             "        \"chan_name\": \"sofia/internal/+12172824030@148.251.91.106\"\n"
                             "      },\n"
                             "      \"times\": {\n"
                             "        \"created_time\": \"1616506937180894\",\n"
                             "        \"profile_created_time\": \"1616506937180894\",\n"
                             "        \"progress_time\": \"1616506941180868\",\n"
                             "        \"progress_media_time\": \"1616506938240855\",\n"
                             "        \"answered_time\": \"0\",\n"
                             "        \"bridged_time\": \"0\",\n"
                             "        \"last_hold_time\": \"0\",\n"
                             "        \"hold_accum_time\": \"0\",\n"
                             "        \"hangup_time\": \"1616506960020890\",\n"
                             "        \"resurrect_time\": \"0\",\n"
                             "        \"transfer_time\": \"0\"\n"
                             "      }\n"
                             "    }\n"
                             "  ]\n"
                             "}";

static size_t httpCallBack(char *buffer, size_t size, size_t nitems, void *outstream)
{
    return size * nitems;
}

int main()
{
    unsigned char out[CHUNK];
    char agent[1024] = {
        0,
    };
    struct curl_slist *headers = NULL;
    int have;
    z_stream strm;


    int i = 0;

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();

    for (i = 0; i < 5; i++)
    {
        if (curl)
        {
            strm_init(&strm);
            strm.next_in = (unsigned char *)message;
            strm.avail_in = strlen(message);
            do
            {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                CALL_ZLIB(deflate(&strm, Z_FINISH));
                have = CHUNK - strm.avail_out;
                printf("\ninputlength=%lu outputlength=%d strm.avail_out=%d\n", strlen(message), have, strm.avail_out);
            } while (strm.avail_out == 0);
            deflateEnd(&strm);

            curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
            snprintf(agent, sizeof(agent), "libcurl/%s",
                     curl_version_info(CURLVERSION_NOW)->version);
            agent[sizeof(agent) - 1] = 0;
            curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);
            headers = curl_slist_append(headers, "Expect:");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Content-Encoding: gzip");
            curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:1323/");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, out);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, have);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpCallBack);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
        }
    }

    /* always cleanup */
    curl_easy_cleanup(curl);

    curl_slist_free_all(headers);
    curl_global_cleanup();
    return 0;
}
